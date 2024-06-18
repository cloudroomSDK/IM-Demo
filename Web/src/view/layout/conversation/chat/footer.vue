<template>
  <div>
    <div
      v-if="
        conversationStore.isCurrentGroupChat &&
        conversationStore.currentGroupInfo?.status === 2
      "
      class="dissolveToast"
    >
      群已解散
    </div>
    <div
      v-else-if="
        conversationStore.isCurrentGroupChat &&
        !conversationStore.isCurrentGroupAdmin &&
        conversationStore.currentGroupInfo?.status === 3
      "
      class="dissolveToast"
    >
      已开启群禁言
    </div>
    <template v-else>
      <div class="transmit" v-if="conversationStore.multipleStatus">
        <el-icon class="icon" @click="conversationStore.multipleStatus = false">
          <i-ep-close />
        </el-icon>
        <div class="btnItem">
          <div class="img" @click="mergeTransmitHandle">
            <img :src="transmitImg" alt="" />
          </div>
          <p>合并转发</p>
        </div>
      </div>
      <div class="container" v-show="!conversationStore.multipleStatus">
        <div class="tool clearfix">
          <el-tooltip content="功能未开发" placement="bottom">
            <div class="icon">
              <img :src="ExpressionSvg" alt="" />
            </div>
          </el-tooltip>
          <div class="icon" @click="photoHandle">
            <img :src="PhotoSvg" alt="" />
          </div>
          <div class="icon">
            <img :src="VideoSvg" @click="videoHandle" alt="" />
          </div>
          <el-tooltip content="功能未开发" placement="bottom">
            <div class="icon">
              <img :src="FileSvg" alt="" />
            </div>
          </el-tooltip>
          <div class="icon" @click="dialogVisible = true">
            <img :src="BusinessCardSvg" alt="" />
          </div>
        </div>
        <div class="context">
          <el-input
            v-model="textarea"
            type="textarea"
            class="textarea"
            resize="none"
            @keydown.prevent.enter="sendHandle"
          />
        </div>
        <div class="bottom clearfix">
          <div class="quote">
            <div v-if="conversationStore.currentQuoteMessage" class="text">
              <div class="bg">
                <div
                  class="button"
                  @click="conversationStore.currentQuoteMessage = undefined"
                >
                  <el-icon :size="12" color="#888" class="icon">
                    <i-ep-close />
                  </el-icon>
                </div>
                <p>
                  回复{{
                    conversationStore.currentQuoteMessage.senderNickname
                  }}:
                  {{ toLastMessage(conversationStore.currentQuoteMessage) }}
                </p>
              </div>
            </div>
          </div>
          <el-tooltip content="按 Enter 可快速发送" placement="top">
            <el-button @click="sendHandle" type="primary" class="btn">
              发送
            </el-button>
          </el-tooltip>
        </div>
      </div>
    </template>
    <!-- 弹出层 -->
    <el-dialog
      v-model="dialogVisible"
      title="选择联系人名片"
      width="330"
      destroy-on-close
    >
      <SendBusinessCard @send="sendBusinessCard" />
    </el-dialog>
  </div>
</template>
<script lang="ts" setup>
import ExpressionSvg from "~/assets/icons/tools-expression.svg";
import PhotoSvg from "~/assets/icons/tools-photo.svg";
import VideoSvg from "~/assets/icons/tools-video.svg";
import FileSvg from "~/assets/icons/tools-file.svg";
import BusinessCardSvg from "~/assets/icons/tools-business-card.svg";
import { onBeforeUnmount, onMounted, ref, watch } from "vue";
import {
  IMSDK,
  IMTYPE,
  createVideoMessage,
  createImageMessage,
  setDraft,
} from "~/utils/imsdk";
import { useAppStore, useConversationStore, useUserStore } from "~/stores";
import {
  clone,
  importFile,
  getVideoSnshotFile,
  toLastMessage,
  errorHandle,
} from "~/utils";
import { MemberSelect, SendBusinessCard } from "~/components";
import transmitImg from "~/assets/icons/transmit.svg";
import { MsgItem } from "~/stores/type";

