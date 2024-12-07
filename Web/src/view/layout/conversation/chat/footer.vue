<template>
  <div>
    <div
      v-if="
        conversationStore.isCurrentGroupChat &&
        groupStore.currentGroupInfo?.status === 2
      "
      class="dissolveToast"
    >
      群已解散
    </div>
    <div
      v-else-if="
        conversationStore.isCurrentGroupChat &&
        !groupStore.isCurrentGroupAdmin &&
        groupStore.currentGroupInfo?.status === 3
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
          <el-popover placement="top" :width="526" trigger="click">
            <template #reference>
              <div class="icon">
                <img :src="ExpressionSvg" alt="" />
              </div>
            </template>
            <FaceIcon
              v-for="value in faceUniCodeList"
              :value="value"
              @click="insertFace(value)"
              class="face-group-icon"
            />
          </el-popover>
          <el-tooltip content="发送图片" placement="top" effect="light">
            <div class="icon" @click="photoHandle()">
              <img :src="PhotoSvg" alt="" />
            </div>
          </el-tooltip>
          <el-tooltip content="发送视频" placement="top" effect="light">
            <div class="icon">
              <img :src="VideoSvg" @click="videoHandle()" alt="" />
            </div>
          </el-tooltip>
          <el-tooltip content="发送名片" placement="top" effect="light">
            <div class="icon" @click="dialogVisible = true">
              <img :src="BusinessCardSvg" alt="" />
            </div>
          </el-tooltip>
          <el-tooltip content="发送文件" placement="top" effect="light">
            <div class="icon" @click="fileHandle()">
              <img :src="FileSvg" alt="" />
            </div>
          </el-tooltip>
        </div>

        <div class="context">
          <ChatEditor
            :key="conversationStore.currentConversation?.conversationID"
            v-model:editorRef="editorRef"
            @enter="sendHandle"
            @pasteFile="pasteFileHandle"
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
                  <TextMsgRender
                    v-if="
                      [106, 101, 114].indexOf(
                        conversationStore.currentQuoteMessage.contentType
                      ) > -1
                    "
                    :arr="chatTextSplit(conversationStore.currentQuoteMessage)"
                  />
                  <template v-else>
                    {{ toLastMessage(conversationStore.currentQuoteMessage) }}
                  </template>
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
import { h, onBeforeUnmount, ref, shallowRef, watch } from "vue";
import {
  IMSDK,
  IMTYPE,
  createVideoMessage,
  createImageMessage,
  setDraft,
} from "~/utils/imsdk";
import {
  useAppStore,
  useConversationStore,
  useGroupStore,
  useUserStore,
} from "~/stores";
import {
  clone,
  importFile,
  getVideoSnshotFile,
  toLastMessage,
  errorHandle,
  chatTextSplit,
} from "~/utils";
import {
  MemberSelect,
  SendBusinessCard,
  FaceIcon,
  ChatEditor,
  TextMsgRender,
} from "~/components";
import transmitImg from "~/assets/icons/transmit.svg";
import { MsgItem } from "~/stores/type";
import { v4 as uuidV4 } from "uuid";
import { FaceElement } from "~/utils/wangeditor";
import { getFaceUnicodeList, unicodeToChar } from "~/utils/face";
import { IDomEditor } from "@wangeditor/editor";
import { nextTick } from "vue";
import { onMounted } from "vue";
import { ElMessage, ElMessageBox } from "element-plus";

const dialogVisible = ref(false);

const conversationStore = useConversationStore();
const groupStore = useGroupStore();
const appStore = useAppStore();
const userStore = useUserStore();
const editorRef = shallowRef<IDomEditor>();

let lastText = "";
const faceUniCodeList = getFaceUnicodeList();

watch(
  () => conversationStore.currentConversation?.conversationID,
  (newVal, oldVal) => {
    if (oldVal) {
      const value = editorRef.value?.getText() || "";
      if (lastText !== value) {
        setDraft(oldVal, value);
      }

      editorRef.value = undefined;
    }

    if (newVal) {
      nextTick(() => {
        lastText = conversationStore.currentConversation!.draftText;
        const arr = chatTextSplit(lastText);

        const html = arr
          .map((item) => {
            if (item.type === "text") return item.text;
            if (item.type === "at")
              return `<span data-w-e-type="mention" data-w-e-is-void data-w-e-is-inline data-value="${item.nickname}" data-info="${item.userID}">@${item.nickname}</span>`;
            if (item.type === "face")
              return `<img data-w-e-type="face" data-name="${item.value}" />`;
          })
          .join("");
        // 这里加了一个P标签解决wangEditor内部的一些bug
        editorRef.value?.setHtml(`<p>${html}</p>`);
      });
    }
  }
);

onMounted(() => {
  const conversationID = conversationStore.currentConversation?.conversationID;
  if (conversationID) {
    lastText = conversationStore.currentConversation!.draftText;
    const arr = chatTextSplit(lastText);

    const html = arr
      .map((item) => {
        if (item.type === "text") return item.text;
        if (item.type === "at")
          return `<span data-w-e-type="mention" data-w-e-is-void data-w-e-is-inline data-value="${
            item.nickname
          }" data-info="${encodeURIComponent(item.userID as string)}">@${
            item.nickname
          }</span>`;
        if (item.type === "face")
          return `<img data-w-e-type="face" data-name="${item.value}"/>`;
      })
      .join("");
    editorRef.value?.setHtml(html);
  }
});

