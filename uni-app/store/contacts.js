import {
	defineStore
} from "pinia";
import crimsdk, {
	IMEvents,
	GroupStatus,
	ApplicationHandleResult,
	GroupMemberFilter,
} from "@cloudroom/crimsdk";
import useUserStore from "./user";
import api from "@/api";
import {
	formatChooseData
} from "@/utils";
// import {
// 	friends
// } from "@/testJson/friend";

let isInitListener = false;
const useContactsStore = defineStore("contacts", {
	state: () => {
		return {
			grpList: [],
			grpReqList: [],
			grpReqCount: 0,
			groupsMemberMap: {},
			friendList: [], // 好友列表
			friendReqList: [], // 我发起的好友申请
			friendRecipientList: [], // 好友申请列表
			friendReqCount: 0,
			blackList: [], // 黑名单列表
			currentGrpInfo: {}, // 当前聊天我的群信息
			currentGrpMemberInfo: null, // 当前聊天我的群成员信息
		}
	},
	getters: {
		userInfo: () => useUserStore().userInfo,
		unreadCount(state) {
			return state.grpReqCount + state.friendReqCount;
		},
		// 群组
		myCreateGrpList(state) {
			return state.grpList.filter(item => item.creatorUserID === this.userInfo.userID);
		},
		myJoinGrpList(state) {
			return state.grpList.filter(item => item.creatorUserID !== this.userInfo.userID);
		},
		// 好友
		friendInfoList: (state) => state.friendList.reduce((result, item) => {
			item.friendInfo && result.push(item.friendInfo); // && !item.blackInfo // 需求改为拉黑了也在好友列表里
			return result;
		}, []),
		friendIndexList(state) {
			const data = formatChooseData(this.friendInfoList);
			return {
				list: data.list ?? [],
				anchors: data.anchors ?? []
			};
		},
	},
	actions: {
		initStoreData() {
			this.grpList = [];
			this.grpReqList = [];
			this.grpReqCount = 0;
			this.groupsMemberMap = {};
			this.friendList = []; // 好友列表 
			// this.friendList = friends; // 测试数据
			this.friendReqList = []; // 我发起的好友申请
			this.friendRecipientList = []; // 好友申请列表
			this.friendReqCount = 0;
			this.blackList = []; // 黑名单列表
			this.currentGrpInfo = {};
			this.currentGrpMemberInfo = null;
		},
		initListener() {
			if (isInitListener) return;
			isInitListener = true;
			crimsdk.on(IMEvents.OnGrpReqAdded, ({
				data
			}) => {});
			crimsdk.on(IMEvents.OnGrpReqAccepted, ({
				data
			}) => {});
			crimsdk.on(IMEvents.OnGrpReqRejected, ({
				data
			}) => {});

			// 用户所在群组的数量增加时
			crimsdk.on(IMEvents.OnJoinedGrpAdded, ({
				data
			}) => {
				this.grpList.push(data);
				// 初始化群响应的成员列表
				this.groupsMemberMap[data.groupID] = [];
			});
			// 群信息被改变
			crimsdk.on(IMEvents.OnGrpInfoChanged, ({
				data
			}) => {
				const idx = this.grpList.findIndex(item => item.groupID === data.groupID);
				if (idx > -1) {
					this.grpList[idx] = data;
				}

				// 当前聊天的群信息
				if (data.groupID === this.currentGrpInfo.groupID) {
					Object.assign(this.currentGrpInfo, data);
				}
			});
			// 群解散通知
			crimsdk.on(IMEvents.OnGrpDismissed, ({
				data
			}) => {
				const grp = this.grpList.find(item => item.groupID === data.groupID);
				grp && Object.assign(grp, {
					status: GroupStatus.Dismissed
				});

				// 当前聊天的群信息
				if (data.groupID === this.currentGrpInfo.groupID) {
					Object.assign(this.currentGrpInfo, data);
				}
			});

			// 群成员
			crimsdk.on(IMEvents.OnGrpMemberAdded, ({
				data
			}) => {
				const gid = data.groupID;
				if (!this.groupsMemberMap[gid]) {
					this.groupsMemberMap[gid] = [];
				}
				this.groupsMemberMap[gid].push(data);
			});
			crimsdk.on(IMEvents.OnGrpMemberDeleted, ({
				data
			}) => {
				const gid = data.groupID;
				if (this.groupsMemberMap[gid]) {
					const list = this.groupsMemberMap[gid];
					const idx = list.findIndex(item => item.userID === data.userID);
					idx > -1 && list.splice(idx, 1);
				}
			});
			crimsdk.on(IMEvents.OnGrpMemberInfoChanged, ({
				data
			}) => {
				const gid = data.groupID;
				if (this.groupsMemberMap[gid]) {
					const list = this.groupsMemberMap[gid];
					const idx = list.findIndex(item => item.userID === data.userID);
					idx > -1 && Object.assign(list[idx], data);
				}
				// 当期聊天的群里，我的群成员信息
				if (this.currentGrpMemberInfo && this.currentGrpMemberInfo.groupID === gid && this
					.currentGrpMemberInfo.userID === data.userID) {
					Object.assign(this.currentGrpMemberInfo, data);
				}
			});

			// 好友
			crimsdk.on(IMEvents.OnFriendAdded, ({
				data
			}) => {
				const idx = this.friendList.findIndex(item => item.userID === data.userID);
				if (idx > -1) {
					this.friendList[idx] = data;
				} else {
					this.friendList.push(data);
				}
			});

			// 删好友
			crimsdk.on(IMEvents.OnFriendDeleted, ({
				data
			}) => {
				const idx = this.friendList.findIndex(item => item.friendInfo.userID === data.userID);
				if (idx > -1) {
					this.friendList.splice(idx, 1);
				}
			});

			// 好友信息改变
			crimsdk.on(IMEvents.OnFriendInfoChanged, ({
				data
			}) => {
				const fri = this.friendList.find(item => item.friendInfo.userID === data.userID);
				fri && Object.assign(fri.friendInfo, data);
			});

			// 好友申请
			crimsdk.on(IMEvents.OnFriendReqAdded, ({
				data
			}) => {
				if (data.fromUserID === this.userInfo.userID) {
					// 发起者
					const idx = this.friendReqList.findIndex(item => item.toUserID === data.toUserID);
					if (idx > -1) {
						this.friendReqList[idx] = data;
					} else {
						this.friendReqList.push(data);
					}
				} else {
					// 接收者
					const idx = this.friendRecipientList.findIndex(item => item.fromUserID === data
						.fromUserID);
					if (idx > -1) {
						this.friendRecipientList[idx] = data;
					} else {
						this.friendRecipientList.unshift(data);
					}
					this.setContactsTabBarBadge();
				}
			});
			crimsdk.on(IMEvents.OnFriendReqAccepted, ({
				data
			}) => {
				// fromUserID 发起人ID
				if (data.fromUserID === this.userInfo.userID) {
					// 同意我的好友申请回调
					const bfri = this.friendReqList.find(item => item.toUserID === data.toUserID);
					if (bfri) {
						bfri.handleResult = ApplicationHandleResult.Agree;
					}
				} else {
					// 同意申请好友回调
					const fri = this.friendRecipientList.find(item => item.fromUserID === data
						.fromUserID);
					fri.handleResult = ApplicationHandleResult.Agree;
					this.setContactsTabBarBadge();
				}
				// 更新好友
				// const { data } = await crimsdk.getSpecifiedFriendsInfo([data.fromUserID === uid ? data.toUserID : data.fromUserID])
				// 更新好友列表
				this.getFriendList();
			});
			crimsdk.on(IMEvents.OnFriendReqRejected, ({
				data
			}) => {
				if (data.fromUserID === this.userInfo.userID) {
					// 拒绝我的好友申请回调
					const bfri = this.friendReqList.find(item => item.toUserID === data.toUserID);
					if (bfri) {
						bfri.handleResult = ApplicationHandleResult.Reject;
					}
				} else {
					// 拒绝申请好友回调
					const fri = this.friendRecipientList.find(item => item.fromUserID === data
						.fromUserID);
					fri.handleResult = ApplicationHandleResult.Reject;
					this.setContactsTabBarBadge();
				}
			});

			// 黑名单
			crimsdk.on(IMEvents.OnBlackAdded, ({
				data
			}) => {
				const idx = this.blackList.findIndex(item => item.userID === data.userID);
				if (idx > -1) {
					this.blackList[idx] = data;
				} else {
					this.blackList.push(data);
				}
			});
			crimsdk.on(IMEvents.OnBlackDeleted, ({
				data
			}) => {
				const idx = this.blackList.findIndex(item => item.userID === data.userID);
				if (idx > -1) {
					this.blackList.splice(idx, 1);
				}
			});
		},
		setContactsTabBarBadge() {
			// this.grpReqCount = this.grpReqList.reduce((result, fri) => {
			// 	if (grp.handleResult === ApplicationHandleResult.Unprocessed) {
			// 		result++;
			// 	}
			// 	return result;
			// }, 0);
			this.friendReqCount = this.friendRecipientList.reduce((result, fri) => {
				if (fri.handleResult === ApplicationHandleResult.Unprocessed) {
					result++;
				}
				return result;
			}, 0);
		},
		// 我的群组列表
		async getGrpList() {
			const {
				data
			} = await crimsdk.getJoinedGrpList();
			this.grpList = data;
		},
		// 获取指定群信息
		async getSpecifiedGrpsInfo(gid) {
			const grp = this.grpList.find(item => item.groupID === gid);
			if (!grp) {
				const {
					data
				} = await crimsdk.getSpecifiedGrpsInfo([gid]);
				return data.length > 0 ? data[0] : null;
			}
			return grp;
		},
		async getGrpMemberList(groupID) {
			const members = this.groupsMemberMap[groupID];
			if (!members) {
				const {
					data
				} = await crimsdk.getGrpMemberList({
					groupID,
					filter: GroupMemberFilter.All,
					offset: 0,
					count: 1000,
				});
				this.groupsMemberMap[groupID] = data;
				return data;
			}
			return members;
		},
		// 获取指定群信息
		async getSpecifiedGrpMemberInfo(gid) {
			const {
				data
			} = await crimsdk.getSpecifiedGrpMembersInfo({
				groupID: gid,
				userIDList: [this.userInfo.userID]
			});
			return data.length > 0 ? data[0] : null;
		},
		// 获取当前聊天我的群信息
		// 获取当前聊天我的群成员信息
		async initCurrentGrpAndMyMemInfo(groupID) {
			if (groupID) {
				this.currentGrpMemberInfo = await this.getSpecifiedGrpMemberInfo(groupID);
				this.currentGrpInfo = await this.getSpecifiedGrpsInfo(groupID);
			} else {
				this.currentGrpInfo = {};
				this.currentGrpMemberInfo = null;
			}
		},
		async getGrpReqListAsRecipient() {
			const {
				data
			} = await crimsdk.getGrpReqListAsRecipient();
			this.grpReqList = data;
			this.setContactsTabBarBadge();
		},
		// 我的好友列表
		async getFriendList() {
			const {
				data
			} = await crimsdk.getFriendList();
			this.friendList = data;
		},
		async getUserInfoById(uid) {
			const {
				users
			} = await api.searchUserInfo(uid);
			const fri = this.friendList.find(item => item.friendInfo.userID === uid);
			Object.assign(fri.friendInfo, users[0]);
			return fri;
		},
		async getBlackList() {
			const {
				data
			} = await crimsdk.getBlackList();
			this.blackList = data;
		},
		async getFriendReqListAsApplicant() {
			const {
				data
			} = await crimsdk.getFriendReqListAsApplicant();
			this.friendReqList = data;
		},
		// 获取好友申请列表
		async getFriendReqListAsRecipient() {
			const {
				data
			} = await crimsdk.getFriendReqListAsRecipient();
			this.friendRecipientList = data;
			this.setContactsTabBarBadge();
		},
		// 查看好友请求，本地处理badge的显示
		lookFriendReq(fromUserID) {
			const fri = this.friendRecipientList.find(item => item.fromUserID === fromUserID);
			if (fri) {
				fri.handleResult = -1; // 自定义
				this.setContactsTabBarBadge();
			}
		},
		// 同意好友申请
		async acceptFriendReq(toUserID, handleMsg) {
			const {
				data
			} = await crimsdk.acceptFriendReq({
				toUserID,
				handleMsg,
			});
			console.log(data);
			this.getFriendList();
		},
		// 拒绝好友申请
		async refuseFriendReq(toUserID, handleMsg) {
			const {
				data
			} = await crimsdk.refuseFriendReq({
				toUserID,
				handleMsg,
			});
		},
		// 给好友设置备注
		async setFriendRemark(uid, remark) {
			const {
				data
			} = await crimsdk.setFriendRemark({
				toUserID: uid,
				remark
			});
		}
	}
})

export default useContactsStore;