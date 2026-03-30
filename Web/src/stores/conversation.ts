import { defineStore } from "pinia";
import { ConversationStore, MsgItem } from "./type";
import {
  ConversationItem,
  IMSDK,
  MessageItem,
  MessageType,
  SessionType,
  ViewType,
} from "~/utils/imsdk";
import router from "~/router";
import { useGroupStore } from ".";
import { getSvrTime } from "~/api/login";
import { CustomMsg, CustomMsgType } from "~/utils/callModule";

export const useConversationStore = defineStore("conversation", {
  state: (): ConversationStore => ({
    currentConversation: undefined,
    conversationList: undefined,
    conversationListPromise: undefined,
    currentQuoteMessage: undefined,
    msgList: [], //当前联系人消息列表
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
        return this.conversationList as ConversationItem[];
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
      conversationItem?: ConversationItem;
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
            sessionType: SessionType.Single,
          });
          conversationItem = data;
        }
      }
      if (conversationID) {
        const { data } = await IMSDK.getMultipleConversation([conversationID]);
        if (data.length) {
          conversationItem = data[0];
        } else {
          throw "conversation not found";
        }
      }
      if (conversationItem) {
        if (
          router.currentRoute.value.name === "conversation" &&
          conversationItem?.conversationID ===
            this.currentConversation?.conversationID
        ) {
          return;
        }

        const flag = this.conversationList?.find(
          (item) => item.conversationID === conversationItem.conversationID,
        );
        if (!flag) {
          if (conversationItem.latestMsgSendTime === 0) {
            conversationItem.latestMsgSendTime = await getSvrTime();
          }

          this.conversationList?.push(conversationItem);
          this.conversationList = this.conversationListSort();
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
    onNewConversation({ data }: { data: ConversationItem[] }) {
      if (!this.conversationList) return;
      let flag = false;
      data.forEach((item) => {
        const exists = this.conversationList?.some((c, idx) => {
          if (c.conversationID === item.conversationID) {
            this.conversationList![idx] = item;
            return true;
          }
        });
        if (!exists && this.conversationList) {
          flag = true;
          this.conversationList.push(item);
        }
      });
      if (flag) {
        this.conversationList = this.conversationListSort();
      }
    },
    onConversationChanged({ data }: { data: ConversationItem[] }) {
      data.forEach((conversationItem) => {
        if (
          this.currentConversation?.conversationID ===
          conversationItem.conversationID
        ) {
          this.currentConversation = conversationItem;
        }
        if (this.conversationList) {
          const idx = this.conversationList.findIndex(
            (item) => item.conversationID === conversationItem.conversationID,
          );

          if (idx > -1) {
            this.conversationList[idx] = conversationItem;
          }
        }
      });

      this.conversationList = this.conversationListSort();
    },
    onTotalUnreadMsgCountChanged({ data }: { data: number }) {
      this.unreadMsgCount = data;
    },
    async changeConversation(conversationItem?: ConversationItem | null) {
      this.resetConversation();
      if (!conversationItem) {
        return;
      }

      this.currentConversation = conversationItem;

      // if (conversationID) {
      //   const { data } = await IMSDK.getMultipleConversation([conversationID]);
      //   if (data.length) {
      //     this.currentConversation = data[0];
      //   } else {
      //     throw "conversation not found";
      //   }
      // }

      if (this.isCurrentGroupChat) {
        const groupStore = useGroupStore();
        await groupStore.updateCurrentGroup(this.currentConversation!.groupID);
      }
    },
    resetConversation() {
      this.currentConversation = undefined;
      this.currentQuoteMessage = undefined;
      this.msgList = [];
      this.multipleStatus = false;
      this.multipleSelect = [];

      const groupStore = useGroupStore();
      groupStore.updateCurrentGroup();
    },
    async refershMsgList(count: number): Promise<number> {
      const { data } = await IMSDK.getAdvancedHistoryMsgList({
        viewType: ViewType.History,
        count: count,
        startClientMsgID: this.msgList[0]?.clientMsgID ?? "",
        conversationID: this.currentConversation!.conversationID,
      });
      let msgList = data.messageList;

      msgList = msgList
        .filter((item) => {
          if (item.contentType === MessageType.CustomMessage) {
            try {
              //@ts-ignore
              const data = JSON.parse(item.customElem.data) as CustomMsg;
              if (
                [
                  CustomMsgType.NewInvitation,
                  CustomMsgType.InviteeAccept,
                ].includes(data.customType)
              ) {
                IMSDK.deleteMsg({
                  conversationID: this.currentConversation!.conversationID,
                  clientMsgID: item.clientMsgID,
                });
                return false;
              }

              //@ts-ignore
              item.customElem.data = data;
            } catch (error) {
              return false;
            }
          }
          return true;
        })
        .map((item, idx) => ({
          ...item,
          isShowTime:
            idx === 0 ||
            item.sendTime - data.messageList[idx - 1]?.sendTime > 120000,
        })) as MsgItem[];
      this.msgList = msgList.concat(this.msgList);

      return data.messageList.length;
    },
    pushMsg(msg: MessageItem[]) {
      const newMsg = msg
        .filter((item) => {
          if (item.contentType === MessageType.CustomMessage) {
            try {
              //@ts-ignore
              const data = JSON.parse(item.customElem.data) as CustomMsg;
              if (
                [
                  CustomMsgType.NewInvitation,
                  CustomMsgType.InviteeAccept,
                ].includes(data.customType)
              ) {
                IMSDK.deleteMsg({
                  conversationID: this.currentConversation!.conversationID,
                  clientMsgID: item.clientMsgID,
                });
                return false;
              }

              //@ts-ignore
              item.customElem.data = data;
            } catch (error) {
              return false;
            }
          }
          return true;
        })
        .map((item, idx) => {
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
    updateMsgList(msgList: MessageItem[]) {
      msgList.forEach((updateItem) => {
        const oldMsgIndex = this.msgList.findIndex(
          (item) => item.clientMsgID === updateItem.clientMsgID,
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
          (item) => item.clientMsgID === clientMsgID,
        );
        if (idx > -1) {
          this.msgList.splice(idx, 1);
        }
      });
    },
    conversationListSort(conversationList?: ConversationItem[]) {
      const list = conversationList || this.conversationList;
      if (!list) return undefined;
      const arr: string[] = [];
      const filterArr = list.filter(
        (c) => !arr.includes(c.conversationID) && arr.push(c.conversationID),
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
  },
  getters: {
    isCurrentGroupChat(): boolean {
      if (this.currentConversation?.groupID) return true;
      return false;
    },
    getMsgListLength(): number {
      if (!this.msgList) return 0;
      return this.msgList.length;
    },
  },
});
