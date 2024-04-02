<template>
  <div class="item" :class="{ quote: isQuote }">
    <template
      v-if="
        [101, 102, 103, 104, 105, 106, 107, 108, 109, 114].includes(
          source.contentType
        )
      "
    >
      <p v-if="source.isShowTime" class="time">
        {{ formatChatString(source.sendTime) }}
      </p>
      <div
        class="chat"
        :class="{
          isMe: source.sendID === userStore.userInfo?.userID,
        }"
      >
        <div
          v-if="conversationStore.multipleStatus && !isQuote"
          class="checkbox"
        >
          <el-checkbox-group v-model="conversationStore.multipleSelect">
            <el-checkbox size="large" :label="source.clientMsgID">
              <template />
            </el-checkbox>
          </el-checkbox-group>
        </div>
        <Avatar
          class="avatar"
          :src="source.senderFaceUrl"
          v-if="!isQuote"
          @click="
            openUserInfo(
              source.sendID,
              conversationStore.currentConversation?.groupID
            )
          "
        />
        <div class="context">
          <span class="name">{{ source.senderNickname }}</span>
          <div class="message">
            <MessagePopover
              :has="!isQuote"
              :message="source"
              @transmit="transmitHandle"
              @multiple="multipleHandle"
              @quote="quoteHandle"
              @revok="revokHandle"
              @del="delHandle"
            >
              <div class="content">
                <!-- 普通文本消息 -->
                <div v-if="source.contentType === 101">
                  <span class="text">
                    {{ source?.textElem?.content }}
                  </span>
                </div>
                <!-- 图片 -->
                <div v-else-if="source.contentType === 102" class="pic">
                  <el-image
                    class="img"
                    :src="source.pictureElem.sourcePicture.url"
                    :previewSrcList="[source.pictureElem.sourcePicture.url]"
                    loading="lazy"
                  />
                </div>
                <p v-else-if="source.contentType === 103">[暂不支持语音消息]</p>
                <!-- 视频 -->
                <div
                  v-else-if="source.contentType === 104"
                  class="pic"
                  @click="openMediaPreview(source, '')"
                >
                  <el-icon class="icon-video"><i-ep-videoPlay /></el-icon>
                  <el-image
                    class="img"
                    :src="source.videoElem.snapshotUrl"
                    loading="lazy"
                  />
                </div>
                <!-- 文件 -->
                <div
                  v-else-if="source.contentType === 105"
                  class="file"
                  @click="downloadUrl(source.fileElem.sourceUrl)"
                >
                  <div class="l">
                    <span class="filename" :title="source.fileElem.fileName">
                      {{ source.fileElem.fileName }}
                    </span>
                    <span class="fileSize">
                      {{ formatFileSize(source.fileElem.fileSize) }}
                    </span>
                  </div>
                  <div class="r">
                    <el-icon size="40px"><i-ep-document /></el-icon>
                  </div>
                </div>
                <!-- @消息 -->
                <div v-else-if="source.contentType === 106">
                  <span class="text">
                    <template v-for="(obj, i) in data" :key="i">
                      <Nickname
                        v-if="obj.type === 'at'"
                        class="at"
                        :nickname="obj.nickname"
                        :userID="obj.userID"
                        showAt
                      />
                      <template v-else>{{ obj.text }}</template>
                    </template>
                  </span>
                </div>
                <!-- 聊天记录 -->
                <div v-else-if="source.contentType === 107" class="chatRecords">
                  <p class="title">{{ source.mergeElem.title }}</p>
                  <div
                    class="chatItem"
                    v-for="item in source.mergeElem.abstractList"
                  >
                    {{ item }}
                  </div>
                </div>
                <!-- 引用消息 -->
                <template v-else-if="source.contentType === 114">
                  <div>
                    <span class="text">
                      {{ source?.quoteElem?.text }}
                    </span>
                  </div>
                  <MessageItem
                    :source="source.quoteElem.quoteMessage"
                    isQuote
                  ></MessageItem>
                </template>
                <!-- 名片 -->
                <div
                  v-else-if="source.contentType === 108"
                  class="businessCard"
                  @click="
                    openUserInfo(
                      source.cardElem.userID,
                      conversationStore.currentConversation?.groupID
                    )
                  "
                >
                  <div class="info">
                    <Avatar :src="source.cardElem.faceURL" />
                    <span class="nickname">{{ source.cardElem.nickname }}</span>
                  </div>
                  <p>个人名片</p>
                </div>
                <!-- 定位 -->
                <div v-else-if="source.contentType === 109" class="map">
                  <p class="locationName">{{ data.name }}</p>
                  <p class="locationDesc">{{ data.addr }}</p>
                </div>
                <p v-else>[暂不支持该内容]</p>
              </div>
            </MessagePopover>
          </div>
        </div>
        <template v-if="!isQuote">
          <div v-if="source.status === 1" class="icon" v-loading="true"></div>
          <div v-else-if="source.status === 3" class="icon">
            <el-icon color="#f00" size="20px"><i-ep-warningFilled /></el-icon>
          </div>
        </template>
      </div>
    </template>
    <div class="centerToast" v-else-if="source.contentType === 1201">
      <span class="toast">你们已经成为好友，可以开始聊天了</span>
    </div>
    <div class="centerToast" v-else-if="source.contentType === 1501">
      <span class="toast">
        <Nickname
          :nickname="data.opUser.nickname"
          :userID="data.opUser.userID"
        />
        创建了群聊
      </span>
    </div>
    <div class="centerToast" v-else-if="source.contentType === 1504">
      <span class="toast">
        <Nickname
          :nickname="data.quitUser.nickname"
          :userID="data.quitUser.userID"
        />
        退出了群聊
      </span>
    </div>
    <div class="centerToast" v-else-if="source.contentType === 1507">
      <span class="toast">
        <Nickname
          :nickname="data.opUser.nickname"
          :userID="data.opUser.userID"
        />
        将群主转让给了
        <Nickname
          :nickname="data.newGroupOwner.nickname"
          :userID="data.newGroupOwner.userID"
        />
      </span>
    </div>
    <div class="centerToast" v-else-if="source.contentType === 1508">
      <span class="toast">
        <Nickname :infos="data.kickedUserList" />
        被
        <Nickname
          :nickname="data.opUser.nickname"
          :userID="data.opUser.userID"
        />
        踢出群聊</span
      >
    </div>
    <div class="centerToast" v-else-if="source.contentType === 1509">
      <span class="toast">
        <Nickname
          :nickname="data.opUser.nickname"
          :userID="data.opUser.userID"
        />
        邀请
        <Nickname :infos="data.invitedUserList" />
        加入群聊
      </span>
    </div>
    <div class="centerToast" v-else-if="source.contentType === 1511">
      <span class="toast">
        <Nickname
          :nickname="data.opUser.nickname"
          :userID="data.opUser.userID"
        />
        解散了群聊
      </span>
    </div>
    <div class="centerToast" v-else-if="source.contentType === 1512">
      <span class="toast">
        <Nickname
          :nickname="data.mutedUser.nickname"
          :userID="data.mutedUser.userID"
        />
        被
        <Nickname
          :nickname="data.opUser.nickname"
          :userID="data.opUser.userID"
        />
        禁言{{ ~~(data.mutedSeconds / 60) }}分{{ data.mutedSeconds % 60 }}秒
      </span>
    </div>
    <div class="centerToast" v-else-if="source.contentType === 1514">
      <span class="toast">
        <Nickname
          :nickname="data.opUser.nickname"
          :userID="data.opUser.userID"
        />
        开启了群禁言
      </span>
    </div>
    <div class="centerToast" v-else-if="source.contentType === 1515">
      <span class="toast">
        <Nickname
          :nickname="data.opUser.nickname"
          :userID="data.opUser.userID"
        />
        关闭了群禁言
      </span>
    </div>
    <div class="centerToast" v-else-if="source.contentType === 1520">
      <span class="toast">
        <Nickname
          :nickname="data.opUser.nickname"
          :userID="data.opUser.userID"
        />
        修改了群名称
      </span>
    </div>
    <div class="centerToast" v-else-if="source.contentType === 1701">
      <span class="toast">
        已{{ data.isPrivate ? "开启" : "关闭" }}阅后即焚
      </span>
    </div>
    <div class="centerToast" v-else-if="source.contentType === 2101">
      <span class="toast">
        <Nickname
          v-if="data.revokerID !== userStore.userInfo?.userID"
          :nickname="data.sourceMessageSenderNickname"
          :userID="data.sourceMessageSendID"
        />
        <Nickname
          v-else="data.revokerID === data.sourceMessageSendID"
          nickname="你"
          :userID="userStore.userInfo?.userID"
        />
        <Nickname
          v-else="data.revokerID === data.sourceMessageSendID"
          :nickname="data.revokerNickname"
          :userID="data.revokerID"
        />
        撤回了一条消息
      </span>
    </div>
    <div v-else>{{ source }}</div>
  </div>
