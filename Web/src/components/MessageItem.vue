<template>
  <div
    class="item"
    :class="{
      quote: isQuote,
      isMe: !leftLayout && source.sendID === userStore.getMyUserID,
    }"
  >
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
      <div class="chat">
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
              groupStore.currentGroupInfo,
              groupStore.isCurrentGroupAdmin
            )
          "
        />
        <div class="context" :style="{ flex: leftLayout ? 1 : 'none' }">
          <span class="name">
            {{ source.senderNickname }}
            <span v-if="leftLayout" class="sendTime">
              {{ formatChatString(source.sendTime) }}
            </span>
          </span>
          <span v-if="isQuote" class="maohao">:&nbsp;</span>
          <div class="message">
            <div class="content">
              <component
                :is="disabledRightClick ? 'div' : 'el-dropdown'"
                ref="dropdownRef"
                trigger="contextmenu"
                :placement="
                  !leftLayout && source.sendID === userStore.getMyUserID
                    ? 'left'
                    : 'right'
                "
                @visible-change="visibleChange($event, $refs.dropdownRef)"
              >
                <template #dropdown>
                  <el-dropdown-menu>
                    <el-dropdown-item
                      v-for="item in rightMenuList"
                      @click="item.fn"
                    >
                      {{ item.text }}
                    </el-dropdown-item>
                  </el-dropdown-menu>
                </template>
                <!-- 普通文本消息 -->
                <div
                  v-if="source.contentType === 101 && source?.textElem?.content"
                >
                  <span class="text">
                    <TextMsgRender :arr="chatTextSplit(source)" />
                  </span>
                </div>
                <!-- 图片 -->
                <div v-else-if="source.contentType === 102" class="pic">
                  <el-image
                    class="img"
                    :src="source.pictureElem.sourcePicture.url"
                    :previewSrcList="[source.pictureElem.sourcePicture.url]"
                    :z-index="9999"
                    loading="lazy"
                  />
                </div>
                <p v-else-if="source.contentType === 103">
                  <MessageAudioView
                    :data="source.soundElem"
                    :reverse="
                      !leftLayout && source.sendID === userStore.getMyUserID
                    "
                  />
                </p>
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
                    <TextMsgRender :arr="chatTextSplit(source)" />
                  </span>
                </div>
                <!-- 聊天记录 -->
                <div
                  v-else-if="source.contentType === 107"
                  class="chatRecords"
                  @click="openChatRecord"
                >
                  <p class="title">{{ source.mergeElem.title }}</p>
                  <div
                    class="chatItem"
                    v-for="item in source.mergeElem.abstractList"
                  >
                    <TextMsgRender :arr="chatTextSplit(item)" />
                  </div>
                </div>
                <!-- 引用消息 -->
                <div v-else-if="source.contentType === 114">
                  <div>
                    <span class="text">
                      <TextMsgRender :arr="chatTextSplit(source)" />
                    </span>
                  </div>
                  <MessageItem
                    :source="source.quoteElem.quoteMessage"
                    isQuote
                    disabledRightClick
                  />
                </div>
                <!-- 名片 -->
                <div
                  v-else-if="source.contentType === 108"
                  class="businessCard"
                  @click="openUserInfo(source.cardElem.userID)"
                >
                  <div class="info">
                    <Avatar :src="source.cardElem.faceURL" />
                    <span class="nickname">{{ source.cardElem.nickname }}</span>
                  </div>
                  <p>个人名片</p>
                </div>
                <!-- 定位 -->
                <div
                  v-else-if="source.contentType === 109"
                  class="map"
                  @click="openMap"
                >
                  <p class="locationName">{{ data.name }}</p>
                  <p class="locationDesc">{{ data.addr }}</p>
                  <MapContainer
                    style="height: 120px"
                    :title="data.name"
                    :LngLat="[data.longitude, data.latitude]"
                    simple
                  />
                </div>
                <p v-else>[暂不支持该内容]</p>
              </component>
            </div>
          </div>
        </div>
        <template v-if="!isQuote">
          <div v-if="source.status === 1" class="icon" v-loading="true" />
          <div
            v-else-if="source.status === 3"
            class="icon"
            style="cursor: pointer"
            @click="reSendMsg"
          >
            <el-icon color="#f00" size="20px">
              <i-ep-warningFilled />
            </el-icon>
          </div>
          <div v-if="attachedTime >= 0" class="attachedTime">
            {{ attachedTime }}s
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
    <div class="centerToast" v-else-if="source.contentType === 1510">
      <span class="toast">
        <Nickname
          :nickname="data.entrantUser.nickname"
          :userID="data.entrantUser.userID"
        />
        加入了群聊
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
        禁言<!-- {{ ~~(data.mutedSeconds / 60) }}分{{ data.mutedSeconds % 60 }}秒 -->
      </span>
    </div>
    <div class="centerToast" v-else-if="source.contentType === 1513">
      <span class="toast">
        <Nickname
          :nickname="data.opUser.nickname"
          :userID="data.opUser.userID"
        />
        取消了
        <Nickname
          :nickname="data.mutedUser.nickname"
          :userID="data.mutedUser.userID"
        />
        的禁言
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
      <span v-if="isQuote">引用消息已撤回</span>
      <span v-else class="toast">
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
import {
  computed,
  h,
  onBeforeMount,
  onBeforeUnmount,
  ref,
  inject,
  Ref,
  watch,
} from "vue";
import { getPrivateExpirationMsgTime, IMSDK, IMTYPE } from "~/utils/imsdk";
import {
  PicPreview,
  Avatar,
  MemberSelect,
  TextMsgRender,
  Nickname,
  MessageAudioView,
  MessageItem,
  MessageRecord,
  MapContainer,
} from "~/components";
import { formatChatString } from "~/utils/dayjs";
import {
  downloadUrl,
  openUserInfo,
  formatFileSize,
  chatTextSplit,
  errorHandle,
} from "~/utils";
import {
  useAppStore,
  useConversationStore,
  useGroupStore,
  useUserStore,
} from "~/stores";
import { getSvrTime } from "~/api/login";

