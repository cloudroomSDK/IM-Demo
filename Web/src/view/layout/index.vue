<template>
  <el-container class="container">
    <el-aside class="aside">
      <Aside />
    </el-aside>
    <div v-if="userStore.isSyncing" class="sync">
      <div
        v-loading="true"
        class="sync-loading"
        element-loading-background="transparent"
      ></div>
      <p class="text">数据同步中，进度: {{ userStore.syncProgress }}%</p>
    </div>
    <template v-else>
      <div class="mian">
        <router-view style="height: 100%" />
      </div>
      <div class="logging" v-if="showLoading">
        <div v-loading="true" class="loading" />
        正在连接
      </div>
    </template>
  </el-container>

  <el-dialog
    v-model="appStore.globalDialog.show"
    :title="appStore.globalDialog.title"
    :width="appStore.globalDialog.width"
    destroy-on-close
    draggable
  >
    <component
      :is="appStore.globalDialog.component"
      :data="appStore.globalDialog.data"
      @success="appStore.globalDialog.confirm"
      @close="appStore.globalDialog.cancel"
    />
  </el-dialog>
</template>

<script setup lang="ts">
import { ElMessage } from "element-plus";
import { onUnmounted, provide, ref } from "vue";
import {
  useAppStore,
  useUserStore,
  useConversationStore,
  useFriendStore,
  useGroupStore,
} from "~/stores";
import {
  CbEvents,
  ConversationItem,
  IMSDK,
  MessageItem,
  MessageReceiveOptType,
  MessageType,
  SessionType,
} from "~/utils/imsdk";
import Aside from "./aside.vue";
import { InvitationOpType, setInvitationCallback } from "~/utils/callModule";
import { InvitationView } from "~/utils/InvitationView";
import { AudioPlayer } from "~/utils/audioPlayer";
import MessageRing from "~/assets/audio/message_ring.wav";
import { Notify } from "~/utils/systemNotify";

Notify.init();
const appStore = useAppStore();
const userStore = useUserStore();
const friendStore = useFriendStore();
const conversationStore = useConversationStore();
const groupStore = useGroupStore();

const dropdownRefArr: any[] = [];
provide("dropdownVisibleChange", (visible: boolean, ref: any) => {
  if (visible) {
    let dropdownRef;
    while ((dropdownRef = dropdownRefArr.pop())) {
      dropdownRef.handleClose();
    }
    dropdownRefArr.push(ref);
  } else {
    const idx = dropdownRefArr.findIndex((item) => item === ref);
    if (idx > -1) {
      dropdownRefArr.splice(idx, 1);
    }
  }
});

const showLoading = ref(false);

const onConnecting = () => {
  showLoading.value = true;
};
const onConnectSuccess = () => {
  showLoading.value = false;
};
const onKickedOffline = () => {
  ElMessage({
    message: "您已在别处登录",
    type: "warning",
  });
  userStore.exit();
};
const onUserTokenExpired = () => {
  ElMessage({
    message: "登录已失效",
    type: "warning",
  });
  userStore.exit();
};

// 收到消息，播放响铃和系统通知
const onRecvNewMsgs = async ({ data }: { data: MessageItem[] }) => {
  const msg = data.find((item, idx) => {
    if (item.sendID === userStore.getMyUserID) {
      return false;
    }
    const conversationItem = conversationStore.conversationList?.find(
      (conversation) => {
        if (item.sessionType === SessionType.Group && conversation.groupID) {
          return item.groupID === conversation.groupID;
        }
        if (
          item.sessionType === SessionType.Single &&
          conversation.userID &&
          conversation.userID !== userStore.getMyUserID
        ) {
          return item.sendID === conversation.userID;
        }
      },
    );

    if (
      conversationItem &&
      conversationItem.recvMsgOpt !== MessageReceiveOptType.Normal
    ) {
      return false;
    }

    if (
      item.contentType >= MessageType.TextMessage &&
      item.contentType < MessageType.CustomMessage
    ) {
      return true;
    }
    return false;
  });

  if (msg) {
    const player = new AudioPlayer(MessageRing);
    player.play();
    
    Notify.pushMsg(msg);
  }
};

