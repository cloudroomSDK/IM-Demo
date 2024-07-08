import {
	defineStore
} from "pinia";
// import useUserStore from "./user";
import useConversation from "./conversation";
import crimsdk, {
	IMEvents,
	SessionType,
	MessageStatus,
	MessageType,
} from "@cloudroom/crimsdk";
import {
	parseMessageByType,
	filterPreviewImage,
	offlinePushInfo
}
from "@/utils/imUtils";
import {
	GlobalEvents,
} from "@/utils";
// import {
// 	msgData
// } from "@/testJson/msg";

let isInitListener = false;
const useMsgStore = defineStore("msg", {
	state: () => {
		return {
			selfUserID: "",
			// currentConversation: {},
			msgCondition: {},
			msgList: [],
			msgListIsEnd: false,
			quoteMsg: null,
			previewImageList: [],
			insertAtFlag: null,
		}
	},
	getters: {
		currentConversation: () => useConversation().currentConversation,
		replyQuoteMsg(state) {
			if (!state.quoteMsg) return "";
			return `${state.quoteMsg.senderNickname}: ${parseMessageByType(state.quoteMsg)}`;
		},
	},
	actions: {
		initStoreData() {
			this.selfUserID = crimsdk.getLoginUserID();
			this.currentGrp = null;
			this.msgCondition = {};
			this.msgList = [];
			this.msgListIsEnd = false;
			this.quoteMsg = null;
			this.previewImageList = [];
			this.insertAtFlag = null;
		},
		initListener() {
			if (isInitListener) {
				return;
			}
			isInitListener = true;

			// 收到批量新消息
			crimsdk.on(IMEvents.OnRecvNewMsgs, ({
				data
			}) => {
				data.forEach(this.handleNewMessage);
			});

			// 私聊消息被对方标记为已读
			crimsdk.on(IMEvents.OnRecv1v1ReadReceipt, ({
				data
			}) => {
				const item = this.msgList.find(
					(msg) => msg.clientMsgID === data.clientMsgID,
				);
				item && (item.isRead = true);
			});

			// 群聊消息被对方标记为已读
			crimsdk.on(IMEvents.OnRecvGrpReadReceipt, ({
				data
			}) => {
				const item = this.msgList.find(
					(msg) => msg.clientMsgID === data.clientMsgID,
				);
				item && (item.isRead = true);
			});

			// 删消息
			crimsdk.on(IMEvents.OnMsgDeleted, ({
				data
			}) => {
				// 删store列表消息
				this.deleteMessage(data);
			});

			// 撤回
			crimsdk.on(IMEvents.OnNewRecvMsgRevoked, ({
				data
			}) => {
				// 更新成撤回的消息
				const item = this.msgList.find(
					(msg) => msg.clientMsgID === data.clientMsgID,
				);
				Object.assign(item, {
					contentType: MessageType.RevokeMessage,
					sessionType: SessionType.Notification,
					notificationElem: {
						detail: JSON.stringify(data)
					}
				});
			});

			crimsdk.on(IMEvents.SendMsgSuccess, ({
				data
			}) => {});

			crimsdk.on(IMEvents.SendMsgFailed, ({
				data
			}) => {});

			// 发送消息进度回调
			crimsdk.on(IMEvents.OnProgress, ({
				data
			}) => {});

			crimsdk.on(IMEvents.UploadFileProgress, ({
				data
			}) => {});

			crimsdk.on(IMEvents.OnRecvCustomBusinessMsg, ({
				data
			}) => {});
		},
		// 进入聊天界面，初始化数据
		async initCurrentConversation(data) {
			await useConversation().getCurrentConversation(data);
			this.msgCondition = {
				conversationID: this.currentConversation.conversationID,
				startClientMsgID: "",
				count: 20,
				lastMinSeq: 0,
			}
			this.quoteMsg = null;
			this.insertAtFlag = null;
			this.msgList = [];
			this.msgListIsEnd = false;
			this.previewImageList = [];
		},
		inCurrentConversation(newMsg) {
			const {
				userID,
				groupID
			} = this.currentConversation;
			switch (newMsg.sessionType) {
				case SessionType.Single:
					return (
						newMsg.sendID === userID ||
						(newMsg.sendID === this.selfUserID &&
							newMsg.recvID === userID)
					);
				case SessionType.Group:
				case SessionType.WorkingGroup:
					return newMsg.groupID === groupID;
				case SessionType.Notification:
					return newMsg.sendID === userID;
				default:
					return false;
			}
		},
		handleNewMessage(newMsg) {
			if (this.inCurrentConversation(newMsg)) {
				const isSingleMessage = newMsg.sessionType === SessionType.Single;

				// 	if (isSingleMessage) {
				// 		uni.$u.throttle(() => uni.$emit(GlobalEvents.OnlineStateCheck), 2000);
				// 	}

				if (newMsg.contentType === MessageType.TypingMessage) {
					if (isSingleMessage) {
						uni.$emit(GlobalEvents.TypingUpdate);
					}
				} else {
					newMsg.isAppend = true;
					// 添加新消息
					this.msgList.push(newMsg);
					// 如果有图片加到预览列表
					this.addPreviewImage(newMsg);
					setTimeout(() => uni.$emit(GlobalEvents.ScrollToBottom, true));

					// uni.$u.debounce(this.markConversationAsRead, 2000);
				}
			}
		},
		async getAdvancedHistoryMsgList() {
			try {
				const {
					msgCondition
				} = this;
				const {
					data: {
						messageList,
						lastMinSeq,
						isEnd,
					}
				} = await crimsdk.getAdvancedHistoryMsgList(msgCondition);
				// const {
				// 	messageList,
				// 	lastMinSeq,
				// 	isEnd,
				// } = msgData;
				const list = messageList ?? [];
				this.msgList.unshift(...list);
				this.previewImageList.unshift(...filterPreviewImage(list));
				const len = list.length;
				this.msgListIsEnd = isEnd || len < msgCondition.count;
				this.msgCondition.startClientMsgID = list[0]?.clientMsgID ?? "";
				this.msgCondition.lastMinSeq = lastMinSeq ?? 0;

				return len > 0 ? list[len - 1] : null;
			} catch (e) {
				this.msgListIsEnd = true;
				console.log(e);
			}
		},
		// 添加消息里的图片到预览列表
		addPreviewImage(msg) {
			if (msg.status === MessageStatus.Succeed && (msg.contentType === MessageType.PictureMessage || msg
					.contentType === MessageType.OANotification)) {
				this.previewImageList.push(...filterPreviewImage([msg]));
			}
		},
		// 添加一条未发送成功的msg
		async sendNewMessage(message, opt = {}) {
			this.msgList.push(message);
			const msgCondition = Object.assign({
				userID: this.currentConversation.userID,
				groupID: this.currentConversation.groupID,
			}, opt);
			await this.sendMessage(message, msgCondition);
		},
		// 更新store中msg的状态为发送中
		async reSendMessage(message) {
			const cmsg = this.msgList.find(
				(msg) => msg.clientMsgID === message.clientMsgID,
			);
			cmsg && (cmsg.status = MessageStatus.Sending);
			await this.sendMessage(message, this.currentConversation);
		},
		// 发送msg 再次更新状态
		async sendMessage(message, {
			userID,
			groupID,
		}) {
			try {
				const condition = Object.assign({
					recvID: userID,
					groupID,
					offlinePushInfo,
				}, {
					message,
				});
				await crimsdk.sendMsg(condition);
				// 更新message状态为Succ
				const cmsg = this.msgList.find(
					(msg) => msg.clientMsgID === message.clientMsgID,
				);
				cmsg.status = MessageStatus.Succeed;
				cmsg.errCode = 0;
				// 如果有图片加到预览列表
				this.addPreviewImage(message);
			} catch (e) {
				// 更新message状态为Fail
				const cmsg = this.msgList.find(
					(msg) => msg.clientMsgID === message.clientMsgID,
				);
				cmsg.status = MessageStatus.Failed;
				cmsg.errCode = e.errCode;
			}
		},
		updateMessage(message) {
			const idx = this.msgList.findIndex(
				(msg) => msg.clientMsgID === message.clientMsgID,
			);
			idx > -1 && (this.msgList[idx] = message);
		},
		deleteMessage(message) {
			const idx = this.msgList.findIndex(
				(msg) => msg.clientMsgID === message.clientMsgID,
			);
			idx > -1 && this.msgList.splice(idx, 1);
		},
		// 删消息保留会话
		async clearConversationAndDeleteAllMsg(conversationID) {
			await crimsdk.clearConversationAndDeleteAllMsg(conversationID);
			this.msgCondition.startClientMsgID = "";
			this.msgCondition.lastMinSeq = 0;
			this.msgList = [];
			// this.msgListIsEnd = false;
			this.quoteMsg = null;
			this.previewImageList = [];
		}
	}
})

export default useMsgStore;