import { defineStore } from "pinia";
import { ConfigStore } from "./type";
import { businessServer } from "~/config"; //获取默认值

export const useConfigStore = defineStore("config", {
  state: (): ConfigStore => ({
    businessServer,
  }),
  actions: {
    change(config: ConfigStore) {
      this.businessServer = config.businessServer;
    },
  },
  persist: {
    key: "LOGIN_CFG",
  },
});
