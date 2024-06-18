import { defineStore } from "pinia";
import { GroupStore } from "./type";
import { IMSDK, IMTYPE } from "~/utils/imsdk";
import { useUserStore } from ".";

export const useGroupStore = defineStore("group", {
  state: (): GroupStore => ({
    list: undefined,
    listPromise: undefined,
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
      if (!this.list) return;
      const idx = this.list.findIndex((item) => item.groupID === data.groupID);
      if (idx > -1) {
        this.list[idx] = data;
      }
    },
    onJoinedGrpDeleted({ data }: { data: IMTYPE.GroupItem }) {
      if (!this.list) return;
      const idx = this.list.findIndex((item) => item.groupID === data.groupID);
      if (idx > -1) {
        this.list.splice(idx, 1);
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
    },
  },
});
