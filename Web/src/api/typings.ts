export namespace API {
  export namespace Login {
    export enum UsedFor {
      Register = 1,
      Modify = 2,
      Login = 3,
    }
    export type LoginParams = {
      appID: string;
      verifyCode: string;
      deviceID?: string;
      phoneNumber: string;
      areaCode: string;
      password: string;
    };
    export type LoginData = {
      chatToken: string;
      imToken: string;
      userID: string;
      nickName: string;
    };
    export type SendSmsParams = {
      phoneNumber: string;
      areaCode: string;
      deviceID?: string;
      usedFor: UsedFor;
      appID: string;
      invitationCode?: string;
    };
  }
}
