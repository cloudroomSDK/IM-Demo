<template>
  <div class="container" v-loading="loading">
    <Empty v-if="!conversationStore.msgList.length && !loading" />
    <VirtualList
      v-show="conversationStore.msgList.length"
      class="list"
      :style="{
        paddingTop: overflow ? 0 : '20px',
      }"
      ref="vsl"
      data-key="clientMsgID"
      :data-sources="conversationStore.msgList"
      :data-component="MessageItem"
      :estimate-size="200"
      @resized="onItemRendered"
      @totop="onTotop"
      @scroll="onListScroll"
    >
      <template #header>
        <div class="header" v-if="overflow">
          <div class="finished" v-if="canLoading">没有更多了</div>
          <div class="spinner" v-else></div>
        </div>
      </template>
    </VirtualList>
    <div v-if="newMsgCount" class="newMsg" @click="goToBottom">
      {{ newMsgCount }}条新消息
    </div>
  </div>
</template>
<script lang="ts" setup>
import { IMSDK, IMTYPE } from "~/utils/imsdk";
import { useConversationStore, useUserStore } from "~/stores";
import { Empty } from "~/components";
import { nextTick, onBeforeMount, onBeforeUnmount, ref } from "vue";
//@ts-ignore
import VirtualList from "vue3-virtual-scroll-list";
import MessageItem from "./messageItem.vue";

const userStore = useUserStore();
const conversationStore = useConversationStore();
const newMsgCount = ref(0);

const loading = ref(true);
const overflow = ref(false);
const canLoading = ref(false); //可以加载更多消息
const vsl = ref();

let isFirstRender: boolean = true;

const maxUpdateLength = 20; //每次拉取消息的长度

const refershList = async () => {
  const msgLength = await conversationStore.refershMsgList(maxUpdateLength);

  canLoading.value = msgLength < maxUpdateLength;
};

onBeforeMount(async () => {
  await refershList();
  loading.value = false;
});

const goToBottom = () => {
  newMsgCount.value = 0;
  vsl.value.scrollToBottom();
};

const onTotop = async () => {
  if (canLoading.value) return;
  const msgLength = await conversationStore.refershMsgList(maxUpdateLength);
  canLoading.value = msgLength < maxUpdateLength;
  await nextTick();

  if (vsl.value) {
    const newMsgIds = conversationStore.msgList
      .slice(0, msgLength)
      .map((item) => item.clientMsgID);
    const offset = newMsgIds.reduce((previousValue: any, currentSid: any) => {
      const previousSize =
        typeof previousValue === "string"
          ? vsl.value.getSize(previousValue)
          : previousValue;

      return previousSize + vsl.value.getSize(currentSid);
    });
    vsl.value.scrollToOffset(offset);
  }
};

const onItemRendered = (id: string, size: number) => {
  if (!vsl.value) return;

  if (
    isFirstRender &&
    conversationStore.msgList.length <= vsl.value.getSizes()
  ) {
    isFirstRender = false;
    vsl.value.scrollToBottom();
  }

  if (!isFirstRender) {
    // 如果聊天记录窗口有滚动条，则显示顶部loadding
    overflow.value = vsl.value.getScrollSize() > vsl.value.getClientSize();

    // 如果是新消息，将滚动条滚到底部
    if (
      size + vsl.value.getClientSize() + vsl.value.getOffset() ===
      vsl.value.getScrollSize()
    ) {
      vsl.value.scrollToBottom();
    }
  }
};

const inCurrentConversation = (messageItem: IMTYPE.MessageItem): Boolean => {
  switch (messageItem.sessionType) {
    case 1:
      return (
        messageItem.sendID === conversationStore.currentConversation?.userID ||
        (messageItem.sendID === userStore.userInfo?.userID &&
          messageItem.recvID === conversationStore.currentConversation?.userID)
      );
    case 3:
      return (
        messageItem.groupID === conversationStore.currentConversation?.groupID
      );
    case 4:
      return (
        messageItem.sendID === conversationStore.currentConversation?.userID
      );
    default:
      return false;
  }
};