const appStore = useAppStore();
const userStore = useUserStore();
const conversationStore = useConversationStore();
const groupStore = useGroupStore();

type MsgItem = IMTYPE.MessageItem & {
  isShowTime?: boolean;
};
const props = defineProps<{
  source: MsgItem;
  isQuote?: boolean;
  leftLayout?: boolean;
  disabledRightClick?: boolean;
}>();

let attachedTimerId: NodeJS.Timeout, revokeTimerId: NodeJS.Timeout;
const attachedTime = ref();
const dropdownVisibleChange = props.disabledRightClick
  ? undefined
  : (inject("dropdownVisibleChange") as (e: boolean, ref: any) => void);
const dropdownRef = ref();
const rightMenuList = ref<any[]>([]); //右键菜单列表

// 阅后即焚处理
const attachedHandle = async () => {
  const msg = props.source;
  if (!msg.attachedInfoElem?.isPrivateChat) return;
  if (msg.isRead || msg.sendID !== userStore.getMyUserID) {
    const time = await getPrivateExpirationMsgTime(
      conversationStore.currentConversation?.conversationID!,
      msg
    );
    if (time > 0) {
      attachedTime.value = Math.ceil(time / 1000);
      clearInterval(attachedTimerId);
      attachedTimerId = setInterval(() => {
        attachedTime.value--;
        if (attachedTime.value < 0) {
          clearInterval(attachedTimerId);
        }
      }, 1e3);
    }
  }
};

watch(
  () => [props.source?.isRead, props.source?.attachedInfoElem?.hasReadTime],
  () => {
    attachedHandle();
  }
);

onBeforeMount(() => {
  attachedHandle();
});
onBeforeUnmount(() => {
  clearInterval(attachedTimerId);
  clearTimeout(revokeTimerId);
});

