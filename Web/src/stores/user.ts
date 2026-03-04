import { defineStore } from "pinia";
import { UserStore } from "./type";
import { CbEvents, IMSDK, SelfUserInfo } from "~/utils/imsdk";
import router from "~/router";
import { API } from "~/api/typings";
import loginApi from "~/api/login";
import { base64Parms, md5 } from "~/utils";

export const useUserStore = defineStore("user", {
  state: (): UserStore => ({
    businessData: undefined,
    userInfo: undefined,
    isSyncing: false,
    syncProgress: 0,
  }),
  getters: {
    getMyUserID: (state) => state.userInfo?.userID,
    getChatToken: (state) => state.businessData?.chatToken,
  },
  actions: {
    // 业务登录-使用账号密码或者手机号
    async businessLogin(businessInfo: API.Login.LoginParams) {
      this.businessData = await loginApi.login(businessInfo);
      return this.businessData;
    },
    // 业务登录-使用chatToken
    async businessLoginByToken() {
      this.businessData = await loginApi.loginByToken();
      return this.businessData;
    },
    // 登录SDK
    async sdkLogin({
      sdkAuthType,
      sdkSvr,
      sdkAppId,
      sdkSecret,
      sdkToken,
    }: API.Login.LoginData) {
      const loginInfo =
        sdkAuthType === "0"
          ? {
              sdkServer: sdkSvr,
              appId: sdkAppId,
              appSecret: md5(base64Parms(sdkSecret)),
            }
          : {
              sdkServer: sdkSvr,
              token: sdkToken,
            };

      this.isSyncing = true;
      this.syncProgress = 0;

      await IMSDK.login({
        userID: this.businessData!.userID,
        appId: loginInfo.appId,
        appSecret: loginInfo.appSecret,
        sdkServer: loginInfo.sdkServer,
        token: loginInfo.token,
        tryParse: true,
      });

      const { data: selfUserInfo } = await IMSDK.getSelfUserInfo();
      IMSDK.on(CbEvents.OnSelfInfoUpdated, this.updateUserInfo);
      this.userInfo = selfUserInfo;
    },
    logout() {
      IMSDK.off(CbEvents.OnSelfInfoUpdated, this.updateUserInfo);
      IMSDK.logout();
      this.exit();
    },
    updateUserInfo({ data }: { data: SelfUserInfo }) {
      this.userInfo = data;
    },
    exit() {
      this.userInfo = undefined;
      this.businessData = undefined;
      router.push({ name: "login" });
    },
  },
  persist: {
    key: "LAST_LOGIN_INFO",
    paths: ["businessData"],
  },
});