</template>
<script setup lang="ts">
import { ElMessageBox } from "element-plus";
import { computed, h } from "vue";
import { IMSDK, IMTYPE } from "~/utils/imsdk";
import { PicPreview, Avatar, MessagePopover, MemberSelect } from "~/components";
import { formatChatString } from "~/utils/dayjs";
import MessageItem from "./messageItem.vue";
import Nickname from "./nickname.vue";
import { downloadUrl, openUserInfo, formatFileSize } from "~/utils";
import { useAppStore, useConversationStore, useUserStore } from "~/stores";

const appStore = useAppStore();
const userStore = useUserStore();
const conversationStore = useConversationStore();

type MsgItem = IMTYPE.MessageItem & {
  isShowTime?: boolean;
};
const props = defineProps<{
  source: MsgItem;
  isQuote?: boolean;
}>();

const data = computed(() => {
  if (
    [
      1501, 1504, 1507, 1508, 1509, 1511, 1512, 1514, 1515, 1520, 1701, 2101,
    ].includes(props.source.contentType)
  ) {
    return JSON.parse(props.source.notificationElem.detail);
  }
  // @消息
  if (props.source.contentType === 106) {
    // 使用此处代码分割，注意数据渲染时的xss攻击
    const regex = /(@\S+\s)|(\S+)/g;
    const result = [];
    const { text, atUsersInfo = [] } = props.source.atTextElem;
    let match;
    while ((match = regex.exec(text)) !== null) {
      if (match[1]) {
        const userID = match[1].slice(1, -1);
        const member = atUsersInfo.find((user) => user.atUserID === userID);
        if (member) {
          result.push({
            type: "at",
            userID: member?.atUserID,
            nickname: `${member?.groupNickname}`,
          });
        } else {
          result.push({
            type: "text",
            text: match[1],
          });
        }
      } else {
        result.push({
          type: "text",
          text: match[2],
        });
      }
    }
    return result;
  }

  if (props.source.contentType === 109) {
    return JSON.parse(props.source.locationElem.description);
  }
  return null;
});

