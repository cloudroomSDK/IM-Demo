import { defineStore } from "pinia";
import { UserStore, SDKLoginInfo } from "./type";
import { IMSDK, IMTYPE } from "~/utils/imsdk";
import router from "~/router";
import { API } from "~/api/typings";
import loginApi from "~/api/login";

export const useUserStore = defineStore("user", {
  state: (): UserStore => ({
    businessData: undefined,
    userInfo: undefined,
    lastLoginInfo: undefined,
  }),
  getters: {
    getMyUserID: (state) => state.userInfo?.userID,
    getChatToken: (state) => state.businessData?.chatToken,
  },
  actions: {
    // 业务登录
    async businessLogin(businessInfo: API.Login.LoginParams) {
      this.businessData = await loginApi.login(businessInfo);
    },
    // 登录SDK
    async sdkLogin(loginInfo: SDKLoginInfo) {
      await IMSDK.login({
        userID: this.businessData!.userID,
        appId: loginInfo.appId,
        appSecret: loginInfo.appSecret,
        sdkServer: loginInfo.sdkServer,
        token: loginInfo.token,
        tryParse: true,
      });

      const { data: selfUserInfo } = await IMSDK.getSelfUserInfo();
      IMSDK.on("OnSelfInfoUpdated", this.updateUserInfo);
      IMSDK.on("OnSelfInfoUpdated", () => {});
      this.userInfo = selfUserInfo;

      this.lastLoginInfo = loginInfo;
    },
    logout() {
      IMSDK.off("OnSelfInfoUpdated", this.updateUserInfo);
      IMSDK.logout();
      this.exit();
    },
    updateUserInfo({ data }: { data: IMTYPE.SelfUserInfo }) {
      this.userInfo = data;
    },
    exit() {
      this.userInfo = undefined;
      this.businessData = undefined;
      this.lastLoginInfo = undefined;
      router.push({ name: "login" });
    },
  },
  persist: {
    key: "LAST_LOGIN_INFO",
    paths: ["lastLoginInfo", "businessData"],
  },
});
