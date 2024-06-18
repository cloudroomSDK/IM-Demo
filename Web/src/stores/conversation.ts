import { defineStore } from "pinia";
import { ConversationStore, MsgItem } from "./type";
import { IMSDK, IMTYPE } from "~/utils/imsdk";
import router from "~/router";
import { useUserStore } from ".";

export const useConversationStore = defineStore("conversation", {
  state: (): ConversationStore => ({
    currentConversationIndex: undefined,
    currentGroupInfo: undefined,
    currentMemberInGroup: undefined,
    conversationList: undefined,
    conversationListPromise: undefined,
    currentQuoteMessage: undefined,
    msgList: [], //当前联系人消息列表
    lastMinSeq: 0,
    multipleStatus: false, //多选状态
    multipleSelect: [],
    unreadMsgCount: 0, //未读消息数量
  }),
  actions: {
    async init() {
      const { data } = await IMSDK.getTotalUnreadMsgCount();
      this.unreadMsgCount = data;
    },
    async getList() {
      if (this.conversationListPromise) {
        await this.conversationListPromise;
        return this.conversationList as IMTYPE.ConversationItem[];
      }
      if (this.conversationList) return this.conversationList;

      this.conversationListPromise = new Promise(async (resolve, reject) => {
        const { data } = await IMSDK.getAllConversationList();
        this.conversationList = data;
        resolve(this.conversationList);
      });

      return this.conversationListPromise;
    },
    // 跳转联系人聊天页面
    async gotoConversationChat({
      conversationItem,
      conversationID,
      userID,
    }: {
      conversationItem?: IMTYPE.ConversationItem;
      conversationID?: string;
      userID?: string;
    }) {
      if (userID) {
        const t = this.conversationList?.find((item) => item.userID === userID);
        if (t) {
          conversationItem = t;
        } else {
          const { data } = await IMSDK.getOneConversation({
            sourceID: userID,
            sessionType: 1,
          });
          if (data.latestMsgSendTime === 0) {
            data.latestMsgSendTime = new Date().getTime();
          }
          conversationItem = data;
          this.conversationList?.push(conversationItem);
          this.conversationList = this.conversationListSort();
        }
      }
      if (conversationItem) {
        if (
          conversationItem?.conversationID ===
          this.currentConversation?.conversationID
        ) {
          return;
        }

        this.changeConversation(conversationItem);
        router.push({
          name: "conversation",
          params: {
            conversationID: conversationItem.conversationID,
          },
        });
      }
    },
    onNewConversation({ data }: { data: IMTYPE.ConversationItem[] }) {
      if (this.conversationList)
        this.conversationList = data.concat(this.conversationList);
    },
    onConversationChanged({ data }: { data: IMTYPE.ConversationItem[] }) {
      data.forEach((conversationItem) => {
        if (!this.conversationList) return;
        const idx = this.conversationList.findIndex(
          (item) => item.conversationID === conversationItem.conversationID
        );

        if (idx > -1) {
          this.conversationList[idx] = conversationItem;
        }
      });

      const curID = this.currentConversation?.conversationID;
      const newList = this.conversationListSort();
      if (curID) {
        const idx = newList?.findIndex(
          (item) => item.conversationID === curID
        )!;
        if (idx > -1) {
          this.currentConversationIndex = idx;
        }
      }
      this.conversationList = newList;
    },
    onTotalUnreadMsgCountChanged({ data }: { data: number }) {
      this.unreadMsgCount = data;
    },
    async changeConversation(conversation: IMTYPE.ConversationItem | null) {
      this.resetConversation();
      if (!conversation) {
        return;
      }
      const conversationList = await this.getList();

      const idx = conversationList.findIndex(
        (item) => item.conversationID === conversation!.conversationID
      );
      this.currentConversationIndex = idx;
      if (idx === -1) {
        if (conversation.latestMsgSendTime === 0) {
          conversation.latestMsgSendTime = new Date().getTime();
        }
        this.conversationList?.push(conversation);
        this.conversationList = this.conversationListSort();
      }

      // @ts-ignore
      if (this.currentConversation?.unreadCount > 0) {
        // 清空当前绘画未读数
        IMSDK.markConversationMsgAsRead(
          this.currentConversation!.conversationID
        );
      }

      if (this.isCurrentGroupChat) {
        {
          const { data } = await IMSDK.getSpecifiedGrpsInfo([
            this.currentConversation!.groupID,
          ]);
          this.currentGroupInfo = data[0];
        }
        {
          const userStore = useUserStore();
          const { data } = await IMSDK.getSpecifiedGrpMembersInfo({
            groupID: this.currentConversation!.groupID,
            userIDList: [userStore.userInfo!.userID],
          });
          this.currentMemberInGroup = data[0];
        }
      }
    },
    resetConversation() {
      this.currentConversationIndex = undefined;
      this.currentGroupInfo = undefined;
      this.currentMemberInGroup = undefined;
      this.currentQuoteMessage = undefined;
      this.msgList = [];
      this.lastMinSeq = 0;
      this.multipleStatus = false;
      this.multipleSelect = [];
    },
    async refershMsgList(count: number): Promise<number> {
      const { data } = await IMSDK.getAdvancedHistoryMsgList({
        lastMinSeq: this.lastMinSeq,
        count: count,
        startClientMsgID: this.msgList[0]?.clientMsgID ?? "",
        conversationID: this.currentConversation!.conversationID,
      });
      this.lastMinSeq = data.lastMinSeq;
      const msgList = data.messageList.map((item, idx) => ({
        ...item,
        isShowTime:
          idx === 0 ||
          item.sendTime - data.messageList[idx - 1]?.sendTime > 120000,
      })) as MsgItem[];
      this.msgList = msgList.concat(this.msgList);
      return data.messageList.length;
    },
    pushMsg(msg: IMTYPE.MessageItem[]) {
      const newMsg = msg.map((item, idx) => {
        let isShowTime = false;
        if (idx === 0) {
          if (this.msgList.length) {
            isShowTime =
              item.sendTime - this.msgList[this.msgList.length - 1].sendTime >
              120000;
          } else {
            isShowTime = true;
          }
        } else {
          isShowTime = item.sendTime - msg[idx - 1]?.sendTime > 120000;
        }
        return {
          ...item,
          isShowTime,
        };
      });
      this.msgList = this.msgList.concat(newMsg);
    },
    updateMsgList(msgList: IMTYPE.MessageItem[]) {
      msgList.forEach((updateItem) => {
        const oldMsgIndex = this.msgList.findIndex(
          (item) => item.clientMsgID === updateItem.clientMsgID
        );
        if (oldMsgIndex > -1) {
          this.msgList[oldMsgIndex] = {
            ...this.msgList[oldMsgIndex],
            ...updateItem,
            isShowTime: this.msgList[oldMsgIndex].isShowTime,
          };
        }
      });
    },
    removeList(MsgIDs: string[]) {
      MsgIDs.forEach((clientMsgID) => {
        const idx = this.msgList.findIndex(
          (item) => item.clientMsgID === clientMsgID
        );
        if (idx > -1) {
          this.msgList.splice(idx, 1);
        }
      });
    },
    conversationListSort(conversationList?: IMTYPE.ConversationItem[]) {
      const list = conversationList || this.conversationList;
      if (!list) return undefined;
      const arr: string[] = [];
      const filterArr = list.filter(
        (c) => !arr.includes(c.conversationID) && arr.push(c.conversationID)
      );
      filterArr.sort((a, b) => {
        if (a.isPinned === b.isPinned) {
          const aCompare =
            a.draftTextTime > a.latestMsgSendTime
              ? a.draftTextTime
              : a.latestMsgSendTime;
          const bCompare =
            b.draftTextTime > b.latestMsgSendTime
              ? b.draftTextTime
              : b.latestMsgSendTime;
          if (aCompare > bCompare) {
            return -1;
          } else if (aCompare < bCompare) {
            return 1;
          } else {
            return 0;
          }
        } else if (a.isPinned && !b.isPinned) {
          return -1;
        } else {
          return 1;
        }
      });
      return filterArr;
    },
    onGrpInfoChanged({ data }: { data: IMTYPE.GroupItem }) {
      if (data.groupID === this.currentGroupInfo?.groupID) {
        this.currentGroupInfo = data;
      }
    },
    onGrpDismissed({ data }: { data: IMTYPE.GroupItem }) {
      if (this.currentGroupInfo?.groupID === data.groupID) {
        this.currentGroupInfo = {
          ...data,
          status: 2,
        };
      }
    },
  },
  getters: {
    currentConversation(): IMTYPE.ConversationItem | undefined {
      if (this.currentConversationIndex === undefined) return undefined;
      return (
        this.conversationList &&
        this.conversationList[this.currentConversationIndex]
      );
    },
    isCurrentGroupChat(): boolean {
      if (this.currentConversation?.groupID) return true;
      return false;
    },
    isCurrentGroupAdmin(): boolean {
      return (
        this.currentMemberInGroup?.roleLevel === 100 ||
        this.currentMemberInGroup?.roleLevel === 60
      );
    },
    getMsgListLength(): number {
      if (!this.msgList) return 0;
      return this.msgList.length;
    },
  },
});
