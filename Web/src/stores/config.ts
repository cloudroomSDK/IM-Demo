import { defineStore } from "pinia";
import { ConfigStore } from "./type";
import {
  businessServer,
  sdkServer,
  appId,
  appSecret,
  useToken,
  token,
} from "~/config"; //获取默认值

export const useConfigStore = defineStore("config", {
  state: (): ConfigStore => ({
    businessServer,
    sdkServer,
    useToken,
    appId,
    appSecret,
    token,
  }),
  actions: {
    change(config: ConfigStore) {
      this.businessServer = config.businessServer;
      this.sdkServer = config.sdkServer;
      this.useToken = config.useToken;
      this.appId = config.appId;
      this.appSecret = config.appSecret;
      this.token = config.token;
    },
  },
  persist: {
    key: "LOGIN_CFG"
  },
});