const openMediaPreview = (chat: IMTYPE.MessageItem, title: string) => {
  ElMessageBox({
    title: title,
    message: h(PicPreview, {
      chat,
    }),
    showClose: true,
    customStyle: { maxWidth: "90vw", maxHeight: "90vh" },
    showConfirmButton: false,
  });
};

const revokHandle = () => {
  IMSDK.revokeMsg({
    conversationID: conversationStore.currentConversation!.conversationID,
    clientMsgID: props.source.clientMsgID,
  });
};
const transmitHandle = async () => {
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

  const message = (await IMSDK.createForwardMsg(props.source)).data;
  data.forEach(async (item) => {
    if (
      item.userID === conversationStore.currentConversation?.userID ||
      item.groupID === conversationStore.currentConversation?.groupID
    ) {
      conversationStore.pushMsg([message]);
    }
    await IMSDK.sendMsg({
      recvID: item.userID ?? "",
      groupID: item.groupID ?? "",
      message: message,
    });
  });
};
const multipleHandle = () => {
  conversationStore.multipleSelect = [props.source.clientMsgID];
  conversationStore.multipleStatus = true;
};
const quoteHandle = () => {
  conversationStore.currentQuoteMessage = props.source;
};
const delHandle = async () => {
  // await IMSDK.deleteMsgFromLocalStorage({
  //   conversationID: conversationStore.currentConversation!.conversationID,
  //   clientMsgID: props.source.clientMsgID,
  // });

  await IMSDK.deleteMsg({
    conversationID: conversationStore.currentConversation!.conversationID,
    clientMsgID: props.source.clientMsgID,
  });
  conversationStore.removeList([props.source.clientMsgID]);
};
</script>

