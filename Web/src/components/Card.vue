<template>
  <div class="container" :class="{ active: active }">
    <Avatar
      class="avatar"
      :size="40"
      :src="imgSrc"
      :type="avatarType"
    />
    <div class="fr" v-if="conversationItem">
      <div class="first">
        <span class="name">{{ name }}</span>
        <span class="time desc" v-if="conversationItem.latestMsgSendTime">
          {{ formatListString(conversationItem.latestMsgSendTime) }}
        </span>
      </div>
      <div class="second">
        <span v-if="conversationItem.latestMsg" class="desc">
          {{ toLastMessage(JSON.parse(conversationItem.latestMsg)) }}
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
withDefaults(defineProps<Props>(), {
  imgSrc: "",
  active: false,
  conversationItem: null,
  avatarType: "user",
});

import { IMTYPE } from "~/utils/imsdk";
import Avatar from "./Avatar.vue";
import { toLastMessage } from "~/utils/index";
import { formatListString } from "~/utils/dayjs";
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
