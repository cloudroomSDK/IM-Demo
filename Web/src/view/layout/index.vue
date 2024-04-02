<template>
  <el-container class="container">
    <el-aside class="aside">
      <Aside />
    </el-aside>
    <div class="mian">
      <router-view style="height: 100%" />
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
import { onUnmounted } from "vue";
import {
  useAppStore,
  useConversationStore,
  useFriendStore,
  useGroupStore,
} from "~/stores";
import { IMSDK } from "~/utils/imsdk";
import Aside from "./aside.vue";

const appStore = useAppStore();
const friendStore = useFriendStore();
const conversationStore = useConversationStore();
const groupStore = useGroupStore();

const onKickedOffline = () => {};
const onUserTokenExpired = () => {};

IMSDK.on("OnKickedOffline", onKickedOffline);
IMSDK.on("OnUserTokenExpired", onUserTokenExpired);
IMSDK.on("OnNewConversation", conversationStore.onNewConversation);
IMSDK.on("OnConversationChanged", conversationStore.onConversationChanged);
IMSDK.on(
  "OnTotalUnreadMsgCountChanged",
  conversationStore.onTotalUnreadMsgCountChanged
);
IMSDK.on("OnGrpInfoChanged", conversationStore.onGrpInfoChanged);
IMSDK.on("OnGrpDismissed", conversationStore.onGrpDismissed);
IMSDK.on("OnFriendAdded", friendStore.onFriendAdded);
IMSDK.on("OnFriendDeleted", friendStore.onFriendDeleted);
IMSDK.on("OnFriendInfoChanged", friendStore.onFriendInfoChanged);
IMSDK.on("OnFriendReqAdded", friendStore.onFriendReqAdded);
IMSDK.on("OnBlackAdded", friendStore.onBlackAdded);
IMSDK.on("OnBlackDeleted", friendStore.onBlackDeleted);
IMSDK.on("OnJoinedGrpAdded", groupStore.onJoinedGrpAdded);
IMSDK.on("OnJoinedGrpDeleted", groupStore.onJoinedGrpDeleted);
IMSDK.on("OnGrpInfoChanged", groupStore.onGrpInfoChanged);

friendStore.init();
conversationStore.init();

onUnmounted(() => {
  IMSDK.off("OnKickedOffline", onKickedOffline);
  IMSDK.off("OnUserTokenExpired", onUserTokenExpired);
  IMSDK.off("OnNewConversation", conversationStore.onNewConversation);
  IMSDK.off("OnConversationChanged", conversationStore.onConversationChanged);
  IMSDK.off(
    "OnTotalUnreadMsgCountChanged",
    conversationStore.onTotalUnreadMsgCountChanged
  );
  IMSDK.off("OnGrpInfoChanged", conversationStore.onGrpInfoChanged);
  IMSDK.off("OnGrpDismissed", conversationStore.onGrpDismissed);
  IMSDK.off("OnFriendAdded", friendStore.onFriendAdded);
  IMSDK.off("OnFriendDeleted", friendStore.onFriendDeleted);
  IMSDK.off("OnFriendInfoChanged", friendStore.onFriendInfoChanged);
  IMSDK.off("OnFriendReqAdded", friendStore.onFriendReqAdded);
  IMSDK.off("OnBlackAdded", friendStore.onBlackAdded);
  IMSDK.off("OnBlackDeleted", friendStore.onBlackDeleted);
  IMSDK.off("OnJoinedGrpAdded", groupStore.onJoinedGrpAdded);
  IMSDK.off("OnJoinedGrpDeleted", groupStore.onJoinedGrpDeleted);
  IMSDK.off("OnGrpInfoChanged", groupStore.onGrpInfoChanged);

  conversationStore.$reset();
  friendStore.$reset();
});
</script>

<style lang="scss" scoped>
.container {
  width: 100%;
  height: 100%;
  display: flex;
  .aside {
    background-color: #ebebeb;
    width: 65px;
  }
  .mian {
    flex: 1;
    overflow: hidden;
  }
}
</style>
