import { ConversationItem, Platform, SessionType, IMSDK } from "./imsdk";
import {
  callAccept,
  callCancel,
  callHangUp,
  callReject,
  clearInvitationInfo,
  InvitationInfo,
  InvitationOpType,
  getCurInvitationInfo,
  setCurInvitationInfo,
  startCall,
} from "./callModule";
import { useUserStore } from "~/stores";
import { v4 as uuidv4 } from "uuid";
import { ElMessageBox, ElMessageBoxOptions } from "element-plus";
import { Call } from "../components";
import { h, reactive } from "vue";
import { RTCCtrl } from "./rtcsdk";
import { getSvrTime } from "~/api/login";
import { AudioPlayer } from "./audioPlayer";
import OutgoingRing from "~/assets/audio/outgoing_ring.mp3";
import IncomingRing from "~/assets/audio/incoming_ring.mp3";
import { Notify } from "./systemNotify";

window.addEventListener("beforeunload", (e) => {
  const curInfo = getCurInvitationInfo();
  if (!curInfo) return;
  InvitationView.optionMap[curInfo.invitationMsgId]?.destory();
});

class AudioCtrl {
  private static msgID: string | null;
  private static player: AudioPlayer | null;
  static play(msgID: string, isMyCall: boolean) {
    AudioCtrl.msgID = msgID;
    AudioCtrl.player = new AudioPlayer(isMyCall ? OutgoingRing : IncomingRing, {
      priority: 3,
      loop: true,
    });

    AudioCtrl.player.play();
  }
  static stop(msgID: string) {
    if (AudioCtrl.msgID === msgID) {
      AudioCtrl.player?.destroy();
      AudioCtrl.player = null;
      AudioCtrl.msgID = null;
    }
  }
}

export class InvitationView implements InvitationInfo {
  duration: number = 0;
  groupID: string = "";
  initiateTime: number = 0;
  invitationMsgId: string;
  inviteeUserIDList: string[] = [];
  inviterUserID: string = "";
  mediaType: "video" | "audio";
  opDesc: string = "";
  opType: InvitationOpType = InvitationOpType.BUTT;
  platformID: Platform = Platform.Web;
  roomID: string = "0";
  sessionType: SessionType = SessionType.Single;
  timeout: number = 30;
  acceptTime: number = 0;

  conversation?: ConversationItem;
  isMyCall: boolean = false;
  showName: string = "";
  invitationStatus: "waiting" | "calling" | "meeting" | "ended" = "waiting";
  private closeElbox?: () => void;
  private calledTimerID?: NodeJS.Timeout | null = null; //被叫

  //   onClickHangup?: () => void;

  static optionMap: Record<string, InvitationView> = {};

