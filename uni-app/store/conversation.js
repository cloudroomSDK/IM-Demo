import {
	defineStore
} from "pinia";
import useUserStore from "./user";
import crimsdk, {
	IMEvents,
	SessionType,
	MessageReceiveOptType
} from "@cloudroom/crimsdk";
import {
	appNavigator,
	AppRoutes,
	effect
} from "@/utils";

// import testconversation from "@/testJson/conversation";

let isInitListener = false;
const useConversationStore = defineStore("conversation", {
	state: () => {
		return {
			connectStatus: 0, // [OnConnectFailed, OnConnecting, OnConnectSuccess] [-1, 0, 1] 
			syncServer: false,
			conversationList: [],
			conversationListIsNoData: false,
			unreadMsgCount: 0,
			currentConversation: {},
		}
	},
	getters: {
		userInfo: () => useUserStore().userInfo,
	},
	actions: {
		initStoreData() {
			this.connectStatus = 0;
			this.syncServer = false;
			this.conversationList = [];
			this.conversationListIsNoData = false;
			this.unreadMsgCount = 0;
			this.currentConversation = {
				conversationID: "",
				userID: "",
				groupID: "",
				showName: "",
				faceURL: "",
			};
		},
		initListener() {
			if (isInitListener) {
				return;
			}
			isInitListener = true;

			crimsdk.on(IMEvents.OnConversationChanged, ({
				data
			}) => {
				data.forEach(conv => {
					const idx = this.conversationList.findIndex(item => item
						.conversationID ===
						conv.conversationID);
					const nidx = this.findInsertIndex(conv);
					if (idx > -1) {
						const item = this.conversationList[idx];
						// 更新lastmsg
						if (conv.latestMsgSendTime != item.latestMsgSendTime) {

							// 更新内容、更换位置
							Object.assign(item, conv);
							if (idx !== nidx) {
								const maxIdx = Math.max(idx, nidx);
								const minIdx = Math.min(idx, nidx);
								const item = this.conversationList[maxIdx];
								for (let i = maxIdx; i >= minIdx; i--) {
									this.conversationList[i] = this.conversationList[i - 1];
								}
								this.conversationList[minIdx] = item;
							}

							// 声音或震动
							this.playAudio(conv);
						} else {
							// 没有更新latestMsg, 只更新了群属性
							Object.assign(item, conv);
						}
					} else {
						// 插入置顶列表后的位置
						this.conversationList.splice(nidx, 0, conv);
						this.playAudio(conv);
					}

					// 如果是当前聊天，需要更新
					if (this.currentConversation && this.currentConversation
						.conversationID ===
						conv.conversationID) {
						Object.assign(this.currentConversation, conv);
					}
				});
			});

			// 新会话
			crimsdk.on(IMEvents.OnNewConversation, ({
				data
			}) => {
				data.forEach(conv => {
					const nidx = this.findInsertIndex(conv);
					this.conversationList.splice(nidx, 0, conv);

					// 声音或震动
					this.playAudio(conv);
				})
			});

			crimsdk.on(IMEvents.OnTotalUnreadMsgCountChanged, ({
				data
			}) => {
				this.unreadMsgCount = data;
			});
		},
		playAudio(conv) {
			// 是否当前聊天
			const isCurrChat = appNavigator.isPage(AppRoutes.Chat) && this.currentConversation && this
				.currentConversation
				.conversationID ===
				conv.conversationID;
			if (!isCurrChat) {
				let isMySend = false;
				try {
					const latestMsg = JSON.parse(conv.latestMsg);
					isMySend = latestMsg.sendID === this.userInfo.userID;
				} catch (e) {}
				if (!isMySend) {
					const {
						allowBeep,
						allowVibration,
						globalRecvMsgOpt
					} = this.userInfo;
					if (globalRecvMsgOpt == MessageReceiveOptType.Nomal && conv.recvMsgOpt ==
						MessageReceiveOptType
						.Nomal) {
						uni.$u.debounce(() => {
							// 1 open 2 close
							allowBeep == 1 && effect.play(); // 声音
							allowVibration == 1 && effect.vibrate(); // 震动
						}, 500);
					}
				}
			}
		},
		async getCurrentConversation(params) {
			let oneConversation = this.conversationList.find(conv => conv.conversationID === params
				.conversationID || conv.userID === params.userID ||
				conv
				.groupID === params.groupID);
			if (!oneConversation) {
				const sessionType = params.userID ? SessionType.Single : SessionType.Group;
				const sourceID = params.userID ? params.userID : params.groupID;
				const {
					data
				} = await crimsdk.getOneConversation({
					sessionType,
					sourceID
				});
				oneConversation = data;
			}
			this.currentConversation = oneConversation;
		},
		async getTotalUnreadMsgCount() {
			const {
				data
			} = await crimsdk.getTotalUnreadMsgCount();
			this.unreadMsgCount = data;
		},
		// 插入到会话列表合适的位置
		findInsertIndex(conv) {
			let count = 0;
			for (let i = 0; i < this.conversationList.length; i++) {
				const item = this.conversationList[i];
				// 置顶顺序：先群，再私聊
				// 找到最后一个置顶群，插入群， 找到最后一个置顶私聊，插入私聊
				if (item.isPinned && (item.sessionType === SessionType.Group || conv.sessionType !== SessionType
						.Group)) {
					count++;
				} else {
					break;
				}
			}
			return count;
		},
		async getConversationListSplit(refresh = false) {
			const count = 20;
			const offset = refresh ? 0 : this.conversationList.length;
			try {
				const {
					data
				} = await crimsdk.getConversationListSplit({
					offset,
					count,
				});

				if (refresh) {
					this.conversationList = data;
				} else {
					data.forEach(d => {
						const conv = this.conversationList.find(con => con.conversationID === d
							.conversationID);
						if (conv) {
							Object.assign(conv, d);
						} else {
							this.conversationList.push(d);
						}
					});
				}

				this.conversationListIsNoData = data.length < count;
			} catch (e) {
				this.conversationListIsNoData = true;
			}

			// this.conversationList = JSON.parse(testconversation.data);
			// this.conversationListIsNoData = true;
		},
		// 标记已读
		async markConversationMsgAsRead(conversationID) {
			await crimsdk.markConversationMsgAsRead(conversationID);
			const conversation = this.conversationList.find(item => item.conversationID === conversationID);
			this.unreadMsgCount -= conversation.unreadCount;
			conversation.unreadCount = 0;
		},
		// 删会话
		async deleteConversationAndDeleteAllMsg(conversationID) {
			await crimsdk.deleteConversationAndDeleteAllMsg(conversationID);
			const idx = this.conversationList.findIndex(item => item.conversationID === conversationID);
			this.conversationList.splice(idx, 1)
		},
		// 隐藏会话
		async hideConversation(conversationID) {
			await crimsdk.hideConversation(conversationID);
			const idx = this.conversationList.findIndex(item => item.conversationID === conversationID);
			this.conversationList.splice(idx, 1)
		},
		// 清空聊天记录、删所有会话
		async clearChatHistory() {
			// 删除本地和服务器所有会话中的消息，会话仍会保留。
			await crimsdk.deleteAllMsgFromLocalAndSvr();
			const {
				data
			} = await crimsdk.getAllConversationList();
			data.forEach(async (item) => {
				// 删所有会话
				await crimsdk.deleteConversationAndDeleteAllMsg(item.conversationID);
			});
			this.conversationList = [];
			this.conversationListIsNoData = false;
		}
	}
});

export default useConversationStore;