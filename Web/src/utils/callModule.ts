import { getSvrTime } from "~/api/login";
import {
  Platform,
  SessionType,
  CbEvents,
  MessageItem,
  IMSDK,
  MessageType,
} from "./imsdk";
export enum CustomMsgType {
  NewInvitation = 300, /// 新邀请
  InviteeAccept, /// 邀请接受
  InviteeReject, /// 邀请拒绝
  InvitationCancel, /// 邀请取消
  InvitationHangUp, /// 邀请挂断
}

export interface CustomMsg {
  customType: CustomMsgType;
  data: CustomMsgData<keyof CustomEventMap>;
}
export enum InvitationOpType {
  Cancel, //主叫取消
  Timeout, //被叫超时无应答
  Busy, //被叫忙
  Reject, //被叫拒接
  AcceptedByOther = 100, //其他端接受
  RejectedByOther, //其他端拒绝

  BUTT = 340,
}

export interface InvitationInfo {
  duration: number; //通话时长（秒） ，通话接通后挂断者，依据acceptTime计算此值；
  groupID: string; //被邀请的群id，单聊时为空
  initiateTime: number; //发起时间（秒）
  invitationMsgId: string; //邀请的唯一标识，拒绝/接受/取消/挂断时应填写对应的邀请标识
  inviteeUserIDList: string[]; //被邀请者UserID列表, 如果目标是群且为全部成员，列表可为空
  inviterUserID: string; ////邀请的初始发起者
  mediaType: "video" | "audio";
  opDesc: string; //操作附加描述
  opType: InvitationOpType;
  platformID: Platform;
  roomID: string; //会议号
  sessionType: SessionType;
  timeout: number; //邀请超时时间（秒）
  acceptTime: number; //接受邀请的时间（秒）
}

type CustomEventMap = {
  [CustomMsgType.NewInvitation]: InvitationInfo;
  [CustomMsgType.InviteeAccept]: InvitationInfo;
  [CustomMsgType.InviteeReject]: InvitationInfo;
  [CustomMsgType.InvitationCancel]: InvitationInfo;
  [CustomMsgType.InvitationHangUp]: InvitationInfo;
  [key: string]: any;
};
type CustomMsgData<E extends keyof CustomEventMap> = CustomEventMap[E];

interface Callback {
  //按有呼入处理
  notifyCallIn?: (invitationInfo: InvitationInfo) => void;
  //通知呼出被拒绝
  notifyCallRejected?: (invitationInfo: InvitationInfo) => void;
  //通知呼出被接听
  notifyCallAccepted?: (invitationInfo: InvitationInfo) => void;
  //通知呼出被取消
  notifyCallCancel?: (invitationInfo: InvitationInfo) => void;
  //通知呼叫被挂断
  notifyCallHangUp?: (invitationInfo: InvitationInfo) => void;
  //通知呼入在其它设备上接听
  notifyCallAcceptedByOtherDevice?: (invitationInfo: InvitationInfo) => void;
  //通知呼入在其它设备上拒绝
  notifyCallRejectedByOtherDevice?: (invitationInfo: InvitationInfo) => void;
  //通知插入新消息到当前聊天列表
  notifyInsertMessage?: (msg: MessageItem) => void;
}

class CallModule {
  callback: Callback;
  curInvitationInfo: InvitationInfo | null = null;
  timeoutTimer: NodeJS.Timeout | null = null;
  private myUserID: string = "";
  constructor() {
    this.callback = {};
    this.init();
  }
  init() {
    setTimeout(() => {
      IMSDK.on(CbEvents.OnRecvNewMsg, ({ data }) => {
        this.msgHandle([data]);
      });
      IMSDK.on(CbEvents.OnRecvNewMsgs, ({ data }) => {
        this.msgHandle(data);
      });
      IMSDK.on(CbEvents.OnConnectSuccess, async () => {
        const { data: userID } = await IMSDK.getLoginUserID();
        this.myUserID = userID;
      });
    });
  }

  //发起呼叫-利用自定义消息
  async startCall(userID: string, groupID: string) {
    if (this.curInvitationInfo) {
      const customMsg: CustomMsg = {
        customType: CustomMsgType.NewInvitation,
        data: this.curInvitationInfo,
      };

      const { data: message } = await IMSDK.createCustomMsg(
        {
          data: JSON.stringify(customMsg),
          extension: "",
          description: "",
        },
        this.curInvitationInfo.invitationMsgId,
      );
      const { data: successMessage } = await IMSDK.sendMsg({
        recvID: userID,
        groupID: groupID,
        message,
        // offlinePushInfo?: OfflinePush;
      });

      // 超时处理
      this.timeoutTimer = setTimeout(async () => {
        if (!this.curInvitationInfo) return;
        this.curInvitationInfo.opType = InvitationOpType.Timeout;
        this.curInvitationInfo.opDesc = "timeout";
        await this.handleTimeout(userID, groupID);
        this.curInvitationInfo = null;
      }, this.curInvitationInfo.timeout * 1e3);
    }
  }

