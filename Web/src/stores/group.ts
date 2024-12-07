import { defineStore } from "pinia";
import { GroupStore } from "./type";
import { IMSDK, IMTYPE } from "~/utils/imsdk";
import { useUserStore } from ".";
import $router from "~/router";

export const useGroupStore = defineStore("group", {
  state: (): GroupStore => ({
    messageCount: 0, //入群申请通知数量
    list: undefined,
    listPromise: undefined,
    currentGroupInfo: undefined,
    currentMemberInGroup: undefined,
    currentGroupMemberList: undefined,
  }),
  actions: {
    async getList() {
      if (this.listPromise) {
        await this.listPromise;
        return this.list as IMTYPE.GroupItem[];
      }

      if (this.list) return this.list;

      this.listPromise = new Promise(async (resolve, reject) => {
        const { data } = await IMSDK.getJoinedGrpList();
        this.list = data;
        resolve(this.list);
      });

      return this.listPromise;
    },
    onJoinedGrpAdded({ data }: { data: IMTYPE.GroupItem }) {
      if (!this.list) return;
      this.list.push(data);
    },
    onGrpInfoChanged({ data }: { data: IMTYPE.GroupItem }) {
      if (data.groupID === this.currentGroupInfo?.groupID) {
        this.currentGroupInfo = data;
      }

      if (!this.list) return;
      const idx = this.list.findIndex((item) => item.groupID === data.groupID);
      if (idx > -1) {
        this.list[idx] = data;
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
    onJoinedGrpDeleted({ data }: { data: IMTYPE.GroupItem }) {
      if (!this.list) return;
      const idx = this.list.findIndex((item) => item.groupID === data.groupID);
      if (idx > -1) {
        this.list.splice(idx, 1);
      }
    },
    onGrpReqAdded({ data }: { data: IMTYPE.GroupApplicationItem }) {
      if ($router.currentRoute.value.name !== "newGroups") {
        this.messageCount++;
      }
    },
    onGrpMemberAdded({ data }: { data: IMTYPE.GroupMemberItem }) {
      if (
        this.currentGroupMemberList &&
        this.currentGroupInfo?.groupID === data.groupID
      ) {
        this.currentGroupMemberList.push(data);
      }
    },
    onGrpMemberDeleted({ data }: { data: IMTYPE.GroupMemberItem }) {
      const userStore = useUserStore();
      if (data.userID === userStore.getMyUserID) {
        this.onJoinedGrpDeleted({
          data: {
            groupID: data.groupID,
          } as IMTYPE.GroupItem,
        });
      }

      if (
        this.currentGroupMemberList &&
        this.currentGroupInfo?.groupID === data.groupID
      ) {
        const idx = this.currentGroupMemberList.findIndex(
          (item) => item.userID === data.userID
        );
        if (idx > -1) {
          this.currentGroupMemberList.splice(idx, 1);
        }
      }
    },
    onGrpMemberInfoChanged({ data }: { data: IMTYPE.GroupMemberItem }) {
      if (data.groupID === this.currentGroupInfo?.groupID) {
        if (this.currentGroupMemberList) {
          const idx =
            this.currentGroupMemberList.findIndex(
              (item) => item.userID === data.userID
            ) ?? -1;
          if (idx > -1) {
            this.currentGroupMemberList[idx] = data;
          }
        }
        if (this.currentMemberInGroup?.userID === data.userID) {
          this.currentMemberInGroup = data;
        }
      }
    },
    async updateCurrentGroup(groupID?: string) {
      this.currentGroupMemberList = undefined;
      this.currentGroupInfo = undefined;
      this.currentMemberInGroup = undefined;
      if (!groupID) return;

      await this.updateGroupInfo(groupID);
      await this.updateMyInfoInGroup(groupID);
      await this.updateGroupMemberList(groupID);
    },
    // 获取群资料
    async updateGroupInfo(groupID: string) {
      const { data } = await IMSDK.getSpecifiedGrpsInfo([groupID]);
      this.currentGroupInfo = data[0];
    },
    // 更新群内我的成员信息
    async updateMyInfoInGroup(groupID: string) {
      const userStore = useUserStore();
      const { data } = await IMSDK.getSpecifiedGrpMembersInfo({
        groupID,
        userIDList: [userStore.userInfo!.userID],
      });
      this.currentMemberInGroup = data[0];
    },
    // 更新群成员列表
    async updateGroupMemberList(groupID: string) {
      // 获取群成员列表
      const { data } = await IMSDK.getGrpMemberList({
        groupID: groupID,
        filter: 0,
        offset: 0,
        count: 10000,
      });

      this.currentGroupMemberList = data;
    },
  },
  getters: {
    isCurrentGroupAdmin(): boolean {
      return (
        this.currentMemberInGroup?.roleLevel === 100 ||
        this.currentMemberInGroup?.roleLevel === 60
      );
    },
  },
});
