export namespace API {
  export namespace Login {
    export enum UsedFor {
      Register = 1,
      Modify = 2,
      Login = 3,
    }
    export type LoginParams = {
      verifyCode?: string;
      deviceID?: string;
      phoneNumber?: string;
      areaCode?: string;
      password?: string;
      account?: string;
    };

    export type LoginData = {
      chatToken: string;
      imToken: string;
      userID: string;
      nickName: string;
      sdkAppId: string;
      sdkSvr: string;
      sdkToken: string;
      sdkAuthType: "0" | "1"; // 0: 密钥模式 1: Token模式
      sdkSecret: string;
    };
    export type SendSmsParams = {
      phoneNumber: string;
      areaCode: string;
      deviceID?: string;
      usedFor: UsedFor;
      invitationCode?: string;
    };
  }
}
