import { defineStore } from "pinia";
import { FriendStore } from "./type";
import { IMSDK, IMTYPE } from "~/utils/imsdk";
import $router from "~/router";

export const useFriendStore = defineStore("friend", {
  state: (): FriendStore => ({
    newFriendCount: 0,
    list: undefined, //好友列表
    listPromise: undefined,
    blackList: undefined, //黑名单列表
    blackListPromise: undefined,
  }),
  actions: {
    async init() {
      const { data: reqList } = await IMSDK.getFriendReqListAsRecipient();
      this.newFriendCount = reqList.reduce(
        (accumulator, currentValue) =>
          accumulator + (currentValue.handleResult === 0 ? 1 : 0),
        0
      );
    },
    async getList() {
      if (this.listPromise) {
        await this.listPromise;
        return this.list as IMTYPE.FriendUserItem[];
      }
      if (this.list) return this.list;

      this.listPromise = new Promise(async (resolve, reject) => {
        const { data } = await IMSDK.getFriendList();
        this.list = data.map((item) => item.friendInfo!);
        resolve(this.list);
      });

      return this.listPromise;
    },
    onFriendDeleted({ data }: { data: IMTYPE.FriendUserItem }) {
      if (!this.list) return;
      const idx = this.list.findIndex((item) => item.userID === data.userID);
      if (idx > -1) {
        this.list.splice(idx, 1);
      }
    },
    onFriendAdded({ data }: { data: IMTYPE.FriendUserItem }) {
      if (!this.list) return;
      this.list.push(data);
    },
    onFriendReqAdded({ data }: { data: IMTYPE.FriendApplicationItem }) {
      if ($router.currentRoute.value.name !== "newFriend") {
        this.newFriendCount++;
      }
    },
    async getFriendInfo(
      userID: string
    ): Promise<IMTYPE.FriendUserItem | undefined> {
      const list = await this.getList();
      return list.find((item) => item.userID === userID);
    },
    onFriendInfoChanged({ data }: { data: IMTYPE.FriendUserItem }) {
      if (!this.list) return;
      const idx = this.list.findIndex((item) => item.userID === data.userID);
      if (idx > -1) {
        this.list[idx] = data;
      }
    },
    async getBlackList() {
      if (this.blackListPromise) {
        return this.blackListPromise;
      }
      if (this.blackList) return this.blackList;

      this.blackListPromise = new Promise(async (resolve, reject) => {
        const { data } = await IMSDK.getBlackList();
        this.blackList = data;
        resolve(this.blackList);
      });

      return this.blackListPromise;
    },
    async getBlackInfo(
      userID: string
    ): Promise<IMTYPE.BlackUserItem | undefined> {
      const list = await this.getBlackList();
      return list.find((item) => item.userID === userID);
    },
    onBlackAdded({ data }: { data: IMTYPE.BlackUserItem }) {
      if (!this.blackList) return;
      this.blackList.push(data);
    },
    onBlackDeleted({ data }: { data: IMTYPE.BlackUserItem }) {
      if (!this.blackList) return;
      const idx = this.blackList.findIndex(
        (item) => item.userID === data.userID
      );
      if (idx > -1) {
        this.blackList.splice(idx, 1);
      }
    },
  },
});
