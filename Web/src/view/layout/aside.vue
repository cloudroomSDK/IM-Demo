<template>
  <div class="container">
    <Avatar
      :src="userStore.userInfo?.faceURL"
      @click="openUserInfo(userStore.userInfo!.userID)"
      style="cursor: pointer"
    />
    <div class="menu">
      <ul>
        <li
          @click="$router.push({ name: 'chat' })"
          :class="{ active: routeName === 'chat' }"
        >
          <div v-if="conversationStore.unreadMsgCount" class="count">
            {{
              conversationStore.unreadMsgCount > 99
                ? 99
                : conversationStore.unreadMsgCount
            }}
          </div>
          <i class="icon icon-message"></i>
          <p>消息</p>
        </li>
        <li
          @click="$router.push({ name: 'friend' })"
          :class="{
            active: routeName === 'friend',
            hot: friendStore.newFriendCount > 0,
          }"
        >
          <i class="icon icon-friend"></i>
          <p>好友</p>
        </li>
        <li
          @click="$router.push({ name: 'group' })"
          :class="{ active: routeName === 'group' }"
        >
          <i class="icon icon-group"></i>
          <p>群组</p>
        </li>
      </ul>
    </div>
    <div class="popover">
      <el-popover placement="right-start" :width="150" trigger="hover">
        <template #reference>
          <el-icon :size="26"><i-ep-setting /></el-icon>
        </template>
        <ul class="popover-ul">
          <!-- <li><div>个人设置</div></li>
          <li><div>清空聊天记录</div></li> -->
          <li><div @click="openBlackList">通讯录黑名单</div></li>
          <li><div @click="openVersionInfo">版本信息</div></li>
          <li><div @click="userStore.logout">退出登录</div></li>
        </ul>
      </el-popover>
    </div>
  </div>
</template>

<script setup lang="ts">
import { Avatar, VersionInfo, BlackList } from "~/components";
import {
  useUserStore,
  useFriendStore,
  useAppStore,
  useConversationStore,
} from "~/stores";
import { useRoute, useRouter } from "vue-router";
import { computed } from "vue";
import { openUserInfo } from "~/utils/index";

const $router = useRouter();
const $route = useRoute();
const appStore = useAppStore();
const userStore = useUserStore();
const friendStore = useFriendStore();
const conversationStore = useConversationStore();

const routeName = computed(() => $route.fullPath.split("/")[1]);

const openVersionInfo = () => {
  appStore.showDialog({
    component: VersionInfo,
    title: "版本信息",
    width: 470,
  });
};
const openBlackList = () => {
  appStore.showDialog({
    component: BlackList,
    title: "通讯录黑名单",
    width: 500,
  });
};
</script>

<style lang="scss" scoped>
.menu {
  .icon {
    display: block;
    width: 30px;
    height: 30px;
    background-repeat: no-repeat;
    background-size: cover;
    background-position: center;

    &.icon-message {
      background-image: url("~/assets/icons/message.svg");
    }
    &.icon-friend {
      background-image: url("~/assets/icons/friend.svg");
    }
    &.icon-group {
      background-image: url("~/assets/icons/group.svg");
    }
  }
  .active .icon {
    &.icon-message {
      background-image: url("~/assets/icons/message_active.svg");
    }
    &.icon-friend {
      background-image: url("~/assets/icons/friend_active.svg");
    }
    &.icon-group {
      background-image: url("~/assets/icons/group_active.svg");
    }
  }
}
.container {
  display: flex;
  flex-direction: column;
  align-items: center;
  padding: 12px 0;
  position: relative;
  .menu {
    margin-top: 80px;
    li {
      font-size: 12px;
      text-align: center;
      color: #666;
      margin-bottom: 40px;
      cursor: pointer;
      position: relative;
      p {
        margin-top: 4px;
      }
      &.active {
        color: var(--el-color-primary);
      }
      .count {
        position: absolute;
        width: 20px;
        height: 20px;
        background-color: #f00;
        right: -8px;
        top: -8px;
        border-radius: 50%;
        color: #fff;
        font-size: 12px;
        line-height: 20px;
        text-align: center;
        transform: scale(0.785);
      }
      &.hot::after {
        content: "";
        position: absolute;
        width: 8px;
        height: 8px;
        background-color: #f00;
        border-radius: 50%;
        right: -2px;
        top: -2px;
      }
    }
  }
  .popover {
    position: absolute;
    bottom: 30px;
    cursor: pointer;
  }
}
.popover-ul {
  div {
    font-size: 14px;
    height: 40px;
    line-height: 40px;
    padding: 0 18px;
    border-radius: 4px;
    cursor: pointer;
    &:hover {
      background-color: #e9ebf4;
    }
  }
}
</style>