const dialogVisible = ref(false);

const conversationStore = useConversationStore();
const appStore = useAppStore();
const userStore = useUserStore();

const textarea = ref("");
const getDraft = () => {
  if (conversationStore.currentConversation?.conversationID) {
    textarea.value = conversationStore.currentConversation.draftText;
  }
};
watch(
  () => conversationStore.currentConversation?.conversationID,
  (newVal, oldVal) => {
    if (newVal) {
      if (oldVal) {
        setDraft(oldVal, textarea.value);
      }
      textarea.value = conversationStore.currentConversation!.draftText;
    }
  }
);
getDraft();
onMounted(() => {
  if (conversationStore.currentConversation?.conversationID) {
    textarea.value = conversationStore.currentConversation.draftText;
  }
});

onBeforeUnmount(() => {
  if (conversationStore.currentConversation?.conversationID) {
    setDraft(
      conversationStore.currentConversation.conversationID,
      textarea.value
    );
  }
});

const sendMsg = async (message: IMTYPE.MessageItem) => {
  try {
    const { data: successMessage } = await IMSDK.sendMsg({
      recvID: conversationStore.isCurrentGroupChat
        ? ""
        : conversationStore.currentConversation!.userID,
      groupID: conversationStore.isCurrentGroupChat
        ? conversationStore.currentConversation!.groupID
        : "",
      message: message,
      // offlinePushInfo?: OfflinePush;
    });

    console.log(successMessage);
    conversationStore.updateMsgList([successMessage]);
  } catch (error) {
    errorHandle(error as IMTYPE.WsResponse);
    message.status = 3;
    conversationStore.updateMsgList([message]);
  }
};
const sendHandle = async () => {
  const text = textarea.value.trim();
  if (!text) {
    return "";
  }
  let messageItem: IMTYPE.MessageItem;
  if (conversationStore.currentQuoteMessage) {
    const { data } = await IMSDK.createQuoteMsg({
      text,
      message: conversationStore.currentQuoteMessage,
    });
    messageItem = data;
    conversationStore.currentQuoteMessage = undefined;
  } else {
    const { data } = await IMSDK.createTextMsg(text);
    messageItem = data;
  }
  conversationStore.pushMsg([messageItem]);
  textarea.value = "";

  await sendMsg(messageItem);
};

const photoHandle = async () => {
  const files = await importFile({ accept: "image/*" });
  files.forEach(async (file: File) => {
    if (!file.type.includes("image")) return;

    const messageItem = await createImageMessage(file);
    //克隆是为了优先在本地展示图片，sendMsg的时刻才上传图片到服务器
    const messageClone = clone(messageItem);
    const url = URL.createObjectURL(file);
    messageClone.pictureElem.bigPicture.url = url;
    messageClone.pictureElem.snapshotPicture.url = url;
    messageClone.pictureElem.sourcePicture.url = url;
    conversationStore.pushMsg([messageClone]);
    await sendMsg(messageItem);
  });
};

const videoHandle = async () => {
  const files = await importFile({ accept: "video/*" });
  files.forEach(async (file: File) => {
    if (!file.type.includes("video")) return;
    const snapShotFile = await getVideoSnshotFile(file);
    const messageItem = await createVideoMessage(file, snapShotFile);
    conversationStore.pushMsg([messageItem]);
    await sendMsg(messageItem);
  });
};