const onRecvNewMsgs = async ({ data }: { data: IMTYPE.MessageItem[] }) => {
  const validMsgList = data.filter(
    (messageItem) =>
      inCurrentConversation(messageItem) &&
      ![113, 2101].includes(messageItem.contentType)
  );

  if (validMsgList.length) {
    // 清空当前绘画未读数
    conversationStore.currentConversation?.conversationID &&
      IMSDK.markConversationMsgAsRead(
        conversationStore.currentConversation.conversationID
      );
    conversationStore.pushMsg(validMsgList);
    // 计算滚动条是否在最底部
    const isBottom =
      vsl.value.getClientSize() + vsl.value.getOffset() ===
      vsl.value.getScrollSize();
    if (!isBottom) {
      newMsgCount.value++;
    }
  }
};

const onNewRecvMsgRevoked = ({ data }: { data: IMTYPE.RevokedInfo }) => {
  const messageItem = {
    clientMsgID: data.clientMsgID,
    contentType: 2101,
    notificationElem: {
      detail: JSON.stringify(data),
    },
  };

  conversationStore.updateMsgList([messageItem as IMTYPE.MessageItem]);
};
const onListScroll = () => {
  if (
    newMsgCount.value &&
    vsl.value.getClientSize() + vsl.value.getOffset() ===
      vsl.value.getScrollSize()
  ) {
    newMsgCount.value = 0;
  }
};

IMSDK.on("OnRecvNewMsgs", onRecvNewMsgs);
IMSDK.on("OnNewRecvMsgRevoked", onNewRecvMsgRevoked);

onBeforeUnmount(() => {
  IMSDK.off("OnRecvNewMsgs", onRecvNewMsgs);
  IMSDK.off("OnNewRecvMsgRevoked", onNewRecvMsgRevoked);
});
</script>
<style lang="scss" scoped>
.container {
  height: 100%;
  position: relative;
  .list {
    height: 100%;
    min-width: 600px;
    padding: 0 16px 0px;
    overflow: auto;
  }
  .header {
    margin: 20px 0;
    text-align: center;
    .finished {
      font-size: 14px;
      color: #ccc;
    }
    .spinner {
      font-size: 10px;
      margin: 0px auto;
      text-indent: -9999em;
      width: 15px;
      height: 15px;
      border-radius: 50%;
      background: #ffffff;
      background: linear-gradient(
        to right,
        #ccc 10%,
        rgba(255, 255, 255, 0) 42%
      );
      position: relative;
      animation: load3 1.4s infinite linear;
      transform: translateZ(0);
    }
    .spinner:before {
      width: 50%;
      height: 50%;
      background: #ccc;
      border-radius: 100% 0 0 0;
      position: absolute;
      top: 0;
      left: 0;
      content: "";
    }
    .spinner:after {
      background: #ffffff;
      width: 75%;
      height: 75%;
      border-radius: 50%;
      content: "";
      margin: auto;
      position: absolute;
      top: 0;
      left: 0;
      bottom: 0;
      right: 0;
    }
    @-webkit-keyframes load3 {
      0% {
        transform: rotate(0deg);
      }
      100% {
        transform: rotate(360deg);
      }
    }
    @keyframes load3 {
      0% {
        transform: rotate(0deg);
      }
      100% {
        transform: rotate(360deg);
      }
    }
  }
  .newMsg {
    position: absolute;
    right: 20px;
    bottom: 20px;
    box-shadow: var(--el-box-shadow);
    padding: 6px 14px;
    border-radius: 20px;
    color: var(--el-color-primary);
    cursor: pointer;
    font-size: 14px;
    background-color: rgba(255, 255, 255, 0.5);
  }
}
</style>
