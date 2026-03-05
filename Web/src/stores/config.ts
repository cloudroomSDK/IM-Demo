import { defineStore } from "pinia";
import { ConfigStore } from "./type";

export const useConfigStore = defineStore("config", {
  state: (): ConfigStore => ({
    businessServer: "默认服务器",
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