setInvitationCallback({
  async notifyCallIn(invitationInfo) {
    try {
      const invitationView = await InvitationView.create2(invitationInfo);
      invitationView.open();
    } catch (error: unknown) {
      const message = error instanceof Error ? error.message : "收到呼叫邀请";
      console.error(error);
      ElMessage.error(message);
    }
  },
  notifyCallAccepted(invitationInfo) {
    console.log("notifyCallAccepted", invitationInfo);
    InvitationView.startMeeting(invitationInfo);
  },
  notifyCallRejected(invitationInfo) {
    console.log("notifyCallRejected", invitationInfo);
    InvitationView.update(invitationInfo);
  },
  notifyCallCancel(invitationInfo) {
    console.log("notifyCallCancel", invitationInfo);
    InvitationView.close(invitationInfo.invitationMsgId, false);
  },
  notifyCallHangUp(invitationInfo) {
    console.log("notifyCallHangUp", invitationInfo);
    InvitationView.close(invitationInfo.invitationMsgId, false);
  },
  notifyInsertMessage(msg) {
    if (conversationStore.currentConversation?.userID === msg.recvID) {
      conversationStore.pushMsg([msg]);
    }
  },
  notifyCallAcceptedByOtherDevice(invitationInfo) {
    console.log("notifyCallAcceptedByOtherDevice", invitationInfo);
    InvitationView.update(
      Object.assign(invitationInfo, {
        invitationStatus: "ended",
        opType: InvitationOpType.AcceptedByOther,
      }),
    );
  },
  notifyCallRejectedByOtherDevice(invitationInfo) {
    console.log("notifyCallRejectedByOtherDevice", invitationInfo);
    InvitationView.update(
      Object.assign(invitationInfo, {
        invitationStatus: "ended",
        opType: InvitationOpType.RejectedByOther,
      }),
    );
  },
});

IMSDK.on(CbEvents.OnConnecting, onConnecting);
IMSDK.on(CbEvents.OnConnectSuccess, onConnectSuccess);
IMSDK.on(CbEvents.OnKickedOffline, onKickedOffline);
IMSDK.on(CbEvents.OnUserTokenExpired, onUserTokenExpired);
IMSDK.on(CbEvents.OnRecvNewMsgs, onRecvNewMsgs);
IMSDK.on(CbEvents.OnNewConversation, conversationStore.onNewConversation);
IMSDK.on(
  CbEvents.OnConversationChanged,
  conversationStore.onConversationChanged,
);
IMSDK.on(
  CbEvents.OnTotalUnreadMsgCountChanged,
  conversationStore.onTotalUnreadMsgCountChanged,
);
IMSDK.on(CbEvents.OnFriendAdded, friendStore.onFriendAdded);
IMSDK.on(CbEvents.OnFriendDeleted, friendStore.onFriendDeleted);
IMSDK.on(CbEvents.OnFriendInfoChanged, friendStore.onFriendInfoChanged);
IMSDK.on(CbEvents.OnFriendReqAdded, friendStore.onFriendReqAdded);
IMSDK.on(CbEvents.OnBlackAdded, friendStore.onBlackAdded);
IMSDK.on(CbEvents.OnBlackDeleted, friendStore.onBlackDeleted);
IMSDK.on(CbEvents.OnJoinedGrpAdded, groupStore.onJoinedGrpAdded);
IMSDK.on(CbEvents.OnJoinedGrpDeleted, groupStore.onJoinedGrpDeleted);
IMSDK.on(CbEvents.OnGrpInfoChanged, groupStore.onGrpInfoChanged);
IMSDK.on(CbEvents.OnGrpDismissed, groupStore.onGrpDismissed);
IMSDK.on(CbEvents.OnGrpReqAdded, groupStore.onGrpReqAdded);
IMSDK.on(CbEvents.OnGrpMemberAdded, groupStore.onGrpMemberAdded);
IMSDK.on(CbEvents.OnGrpMemberDeleted, groupStore.onGrpMemberDeleted);
IMSDK.on(CbEvents.OnGrpMemberInfoChanged, groupStore.onGrpMemberInfoChanged);