  //发送超时信息
  async handleTimeout(userID: string, groupID: string) {
    this.timeoutTimer = null;
    this.curInvitationInfo!.opType = InvitationOpType.Timeout;

    const customMsg: CustomMsg = {
      customType: CustomMsgType.InvitationCancel,
      data: this.curInvitationInfo,
    };
    const { data: message } = await IMSDK.createCustomMsg(
      {
        data: JSON.stringify(customMsg),
        extension: "",
        description: "",
      },
      this.curInvitationInfo!.invitationMsgId,
    );

    this.callback.notifyCallRejected?.(this.curInvitationInfo!);

    const { data: successMessage } = await IMSDK.sendMsg({
      recvID: userID,
      groupID: groupID,
      message,
      // offlinePushInfo?: OfflinePush;
    });

    this.callback.notifyInsertMessage?.(successMessage);
  }
  setCurInvitationInfo(invitationInfo: InvitationInfo | null) {
    if (invitationInfo && this.curInvitationInfo) {
      throw new Error("当前已在会议中或通话中，不能再发起通话。");
    }
    this.curInvitationInfo = invitationInfo;
  }
  clearInvitationInfo(msgId: string) {
    if (this.curInvitationInfo?.invitationMsgId === msgId) {
      this.curInvitationInfo = null;
    }
  }
  setCallback(callback: Callback) {
    this.callback = callback;
  }
  msgHandle(msgarr: MessageItem[]) {
    msgarr
      .filter(
        (item) =>
          item.contentType === MessageType.CustomMessage &&
          item.customElem?.data,
      )
      .forEach(async (msgItem: MessageItem) => {
        const customMsg = JSON.parse(msgItem.customElem!.data) as CustomMsg;
        const invitationInfo = customMsg.data as InvitationInfo;
        console.log("new custom msg", customMsg);
        if (customMsg.customType === CustomMsgType.NewInvitation) {
          if (customMsg.data.inviterUserID === this.myUserID) {
            // 自己的账号在其他端发出的邀请消息，不处理
            return;
          }
          if (this.curInvitationInfo) {
            // 正在通话中
            this.callReject(invitationInfo, InvitationOpType.Busy, "busy");
            return;
          }
          this.setCurInvitationInfo(invitationInfo);
          this.callback.notifyCallIn?.(invitationInfo);
        } else if (
          this.curInvitationInfo?.invitationMsgId ===
          invitationInfo.invitationMsgId
        ) {
          if (customMsg.customType === CustomMsgType.InviteeAccept) {
            this.timeoutTimer && clearTimeout(this.timeoutTimer);
            if (msgItem.sendID === this.myUserID) {
              this.callback.notifyCallAcceptedByOtherDevice?.(invitationInfo);
              this.curInvitationInfo = null;
            } else {
              this.curInvitationInfo.acceptTime = Math.floor(
                (await getSvrTime()) / 1000,
              );
              this.callback.notifyCallAccepted?.(invitationInfo);
            }
          } else if (customMsg.customType === CustomMsgType.InviteeReject) {
            this.timeoutTimer && clearTimeout(this.timeoutTimer);
            if (msgItem.sendID === this.myUserID) {
              this.callback.notifyCallRejectedByOtherDevice?.(invitationInfo);
            } else {
              this.callback.notifyCallRejected?.(invitationInfo);
            }
            this.curInvitationInfo = null;
          } else if (customMsg.customType === CustomMsgType.InvitationCancel) {
            // this.timeoutTimer && clearTimeout(this.timeoutTimer);
            this.callback.notifyCallCancel?.(invitationInfo);
            this.curInvitationInfo = null;
          } else if (customMsg.customType === CustomMsgType.InvitationHangUp) {
            this.callback.notifyCallHangUp?.(invitationInfo);
            this.curInvitationInfo = null;
          }
        }
      });
  }
  // 接受呼叫
  async callAccept() {
    if (!this.curInvitationInfo) return;
    const customMsg: CustomMsg = {
      customType: CustomMsgType.InviteeAccept,
      data: this.curInvitationInfo,
    };

    const { data: message } = await IMSDK.createCustomMsg(
      {
        data: JSON.stringify(customMsg),
        extension: "",
        description: "",
      },
      // this.curInvitationInfo.invitationMsgId,
    );

    const { data: successMessage } = await IMSDK.sendMsg({
      recvID: this.curInvitationInfo.groupID
        ? ""
        : this.curInvitationInfo.inviterUserID,
      groupID: this.curInvitationInfo.groupID,
      message,
      // offlinePushInfo?: OfflinePush;
    });
  }
  // 拒绝呼叫
  async callReject(
    invitationInfo: InvitationInfo | null = this.curInvitationInfo,
    opType: InvitationOpType = InvitationOpType.Reject,
    opDesc = "reject",
  ) {
    if (!invitationInfo) {
      return;
    }
    invitationInfo.opDesc = opDesc;
    invitationInfo.opType = opType;

    const customMsg: CustomMsg = {
      customType: CustomMsgType.InviteeReject,
      data: invitationInfo,
    };

    const { data: message } = await IMSDK.createCustomMsg(
      {
        data: JSON.stringify(customMsg),
        extension: "",
        description: "",
      },
      // invitationInfo.invitationMsgId,
    );

    const { data: successMessage } = await IMSDK.sendMsg({
      recvID: invitationInfo.groupID ? "" : invitationInfo.inviterUserID,
      groupID: invitationInfo.groupID,
      message,
      // offlinePushInfo?: OfflinePush;
    });

    this.callback.notifyInsertMessage?.(successMessage);
  }
  // 挂断呼叫
  async callHangUp(duration: number) {
    if (!this.curInvitationInfo) {
      return;
    }

    this.curInvitationInfo.duration = duration;
    Math.floor((await getSvrTime()) / 1000) - this.curInvitationInfo.acceptTime;

    const customMsg: CustomMsg = {
      customType: CustomMsgType.InvitationHangUp,
      data: this.curInvitationInfo,
    };

    const { data: message } = await IMSDK.createCustomMsg(
      {
        data: JSON.stringify(customMsg),
        extension: "",
        description: "",
      },
      // this.curInvitationInfo.invitationMsgId,
    );
    let recvID = "";
    if (!this.curInvitationInfo.groupID) {
      recvID =
        this.curInvitationInfo.inviterUserID === this.myUserID
          ? this.curInvitationInfo.inviteeUserIDList[0]
          : this.curInvitationInfo.inviterUserID;
    }
    const { data: successMessage } = await IMSDK.sendMsg({
      recvID: recvID,
      groupID: this.curInvitationInfo.groupID,
      message,
      // offlinePushInfo?: OfflinePush;
    });

    this.callback.notifyInsertMessage?.(successMessage);
  }
  // 取消呼叫
  async callCancel() {
    if (!this.curInvitationInfo) return;
    this.curInvitationInfo.opDesc = "cancel";
    this.curInvitationInfo.opType = InvitationOpType.Cancel;

    const customMsg: CustomMsg = {
      customType: CustomMsgType.InvitationCancel,
      data: this.curInvitationInfo,
    };

    const { data: message } = await IMSDK.createCustomMsg(
      {
        data: JSON.stringify(customMsg),
        extension: "",
        description: "",
      },
      // this.curInvitationInfo.invitationMsgId,
    );

    const { data: successMessage } = await IMSDK.sendMsg({
      recvID: this.curInvitationInfo.groupID
        ? ""
        : this.curInvitationInfo.inviteeUserIDList[0],
      groupID: this.curInvitationInfo.groupID,
      message,
      // offlinePushInfo?: OfflinePush;
    });
    this.callback.notifyInsertMessage?.(successMessage);
  }
}

const callModule = new CallModule();

// 设置回调
export const setInvitationCallback = (callback: Callback) => {
  callModule.setCallback(callback);
};

export const setCurInvitationInfo = (invitationInfo: InvitationInfo | null) => {
  callModule.setCurInvitationInfo(invitationInfo);
};

export const getCurInvitationInfo = () => {
  return callModule.curInvitationInfo;
};

export const clearInvitationInfo = (msgId: string) => {
  callModule.clearInvitationInfo(msgId);
};

export const callAccept = async () => {
  await callModule.callAccept();
};

export const callReject = async () => {
  await callModule.callReject();
  callModule.setCurInvitationInfo(null);
};
export const callCancel = async () => {
  await callModule.callCancel();
  callModule.setCurInvitationInfo(null);
};
export const callHangUp = async (duration: number) => {
  await callModule.callHangUp(duration);
  callModule.setCurInvitationInfo(null);
};

export const startCall = (userID: string, groupID: string) => {
  return callModule.startCall(userID, groupID);
};