//合并转发
const mergeTransmitHandle = async () => {
  const data = await appStore.showDialog<
    {
      userID?: string;
      imgSrc: string;
      showName: string;
      groupID?: string;
    }[]
  >({
    component: MemberSelect,
    title: "选择联系人",
    width: 700,
  });

  let list: MsgItem[] = [];
  conversationStore.multipleSelect.forEach((msgID) => {
    const messageItem = conversationStore.msgList.find(
      (item) => item.clientMsgID === msgID
    );
    if (messageItem) {
      list.push(messageItem);
    }
  });

  const { data: message } = await IMSDK.createMergerMsg({
    messageList: list,
    title: conversationStore.isCurrentGroupChat
      ? "群聊天记录"
      : `${userStore.userInfo?.nickname}和${conversationStore.currentConversation?.showName}的聊天记录`,
    summaryList: list.map(
      (item) => `${item.senderNickname}: ${toLastMessage(item)}`
    ),
  });

  data.forEach((item) => {
    IMSDK.sendMsg({
      recvID: item.userID ?? "",
      groupID: item.groupID ?? "",
      message: message,
    });
    if (
      item.userID === conversationStore.currentConversation?.userID ||
      item.groupID === conversationStore.currentConversation?.groupID
    ) {
      conversationStore.pushMsg([message]);
    }
  });

  conversationStore.multipleStatus = false;
};
const sendBusinessCard = async (friendUserItem: IMTYPE.FriendUserItem) => {
  console.log(friendUserItem);
  const { data: message } = await IMSDK.createCardMsg({
    userID: friendUserItem.userID,
    nickname: friendUserItem.nickname,
    faceURL: friendUserItem.faceURL,
    // ex: "",
  });
  conversationStore.pushMsg([message]);
  sendMsg(message);
  dialogVisible.value = false;
};
</script>
<style lang="scss" scoped>
.dissolveToast {
  display: flex;
  justify-content: center;
  align-items: center;
  height: 100%;
  font-size: 14px;
  color: #666;
  cursor: no-drop;
}
.container {
  display: flex;
  flex-direction: column;
  height: 100%;
  .transmit {
    position: relative;
    height: 100%;
    display: flex;
    justify-content: center;
    align-items: center;

    .icon {
      position: absolute;
      right: 10px;
      top: 10px;
      cursor: pointer;
      font-size: 20px;
    }
    .btnItem {
      .img {
        cursor: pointer;
        background-color: #ededed;
        width: 60px;
        height: 60px;
        border-radius: 50%;
        align-items: center;
        justify-content: center;
        display: flex;
        img {
          width: 24px;
        }
      }
      p {
        font-size: 14px;
        line-height: 14px;
        margin-top: 16px;
      }
    }
  }
  .tool {
    margin-top: 4px;
    .icon {
      float: left;
      width: 28px;
      height: 28px;
      display: flex;
      justify-content: center;
      align-items: center;
      cursor: pointer;
      margin-left: 6px;
      border-radius: 4px;
      &:hover {
        background-color: #e9e9e9;
      }
      img {
        width: 26px;
      }
    }
  }
  .context {
    flex: 1;
    .textarea {
      height: 100%;
      word-break: break-all;
      :deep(textarea) {
        height: 100%;
        box-shadow: none;
      }
    }
  }
  .bottom {
    margin-bottom: 20px;
    margin-right: 20px;
    display: flex;
    .quote {
      flex: 1;
      display: flex;
      align-items: center;
      padding: 0 20px;
      .text {
        display: flex;
        flex: 1;
        align-items: center;
        width: 0;
        .bg {
          display: flex;
          max-width: 100%;
          background-color: #f2f2f2;
          border-radius: 4px;
          align-items: center;
          padding: 0 10px;
          .button {
            margin-right: 10px;
            cursor: pointer;
            background-color: #ccc;
            border-radius: 50%;
            width: 18px;
            height: 18px;
            display: flex;
            justify-content: center;
            align-items: center;
          }
          p {
            flex: 1;
            font-size: 12px;
            line-height: 20px;
            text-overflow: ellipsis;
            overflow: hidden;
            text-wrap: nowrap;
            line-height: 28px;
          }
        }
      }
    }
    .btn {
      width: 64px;
      height: 30px;
      padding: 0;
    }
  }
}
</style>
