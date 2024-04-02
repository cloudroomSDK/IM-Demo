<template>
  <div>
    <div class="card">
      <InputItem
        type="switch"
        text="置顶"
        :value="conversationInfo.isPinned"
        @change="changePinned"
      />
      <InputItem
        type="switch"
        text="消息免打扰"
        :value="conversationInfo.recvMsgOpt === 2"
        @change="changeRecvMsgOpt"
      />
      <InputItem
        v-if="isBlack !== undefined"
        type="switch"
        text="黑名单"
        :value="isBlack"
        @change="changeBlackStatus"
      />
    </div>
    <div class="card">
      <InputItem
        type="btn"
        text="清空聊天记录"
        showArrow
        @click="clearHistory"
      />
    </div>
  </div>
</template>
<script lang="ts" setup>
import { clearConversationMsg, IMSDK, IMTYPE } from "~/utils/imsdk";
import { InputItem } from ".";
import { ElMessageBox } from "element-plus";
import { onBeforeMount, ref } from "vue";
import { useFriendStore } from "~/stores";

const friendStore = useFriendStore();

const props = defineProps<{
  conversationInfo: IMTYPE.ConversationItem;
}>();

const isBlack = ref();

onBeforeMount(async () => {
  isBlack.value = !!(await friendStore.getBlackInfo(
    props.conversationInfo.userID
  ));
});

const changePinned = (val: boolean) => {
  IMSDK.pinConversation({
    conversationID: props.conversationInfo.conversationID,
    isPinned: val,
  });
};
const changePrivateChat = (val: boolean) => {
  IMSDK.setConversationPrivateChat({
    conversationID: props.conversationInfo.conversationID,
    isPrivate: val,
  });
};
const changeRecvMsgOpt = (val: boolean) => {
  IMSDK.setConversationRecvMsgOpt({
    conversationID: props.conversationInfo.conversationID,
    opt: val ? 2 : 0,
  });
};
const changeBlackStatus = (val: boolean) => {
  if (val) {
    IMSDK.addBlack(props.conversationInfo.userID);
  } else {
    IMSDK.removeBlack(props.conversationInfo.userID);
  }
};
const clearHistory = async () => {
  await ElMessageBox.confirm(
    `确认删除与 ${props.conversationInfo.showName} 的聊天记录吗?`,
    "清空聊天记录",
    {
      confirmButtonText: "确定",
      cancelButtonText: "取消",
      type: "warning",
    }
  );
  clearConversationMsg(props.conversationInfo.conversationID);
};
</script>
<style lang="scss" scoped>
.card {
  margin-bottom: 8px;
  background-color: #fff;
}
</style>
