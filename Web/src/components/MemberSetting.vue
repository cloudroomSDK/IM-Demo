<template>
  <div>
    <div class="card">
      <InputItem
        type="switch"
        text="聊天置顶"
        v-model="conversationStore.currentConversation!.isPinned"
        @change="changePinned"
      />
      <InputItem
        type="switch"
        text="消息免打扰"
        v-model="MsgDisturbing"
        @change="changeRecvMsgOpt"
      />
    </div>
    <div class="card">
      <InputItem
        type="switch"
        text="阅后即焚"
        v-model="conversationStore.currentConversation!.isPrivateChat"
        @change="changePrivateChat"
      />
      <InputItem
        v-if="conversationStore.currentConversation?.isPrivateChat"
        type="select"
        text="时间设置"
        :options="burnDurationOptions"
        v-model="conversationStore.currentConversation!.burnDuration"
        @change="changePrivateTime"
      />
    </div>
    <div class="card">
      <InputItem
        type="btn"
        text="清空聊天记录"
        showArrow
        @click="clearHistory"
      />
      <InputItem
        v-if="isBlack !== undefined"
        type="switch"
        text="黑名单"
        v-model="isBlack"
        @change="changeBlackStatus"
      />
    </div>
  </div>
</template>
<script lang="ts" setup>
import { clearConversationMsg, IMSDK } from "~/utils/imsdk";
import { InputItem } from ".";
import { ElMessageBox } from "element-plus";
import { computed, onBeforeMount, ref } from "vue";
import { useFriendStore, useConversationStore } from "~/stores";

const friendStore = useFriendStore();
const conversationStore = useConversationStore();
const isBlack = ref();
const burnDurationOptions = ref([
  {
    text: "30秒",
    value: 30,
  },
  {
    text: "1分钟",
    value: 60,
  },
  {
    text: "5分钟",
    value: 300,
  },
  {
    text: "1小时",
    value: 3600,
  },
]);

const MsgDisturbing = computed(
  () => conversationStore.currentConversation?.recvMsgOpt === 2
);

onBeforeMount(async () => {
  isBlack.value = !!(await friendStore.getBlackInfo(
    conversationStore.currentConversation!.userID
  ));
});

const changePinned = (val: boolean) => {
  IMSDK.pinConversation({
    conversationID: conversationStore.currentConversation!.conversationID,
    isPinned: val,
  });
};

const changePrivateChat = (val: boolean) => {
  if (val && conversationStore.currentConversation?.burnDuration === 0) {
    conversationStore.currentConversation!.burnDuration = 30;
    IMSDK.setConversationBurnDuration({
      conversationID: conversationStore.currentConversation!.conversationID,
      burnDuration: 30,
    });
  }
  IMSDK.setConversationPrivateChat({
    conversationID: conversationStore.currentConversation!.conversationID,
    isPrivate: val,
  });
};
const changePrivateTime = (val: number) => {
  IMSDK.setConversationBurnDuration({
    conversationID: conversationStore.currentConversation!.conversationID,
    burnDuration: val,
  });
};

const changeRecvMsgOpt = (val: boolean) => {
  IMSDK.setConversationRecvMsgOpt({
    conversationID: conversationStore.currentConversation!.conversationID,
    opt: val ? 2 : 0,
  });
};
const changeBlackStatus = (val: boolean) => {
  if (val) {
    IMSDK.addBlack(conversationStore.currentConversation!.userID);
  } else {
    IMSDK.removeBlack(conversationStore.currentConversation!.userID);
  }
};
const clearHistory = async () => {
  await ElMessageBox.confirm(
    `确认删除与 ${
      conversationStore.currentConversation!.showName
    } 的聊天记录吗?`,
    "清空聊天记录",
    {
      confirmButtonText: "确定",
      cancelButtonText: "取消",
      type: "warning",
    }
  );
  clearConversationMsg(conversationStore.currentConversation!.conversationID);
};
</script>
<style lang="scss" scoped>
.card {
  margin-bottom: 8px;
  background-color: #fff;
}
</style>