  private constructor(id: string, type: "audio" | "video") {
    this.mediaType = type;
    this.invitationMsgId = id;
  }
  // 我是主叫时创建实例
  static async create(
    type: "audio" | "video",
    inviteeUserIDList: string[] | ConversationItem,
  ) {
    if (!RTCCtrl.canIUse) {
      throw new Error(
        "当前使用http协议，不能调用摄像头麦克风，请使用https协议访问当前页面",
      );
    }
    const isExist = !!getCurInvitationInfo();
    if (isExist) {
      throw new Error("当前已在会议中或通话中，不能再发起通话。");
    }
    let conversation: ConversationItem;
    if (Array.isArray(inviteeUserIDList)) {
      inviteeUserIDList = inviteeUserIDList;
      const { data } = await IMSDK.getOneConversation({
        sourceID: inviteeUserIDList[0],
        sessionType: SessionType.Single,
      });
      conversation = data;
    } else {
      conversation = inviteeUserIDList;
    }

    const invitationView = reactive(new InvitationView(uuidv4(), type));
    invitationView.isMyCall = true;
    invitationView.inviteeUserIDList = [conversation.userID];
    invitationView.conversation = conversation;
    const userStore = useUserStore();
    invitationView.inviterUserID = userStore.getMyUserID!;

    InvitationView.optionMap[invitationView.invitationMsgId] = invitationView;
    return invitationView;
  }
  // 我是被呼时创建实例
  static async create2(invitationInfo: InvitationInfo) {
    if (!RTCCtrl.canIUse) {
      callReject();
      throw new Error(
        "收到呼叫，但当前使用http协议，不能调用摄像头麦克风，请使用https协议访问当前页面",
      );
    }
    const { data: conversation } = await IMSDK.getOneConversation({
      sourceID: invitationInfo.inviterUserID,
      sessionType: invitationInfo.groupID
        ? SessionType.Group
        : SessionType.Single,
    });
    const invitationView = reactive(
      new InvitationView(
        invitationInfo.invitationMsgId,
        invitationInfo.mediaType,
      ),
    );

    invitationView.isMyCall = false;
    invitationView.roomID = invitationInfo.roomID;
    invitationView.conversation = conversation;
    invitationView.inviterUserID = invitationInfo.inviterUserID;
    invitationView.inviteeUserIDList = invitationInfo.inviteeUserIDList;

    InvitationView.optionMap[invitationView.invitationMsgId] = invitationView;
    return invitationView;
  }
  static startMeeting(invitationInfo: InvitationInfo) {
    const invitationView =
      InvitationView.optionMap[invitationInfo.invitationMsgId];
    invitationView && invitationView.onClickAccept();
  }
  static update(invitationInfo: InvitationInfo) {
    const invitationView =
      InvitationView.optionMap[invitationInfo.invitationMsgId];
    if (invitationView) {
      AudioCtrl.stop(invitationView.invitationMsgId);
      Object.assign(invitationView, invitationInfo);
    }
  }
  static close(id: string, beforeDestorySendMsg = true) {
    InvitationView.optionMap[id] &&
      InvitationView.optionMap[id].destory(beforeDestorySendMsg);
  }
  getInfo(): InvitationInfo {
    return {
      acceptTime: this.acceptTime,
      duration: this.duration,
      groupID: this.groupID,
      initiateTime: this.initiateTime,
      invitationMsgId: this.invitationMsgId,
      inviteeUserIDList: this.inviteeUserIDList,
      inviterUserID: this.inviterUserID,
      mediaType: this.mediaType,
      opDesc: this.opDesc,
      opType: this.opType,
      platformID: this.platformID,
      roomID: this.roomID,
      sessionType: this.sessionType,
      timeout: this.timeout,
    };
  }
  async open() {
    const option = this.createElMessageBoxOptions();
    ElMessageBox(option).catch(() => {
      this.destory();
    });

    // 注入关闭弹窗方法
    this.closeElbox = () => {
      // @ts-ignore
      option.onVanish(); //调用element api关闭弹窗
    };

    if (this.isMyCall) {
      this.roomID = await RTCCtrl.getRoomID();
      setCurInvitationInfo(this.getInfo());
      startCall(this.conversation!.userID, this.conversation!.groupID);
    } else {
      const title = `邀请我进行${this.mediaType === "audio" ? "语音" : "视频"}通话`;
      Notify.pushCall(title, this.conversation!);
    }
    AudioCtrl.play(this.invitationMsgId, this.isMyCall);
  }
  async destory(sendMsg = true) {
    if (sendMsg) {
      if (this.invitationStatus === "waiting") {
        if (this.isMyCall) {
          await callCancel();
        } else {
          await callReject();
        }
      } else if (this.invitationStatus === "meeting") {
        await callHangUp(await this.getDuration());
      }
    }

    if (this.invitationStatus === "meeting") {
      RTCCtrl.leaveRoom();
    }

    clearInvitationInfo(this.invitationMsgId);

    AudioCtrl.stop(this.invitationMsgId);
    this.calledTimerID && clearTimeout(this.calledTimerID);
    delete InvitationView.optionMap[this.invitationMsgId];
    this.closeElbox?.();
    this.invitationStatus = "ended";
  }
  async getDuration() {
    if (this.acceptTime) {
      return Math.floor(((await getSvrTime()) - this.acceptTime) / 1000);
    }
    return 0;
  }
  async onClickAccept() {
    this.invitationStatus = "calling";

    if (!this.isMyCall) {
      this.calledTimerID && clearTimeout(this.calledTimerID);
      await callAccept();
    }

    AudioCtrl.stop(this.invitationMsgId);
    await RTCCtrl.joinRoomID(this.roomID);
    this.acceptTime = await getSvrTime();
    this.invitationStatus = "meeting";
  }
  createElMessageBoxOptions() {
    const option: ElMessageBoxOptions = {
      title: this.mediaType === "audio" ? "语音呼叫" : "视频呼叫",
      message: h(Call, {
        invitationView: this,
      }),
      // @ts-ignore
      modalClass: "call-zhezhao",
      showClose: true,
      draggable: true,
      showConfirmButton: false,
      closeOnClickModal: false,
    };
    return option;
  }
}