<style lang="scss" scoped>
.item {
  padding-bottom: 24px;
  word-break: break-all;
  .time {
    text-align: center;
    color: #999999;
    font-size: 14px;
    margin-bottom: 20px;
  }
  .chat {
    display: flex;
    .avatar {
      cursor: pointer;
    }
    .context {
      margin: -4px 12px 0 12px;
      display: inline-block;
      max-width: 80%;
      .name {
        display: block;
        font-size: 14px;
        margin-bottom: 6px;
        line-height: 18px;
        text-wrap: nowrap;
        overflow: hidden;
        text-overflow: ellipsis;
      }
      .content {
        position: relative;
        display: inline-block;
        .text {
          font-size: 16px;
          line-height: 26px;
          display: inline-block;
          background-color: #f5f5f5;
          border-radius: 2px;
          padding: 4px 10px;
          white-space: pre;
          text-wrap: wrap;
          text-align: left;
          .at {
            color: var(--el-color-primary);
            margin: 0 4px;
          }

          &.quote {
            line-height: 20px;
            margin-top: 2px;
            font-size: 12px;
          }
        }
        .pic {
          cursor: pointer;
          .img {
            height: 180px;
            border-radius: 4px;
            box-shadow: var(--el-box-shadow);
          }
          .icon-video {
            position: absolute;
            left: 50%;
            top: 50%;
            transform: translate(-50%, -50%);
            font-size: 30px;
            color: #ddd;
            z-index: 2;
          }
        }
        .businessCard {
          width: 240px;
          box-shadow: 0px 0px 3px rgba(0, 0, 0, 0.2);
          text-align: left;
          border-radius: 10px;
          overflow: hidden;
          cursor: pointer;
          .info {
            display: flex;
            align-items: center;
            padding: 15px 20px;
            background-color: #e6f3ff;
            .nickname {
              margin-left: 10px;
              text-wrap: nowrap;
              overflow: hidden;
              text-overflow: ellipsis;
              flex: 1;
              line-height: 2em;
            }
          }
          p {
            text-align: left;
            font-size: 12px;
            line-height: 30px;
            padding-left: 20px;
            color: #999;
          }
        }
        .file {
          border: 1px solid #ccc;
          display: flex;
          width: 300px;
          padding: 10px;
          text-align: left;
          border-radius: 4px;
          justify-content: center;
          align-items: center;
          cursor: pointer;
          .l {
            display: flex;
            flex-direction: column;
            flex: 1;
            width: 0;
            span {
              overflow: hidden;
              text-wrap: nowrap;
              text-overflow: ellipsis;
              line-height: 26px;
            }
          }
        }
        .chatRecords {
          border: 1px solid #ccc;
          padding: 10px;
          width: 240px;
          text-align: left;
          border-radius: 4px;
          .title {
            font-size: 14px;
            line-height: 30px;
            border-bottom: 1px solid #ccc;
            margin-bottom: 4px;
          }
          .chatItem {
            font-size: 12px;
            line-height: 20px;
            color: #999;
          }
        }
        .map {
          border: 1px solid #ccc;
          border-radius: 10px;
          padding: 10px;
          width: 240px;
          .locationName {
            font-size: 14px;
            line-height: 30px;
          }
          .locationDesc {
            color: #999;
            font-size: 12px;
          }
        }
      }
    }
    .icon {
      width: 20px;
      height: 20px;
      margin-top: 22px;
      --el-loading-spinner-size: 20px;
    }
    &.isMe {
      flex-direction: row-reverse;
      text-align: right;
      .checkbox {
        margin-left: 10px;
      }
    }
  }
  .centerToast {
    text-align: center;
    .toast {
      display: inline-block;
      font-size: 14px;
      line-height: 18px;
      padding: 10px 14px;
      border-radius: 4px;
      background-color: #f4f5f7;
      color: #aaa;
    }
  }

  &.quote {
    background-color: #f5f5f5;
    padding: 8px;
    border-radius: 4px;
    margin-top: 4px;
    display: inline-block;

    .chat {
      margin-right: 0;
      display: block;
      &.isMe {
        margin-left: 0;
      }
      .context {
        display: flex;
        margin: 0px;
        align-items: start;
        max-width: unset;
        .name {
          margin-bottom: 0;
          margin-right: 10px;
          font-size: 14px;
          line-height: 20px;
          &::after {
            content: ":";
          }
        }
        .message {
          flex: 1;
          .text {
            padding: 0;
            margin-bottom: 0;
            font-size: 14px;
            line-height: 20px;
          }
        }
      }
    }
  }
}
</style>