onBeforeUnmount(() => {
  const conversationID = conversationStore.currentConversation?.conversationID;
  const value = editorRef.value?.getText() || "";
  if (conversationID && lastText !== value) {
    console.log("set", conversationID, value);
    setDraft(conversationID, value);
  }
});

const insertFace = (value: string) => {
  const myResume: FaceElement = {
    type: "face",
    name: value,
    // @ts-ignore
    children: [{ text: unicodeToChar(value) }],
  };
  document.body.click(); //隐藏表情选择框
  editorRef.value?.restoreSelection();
  editorRef.value?.insertNode(myResume);
  editorRef.value?.move(1);
  setTimeout(() => {
    editorRef.value?.focus();
  });
};

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
  let text = editorRef.value?.getText();
  if (!text) {
    return "";
  }
  let messageItem: IMTYPE.MessageItem;
  if (/@\{\w+\|.+?\}/.test(text)) {
    const atUserList: Record<string, string> = {};
    // 包含@消息
    text = text.replace(
      /@\{(\w+)\|(.+?)\}/g,
      (str: string, userID: string, nickname: string, idx: number) => {
        console.log(str, userID, nickname, idx);
        atUserList[userID] = nickname;
        return `@${userID}`;
      }
    );
    const { data } = await IMSDK.createTextAtMsg({
      text,
      atUserIDList: Object.keys(atUserList),
      atUsersInfo: Object.keys(atUserList).map((userID) => ({
        atUserID: userID,
        groupNickname: atUserList[userID],
      })),
      message: conversationStore.currentQuoteMessage,
    });

    messageItem = data;
  } else if (conversationStore.currentQuoteMessage) {
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
  editorRef.value?.clear();
  if (lastText) {
    const conversationID =
      conversationStore.currentConversation?.conversationID;
    conversationID && setDraft(conversationID, "");
  }

  await sendMsg(messageItem);
};

const pasteFileHandle = async (file: File) => {
  console.log(file);
  if (file.type.includes("image")) {
    const reader = new FileReader();

    reader.onload = async function (e) {
      const src = e.target!.result;
      await ElMessageBox({
        title: "发送图片",
        showCancelButton: true,
        confirmButtonText: "发送",
        cancelButtonText: "取消",
        message: h("img", {
          src: src,
          style: "width:100%;border: 1px solid #ccc",
        }),
      });
      photoHandle(file);
    };

    reader.readAsDataURL(file);
  } else if (file.type.includes("video")) {
    await ElMessageBox({
      title: "发送视频",
      showCancelButton: true,
      confirmButtonText: "发送",
      cancelButtonText: "取消",
      message: `确定要发送视频"${file.name}"吗？`,
    });
    await videoHandle(file);
  } else if (file.size !== 4096) {
    //排除了文件夹

    await ElMessageBox({
      title: "发送文件",
      showCancelButton: true,
      confirmButtonText: "发送",
      cancelButtonText: "取消",
      message: `确定要发送文件"${file.name}"吗？`,
    });
    fileHandle(file);
  }
};

const photoHandle = async (file?: File) => {
  let files;
  if (file) {
    files = [file];
  } else {
    files = await importFile({ accept: "image/*" });
  }
  if (files.length > 3) {
    return ElMessage.error("单次只允许发送3张图片");
  }
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

const videoHandle = async (file?: File) => {
  if (!file) {
    const files = await importFile({ accept: "video/*", multiple: false });
    file = files[0];
  }
  if (!file) return;

  if (file.size > 52428800) {
    return ElMessage.error("视频大小不能超过50M");
  }
  if (!file.type.includes("video")) return;
  const snapShotFile = await getVideoSnshotFile(file);
  const messageItem = await createVideoMessage(file, snapShotFile);
  conversationStore.pushMsg([messageItem]);
  await sendMsg(messageItem);
};
const fileHandle = async (file?: File) => {
  let files;
  if (file) {
    files = [file];
  } else {
    files = await importFile({ accept: "*" });
  }
  if (files.length > 3) {
    return ElMessage.error("单次只允许发送3个文件");
  }

  if (files.some((item) => item.size > 52428800)) {
    return ElMessage.error("单个文件大小不能超过50M");
  }

  files.forEach(async (file: File) => {
    const { data: messageItem } = await IMSDK.createFileMsgByFile({
      filePath: "",
      fileName: file.name,
      uuid: uuidV4(),
      sourceUrl: "",
      fileSize: file.size,
      fileType: file.type,
      file,
    });
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

  conversationStore.multipleStatus = false;

  data.forEach(async (item) => {
    const isCurrentChat =
      item.userID === conversationStore.currentConversation?.userID ||
      item.groupID === conversationStore.currentConversation?.groupID;
    if (isCurrentChat) {
      conversationStore.pushMsg([message]);
    }

    await IMSDK.sendMsg({
      recvID: item.userID ?? "",
      groupID: item.groupID ?? "",
      message: message,
    });

    if (isCurrentChat) {
      message.status = 2;
      conversationStore.updateMsgList([message]);
    }
  });
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
.container {
  display: flex;
  flex-direction: column;
  height: 100%;

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
    height: 100%;
    overflow: hidden;
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
