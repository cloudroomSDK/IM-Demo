// @ts-ignore
import RTCSDK from "rtc-sdk-web";
import type { Directive } from "vue";

interface SDKParams {
  userID: string;
  sdkServer: string;
  appId?: string;
  appSecret?: string;
  token?: string;
}
interface Callback {
  notifyUserCamOpened?: (userID: string, isOpen: boolean) => void;
  notifyUserMicOpened?: (userID: string, isOpen: boolean) => void;
}

class SDKCtrl {
  private params?: SDKParams;
  private isLoginPromise: Promise<void> | null = null;
  private callback?: Callback;

  constructor() {}
  setParams(params: SDKParams) {
    this.isLoginPromise = null;
    this.params = params;
  }
  setCallback(callback: Callback) {
    this.callback = callback;
  }
  canIUse = (() => {
    // 因为SDK需要调用摄像头麦克风，所以在http协议下是无法使用的，增加这个判断提示用户
    return (
      location.host.startsWith("localhost") ||
      location.host.startsWith("127.0.0.1") ||
      location.protocol !== "http:"
    );
  })();
  private init = (() => {
    let promise: Promise<number> | null = null;
    return () => {
      if (promise) return promise;
      promise = RTCSDK.Init({ isCallSer: false }) as Promise<number>;
      promise.catch(() => {
        promise = null;
      });
      RTCSDK.AudioStatusChanged.callback = (
        userID: string,
        oldStatus: number,
        newStatus: number,
        opID: string,
      ) => {
        console.log("AudioStatusChanged:", userID, oldStatus, newStatus, opID);
        if (opID === this.params?.userID && userID === this.params?.userID) {
          return;
        }

        this.callback?.notifyUserMicOpened?.(userID, newStatus === 3);
      };
      RTCSDK.VideoStatusChanged.callback = (
        userID: string,
        oldStatus: number,
        newStatus: number,
        opID: string,
      ) => {
        console.log(
          "VideoStatusChanged:",
          userID,
          oldStatus,
          newStatus,
          opID,
          this.params?.userID,
        );
        if (opID === this.params?.userID && userID === this.params?.userID) {
          return;
        }
        this.callback?.notifyUserCamOpened?.(userID, newStatus === 3);
      };
      return promise;
    };
  })();

  private async login() {
    await this.init();
    if (this.isLoginPromise) return this.isLoginPromise;
    this.isLoginPromise = new Promise((resolve, reject) => {
      RTCSDK.LoginSuccess.callback = () => {
        console.log("LoginSuccess");
        resolve();
      };
      RTCSDK.LoginFail.callback = () => {
        console.log("LoginFail");
        reject();
      };

      const { token, userID, sdkServer, appId, appSecret } = this.params!;
      const url = new URL(sdkServer);
      RTCSDK.SetSDKParams({
        isHttp: url.protocol === "http:",
      });
      RTCSDK.SetServerAddr(url.host);
      if (token) {
        RTCSDK.LoginByToken(token, userID, userID);
      } else {
        RTCSDK.Login(appId, appSecret, userID, userID);
      }
    });
    return this.isLoginPromise;
  }

  private async createRoom() {
    return new Promise<string>((resolve, reject) => {
      RTCSDK.CreateMeetingSuccess.callback = (roomInfo: { ID: number }) => {
        console.log("CreateMeetingSuccess ID:", roomInfo.ID);
        resolve("" + roomInfo.ID);
      };
      RTCSDK.CreateMeetingFail.callback = () => {
        console.log("CreateMeetingFail");
        reject(new Error("Failed to create meeting"));
      };
      RTCSDK.CreateMeeting2();
    });
  }

  async getRoomID() {
    if (!this.params) throw "未初始化SDK";
    await this.login();
    return await this.createRoom();
  }
  async joinRoomID(roomID: string) {
    if (!this.params) throw "未初始化SDK";
    await this.login();

    return new Promise<void>((resolve, reject) => {
      RTCSDK.EnterMeetingRslt.callback = (sdkErr: number) => {
        console.log("EnterMeetingRslt:", sdkErr);
        if (sdkErr === 0) {
          resolve();
        } else {
          reject(new Error("Failed to enter meeting"));
        }
      };
      RTCSDK.EnterMeeting3(+roomID);
    });
  }
  leaveRoom() {
    RTCSDK.ExitMeeting();
    RTCSDK.Logout();
    this.isLoginPromise = null;
  }
  isCamOpened(userID: string): boolean {
    const memberInfo = RTCSDK.GetMemberInfo(userID);
    console.log("memberInfo:", memberInfo);
    return memberInfo?.videoStatus === 3;
  }
  isMicOpened(userID: string): boolean {
    const memberInfo = RTCSDK.GetMemberInfo(userID);
    console.log("memberInfo:", memberInfo);
    return memberInfo?.audioStatus === 3;
  }

  SwitchMic(userID: string, isOpen: boolean) {
    isOpen ? RTCSDK.OpenMic(userID) : RTCSDK.CloseMic(userID);
  }
  SwitchCam(userID: string, isOpen: boolean) {
    isOpen ? RTCSDK.OpenVideo(userID) : RTCSDK.CloseVideo(userID);
  }
}

export const RTCCtrl = new SDKCtrl();

interface vSDKVideoParam {
  UID: string;
  camId?: string;
}

export type SDKVideoDirective = Directive<HTMLElement, vSDKVideoParam>;

declare module "vue" {
  export interface ComponentCustomProperties {
    vSDKVideo: SDKVideoDirective;
  }
}

//导出指令集
export const vSDKVideo = {
  mounted(el, { value, modifiers }) {
    // 成员视频逻辑
    if (modifiers.user) {
      try {
        const { UID, camId } = value;
        const userVideoObj = RTCSDK.CreatVideoObj();
        userVideoObj.setVideo(UID, camId);
        el.appendChild(userVideoObj.handler());
      } catch (error) {
        console.error("成员视频指令初始化失败：", error);
      }
    }
  },
  // 新增：组件卸载时清理资源（关键！避免内存泄漏）
  unmounted(el) {
    // 1. 清空指令绑定元素的子节点，移除SDK创建的DOM
    el.innerHTML = "";

    // 2. 可选：如果RTCSDK提供了销毁方法，这里调用（示例）
    // if (RTCSDK.DestroyVideoObj) {
    //   RTCSDK.DestroyVideoObj(el)
    // }
  },
} satisfies SDKVideoDirective;