const data = computed(() => {
  if (props.isQuote && props.source.contentType === 2101) {
    return null;
  }
  if (
    [
      1501, 1504, 1507, 1508, 1509, 1510, 1511, 1512, 1513, 1514, 1515, 1520,
      1701, 2101,
    ].includes(props.source.contentType)
  ) {
    return JSON.parse(props.source.notificationElem.detail);
  }

  if (props.source.contentType === 109) {
    try {
      const locationElem = props.source.locationElem;
      const description = JSON.parse(locationElem.description);
      return {
        name: description.name,
        addr: description.addr,
        latitude: locationElem.latitude,
        longitude: locationElem.longitude,
      };
    } catch (error) {
      return {};
    }
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

const openChatRecord = () => {
  ElMessageBox({
    title: props.source.mergeElem.title,
    message: h(MessageRecord, {
      list: props.source.mergeElem.multiMessage,
    }),
    customClass: "histroy-message-box",
    showClose: true,
    showConfirmButton: false,
  });
};

const openMap = () => {
  ElMessageBox({
    message: h(MapContainer, {
      title: data.value.name,
      LngLat: [data.value.longitude, data.value.latitude],
    }),
    showClose: true,
    customClass: "map-container",
    showConfirmButton: false,
  });
};

// 创建右键菜单
const visibleChange = async (isOpen: boolean, ref: Ref) => {
  dropdownVisibleChange!(isOpen, ref);
  if (!isOpen) return;
  rightMenuList.value = [];
  // 语音消息不让转发
  if (props.source.contentType !== 103) {
    rightMenuList.value.push({
      text: "转发",
      fn: async () => {
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

        const { data: message } = await IMSDK.createForwardMsg(props.source);
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
      },
    });
  }
  rightMenuList.value.push({
    text: "多选",
    fn: () => {
      conversationStore.multipleSelect = [props.source.clientMsgID];
      conversationStore.multipleStatus = true;
    },
  });
  rightMenuList.value.push({
    text: "回复",
    fn: () => {
      conversationStore.currentQuoteMessage = props.source;
    },
  });
  rightMenuList.value.push({
    text: "删除",
    fn: async () => {
      await IMSDK.deleteMsg({
        conversationID: conversationStore.currentConversation!.conversationID,
        clientMsgID: props.source.clientMsgID,
      });
      conversationStore.removeList([props.source.clientMsgID]);
    },
  });
  let flag = false;

  // 管理员允许随时撤回消息
  if (groupStore.isCurrentGroupAdmin) {
    if (props.source.sendID === userStore.getMyUserID) {
      flag = true;
    } else {
      // 找到该消息所属的成员，如果该成员的级别低于我，则允许我撤回
      groupStore.currentGroupMemberList?.some((item) => {
        if (item.userID === props.source.sendID) {
          if (groupStore.currentMemberInGroup!.roleLevel > item.roleLevel) {
            flag = true;
          }
          return true;
        }
      });
    }
  } else if (props.source.sendID === userStore.getMyUserID) {
    // 消息在发出两分钟内允许撤回
    if (props.source.sendTime + 120e3 - (await getSvrTime()) > 0) {
      flag = true;
    }
  }

  if (flag) {
    rightMenuList.value.push({
      text: "撤回",
      fn: () => {
        IMSDK.revokeMsg({
          conversationID: conversationStore.currentConversation!.conversationID,
          clientMsgID: props.source.clientMsgID,
        });
      },
    });
  }
};

const reSendMsg = async () => {
  try {
    props.source.status = 1;
    const { data: successMessage } = await IMSDK.sendMsg({
      recvID: conversationStore.isCurrentGroupChat
        ? ""
        : conversationStore.currentConversation!.userID,
      groupID: conversationStore.isCurrentGroupChat
        ? conversationStore.currentConversation!.groupID
        : "",
      message: props.source,
      // offlinePushInfo?: OfflinePush;
    });

    console.log(successMessage);
    conversationStore.updateMsgList([successMessage]);
  } catch (error) {
    errorHandle(error as IMTYPE.WsResponse);
    props.source.status = 3;
    conversationStore.updateMsgList([props.source]);
  }
};
</script>

<style lang="scss">
.map-container {
  width: 90vw;
  height: 80vh;
  max-width: unset;
  padding: 0;
  .el-message-box__message {
    width: 90vw;
    height: 80vh;
  }
}
</style>

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
      flex: none;
      margin-top: 4px;
      cursor: pointer;
    }
    .context {
      margin: 0 12px;
      display: inline-block;
      max-width: 80%;
      .sendTime {
        float: right;
      }
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
          cursor: pointer;

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
          width: 260px;
          overflow: hidden;
          cursor: pointer;

          .locationName {
            font-size: 14px;
            line-height: 20px;

            padding: 10px;
            white-space: nowrap;
            overflow: hidden;
            text-overflow: ellipsis;
          }
          .locationDesc {
            color: #999;
            font-size: 12px;
            padding: 10px;
            white-space: nowrap;
            overflow: hidden;
            text-overflow: ellipsis;
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
    .attachedTime {
      color: var(--el-color-primary);
      font-size: 12px;
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

  &.isMe {
    .quote {
      transform-origin: right top;
    }
    .chat {
      flex-direction: row-reverse;
      text-align: right;
      margin-left: 0;
      .checkbox {
        margin-left: 10px;
      }
    }
  }

  &.quote {
    background-color: #f5f5f5;
    padding: 8px;
    border-radius: 4px;
    margin-top: 4px;
    display: inline-block;
    transform: scale(0.8);
    transform-origin: left top;
    margin-bottom: -20px;

    .chat {
      margin-right: 0;
      display: block;
      .context {
        display: flex;
        margin: 0px;
        align-items: start;
        max-width: unset;
        .name,
        .maohao {
          margin-bottom: 0;
          font-size: 14px;
          line-height: 20px;
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
