<template>
  <el-container class="container">
    <el-aside class="aside">
      <Aside />
    </el-aside>
    <div class="mian">
      <router-view style="height: 100%" />
    </div>
    <div class="logging" v-if="showLoading">
      <div v-loading="true" class="loading" />
      正在连接
    </div>
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
import { IMSDK } from "~/utils/imsdk";
import Aside from "./aside.vue";

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

IMSDK.on("OnConnecting", onConnecting);
IMSDK.on("OnConnectSuccess", onConnectSuccess);
IMSDK.on("OnKickedOffline", onKickedOffline);
IMSDK.on("OnUserTokenExpired", onUserTokenExpired);
IMSDK.on("OnNewConversation", conversationStore.onNewConversation);
IMSDK.on("OnConversationChanged", conversationStore.onConversationChanged);
IMSDK.on(
  "OnTotalUnreadMsgCountChanged",
  conversationStore.onTotalUnreadMsgCountChanged
);
IMSDK.on("OnFriendAdded", friendStore.onFriendAdded);
IMSDK.on("OnFriendDeleted", friendStore.onFriendDeleted);
IMSDK.on("OnFriendInfoChanged", friendStore.onFriendInfoChanged);
IMSDK.on("OnFriendReqAdded", friendStore.onFriendReqAdded);
IMSDK.on("OnBlackAdded", friendStore.onBlackAdded);
IMSDK.on("OnBlackDeleted", friendStore.onBlackDeleted);
IMSDK.on("OnJoinedGrpAdded", groupStore.onJoinedGrpAdded);
IMSDK.on("OnJoinedGrpDeleted", groupStore.onJoinedGrpDeleted);
IMSDK.on("OnGrpInfoChanged", groupStore.onGrpInfoChanged);
IMSDK.on("OnGrpDismissed", groupStore.onGrpDismissed);
IMSDK.on("OnGrpReqAdded", groupStore.onGrpReqAdded);
IMSDK.on("OnGrpMemberAdded", groupStore.onGrpMemberAdded);
IMSDK.on("OnGrpMemberDeleted", groupStore.onGrpMemberDeleted);
IMSDK.on("OnGrpMemberInfoChanged", groupStore.onGrpMemberInfoChanged);

friendStore.init();
conversationStore.init();

onUnmounted(() => {
  IMSDK.off("OnConnecting", onConnecting);
  IMSDK.off("OnConnectSuccess", onConnectSuccess);
  IMSDK.off("OnKickedOffline", onKickedOffline);
  IMSDK.off("OnUserTokenExpired", onUserTokenExpired);
  IMSDK.off("OnNewConversation", conversationStore.onNewConversation);
  IMSDK.off("OnConversationChanged", conversationStore.onConversationChanged);
  IMSDK.off(
    "OnTotalUnreadMsgCountChanged",
    conversationStore.onTotalUnreadMsgCountChanged
  );
  IMSDK.off("OnFriendAdded", friendStore.onFriendAdded);
  IMSDK.off("OnFriendDeleted", friendStore.onFriendDeleted);
  IMSDK.off("OnFriendInfoChanged", friendStore.onFriendInfoChanged);
  IMSDK.off("OnFriendReqAdded", friendStore.onFriendReqAdded);
  IMSDK.off("OnBlackAdded", friendStore.onBlackAdded);
  IMSDK.off("OnBlackDeleted", friendStore.onBlackDeleted);
  IMSDK.off("OnJoinedGrpAdded", groupStore.onJoinedGrpAdded);
  IMSDK.off("OnJoinedGrpDeleted", groupStore.onJoinedGrpDeleted);
  IMSDK.off("OnGrpInfoChanged", groupStore.onGrpInfoChanged);
  IMSDK.off("OnGrpDismissed", groupStore.onGrpDismissed);
  IMSDK.off("OnGrpReqAdded", groupStore.onGrpReqAdded);
  IMSDK.off("OnGrpMemberAdded", groupStore.onGrpMemberAdded);
  IMSDK.off("OnGrpMemberDeleted", groupStore.onGrpMemberDeleted);
  IMSDK.off("OnGrpMemberInfoChanged", groupStore.onGrpMemberInfoChanged);

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
