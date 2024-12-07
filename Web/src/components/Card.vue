<template>
  <div
    class="container"
    :style="{
      backgroundColor: bgc,
    }"
  >
    <Avatar class="avatar" :size="40" :src="imgSrc" :type="avatarType" />
    <div class="fr" v-if="conversationItem">
      <div class="first">
        <span class="name">{{ name }}</span>
        <span class="time desc" v-if="conversationItem.latestMsgSendTime">
          {{ formatListString(conversationItem.latestMsgSendTime) }}
        </span>
      </div>
      <div class="second">
        <span v-if="conversationItem.draftText" class="desc">
          <span style="color: #ff1210; margin-right: 2px"> [草稿] </span>
          <TextMsgRender :arr="chatTextSplit(conversationItem.draftText)" />
        </span>
        <span v-else-if="latestMsg" class="desc">
          <template v-if="[106, 101, 114].indexOf(latestMsg.contentType) > -1">
            <TextMsgRender :arr="chatTextSplit(latestMsg)" />
          </template>
          <template v-else>{{ toLastMessage(latestMsg) }}</template>
        </span>
        <i class="msgCount" v-if="conversationItem?.unreadCount">
          <span>{{
            conversationItem?.unreadCount > 99
              ? "99+"
              : conversationItem?.unreadCount
          }}</span>
        </i>
      </div>
    </div>
    <div class="name" v-else>
      {{ name }}
    </div>
    <i class="msgCount" v-if="msgCount">
      <span>{{ msgCount > 99 ? "99+" : msgCount }}</span>
    </i>
  </div>
</template>

<script lang="ts" setup>
interface Props {
  name: string;
  imgSrc?: string;
  active?: boolean;
  msgCount?: number;
  conversationItem?: IMTYPE.ConversationItem | null;
  avatarType?: "user" | "group";
}
const props = withDefaults(defineProps<Props>(), {
  imgSrc: "",
  active: false,
  conversationItem: null,
  avatarType: "user",
});

import { IMTYPE } from "~/utils/imsdk";
import Avatar from "./Avatar.vue";
import { toLastMessage } from "~/utils/index";
import { formatListString } from "~/utils/dayjs";
import { computed } from "vue";
import { chatTextSplit } from "~/utils/index";
import { TextMsgRender } from "~/components";

const bgc = computed(() => {
  if (props.active) {
    return "#e9ebf4";
  }
  if (props.conversationItem?.isPinned) {
    return "#efefef";
  }
  return "";
});

const latestMsg = computed<IMTYPE.MessageItem>(() => {
  if (props.conversationItem?.latestMsg) {
    return JSON.parse(props.conversationItem.latestMsg);
  }
});
</script>

<style lang="scss" scoped>
.active {
  background-color: #e9ebf4;
}
.desc {
  color: #999;
  font-size: 12px;
}
.container {
  display: flex;
  align-items: center;
  padding: 10px;
  border-radius: 4px;
  overflow: hidden;
  cursor: pointer;
  position: relative;
  margin-bottom: 2px;
  text-wrap: nowrap;
  width: 100%;
  &:hover {
    @extend .active;
  }
  .avatar {
    margin-right: 16px;
  }
  .name {
    flex: 1;
    font-size: 14px;
    overflow: hidden;
    text-overflow: ellipsis;
    line-height: 20px;
  }
  .fr {
    width: 0;
    flex: 1;
    .first {
      display: flex;
      line-height: 20px;
      justify-content: space-between;
      .time {
        margin-left: 2px;
      }
    }
    .second {
      line-height: 20px;
      display: flex;
      justify-content: space-between;
      align-items: center;

      .desc {
        flex: 1;
        overflow: hidden;
        text-overflow: ellipsis;
      }
    }
  }

  .msgCount {
    width: 16px;
    height: 16px;
    background-color: #f00;
    border-radius: 50%;
    color: #fff;
    font-size: 12px;
    line-height: 16px;
    text-align: center;
    margin-left: 2px;
  }
}
</style>