friendStore.init();
conversationStore.init();

onUnmounted(() => {
  IMSDK.off(CbEvents.OnConnecting, onConnecting);
  IMSDK.off(CbEvents.OnConnectSuccess, onConnectSuccess);
  IMSDK.off(CbEvents.OnKickedOffline, onKickedOffline);
  IMSDK.off(CbEvents.OnUserTokenExpired, onUserTokenExpired);
  IMSDK.off(CbEvents.OnRecvNewMsgs, onRecvNewMsgs);
  IMSDK.off(CbEvents.OnNewConversation, conversationStore.onNewConversation);
  IMSDK.off(
    CbEvents.OnConversationChanged,
    conversationStore.onConversationChanged,
  );
  IMSDK.off(
    CbEvents.OnTotalUnreadMsgCountChanged,
    conversationStore.onTotalUnreadMsgCountChanged,
  );
  IMSDK.off(CbEvents.OnFriendAdded, friendStore.onFriendAdded);
  IMSDK.off(CbEvents.OnFriendDeleted, friendStore.onFriendDeleted);
  IMSDK.off(CbEvents.OnFriendInfoChanged, friendStore.onFriendInfoChanged);
  IMSDK.off(CbEvents.OnFriendReqAdded, friendStore.onFriendReqAdded);
  IMSDK.off(CbEvents.OnBlackAdded, friendStore.onBlackAdded);
  IMSDK.off(CbEvents.OnBlackDeleted, friendStore.onBlackDeleted);
  IMSDK.off(CbEvents.OnJoinedGrpAdded, groupStore.onJoinedGrpAdded);
  IMSDK.off(CbEvents.OnJoinedGrpDeleted, groupStore.onJoinedGrpDeleted);
  IMSDK.off(CbEvents.OnGrpInfoChanged, groupStore.onGrpInfoChanged);
  IMSDK.off(CbEvents.OnGrpDismissed, groupStore.onGrpDismissed);
  IMSDK.off(CbEvents.OnGrpReqAdded, groupStore.onGrpReqAdded);
  IMSDK.off(CbEvents.OnGrpMemberAdded, groupStore.onGrpMemberAdded);
  IMSDK.off(CbEvents.OnGrpMemberDeleted, groupStore.onGrpMemberDeleted);
  IMSDK.off(CbEvents.OnGrpMemberInfoChanged, groupStore.onGrpMemberInfoChanged);

  conversationStore.$reset();
  friendStore.$reset();
  groupStore.$reset();
});
</script>

<style lang="scss" scoped>
.container {
  width: 100%;
  height: 100%;
  display: flex;
  position: relative;
  .aside {
    background-color: #ebebeb;
    width: 65px;
  }
  .mian {
    flex: 1;
    overflow: hidden;
  }
  .sync {
    flex: 1;
    overflow: hidden;
    display: flex;
    flex-direction: column;
    align-items: center;
    padding-top: 200px;
    background-color: rgba(0, 0, 0, 0.05);

    .sync-loading {
      width: 60px;
      height: 60px;
    }
    .text {
      color: var(--el-color-primary);
    }
  }
  .logging {
    position: absolute;
    top: 20px;
    left: 50%;
    transform: translateX(-50%);
    --el-color-primary: var(--el-color-error);
    --el-mask-color: var(--el-color-error-light-9);
    background-color: var(--el-color-error-light-9);
    --el-loading-spinner-size: 20px;
    color: var(--el-color-error);
    padding: 6px 20px;
    line-height: 20px;
    display: flex;
    border-radius: 4px;
    .loading {
      width: 20px;
      height: 20px;
      margin-right: 10px;
    }
  }
}
</style>
