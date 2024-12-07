<template>
  <div class="header">
    <div class="container">
      <Avatar
        :src="conversationStore.currentConversation?.faceURL"
        :type="
          conversationStore.currentConversation?.groupID ? 'group' : 'user'
        "
      />
      <div class="title">
        <p>{{ conversationStore.currentConversation?.showName }}</p>
        <span v-if="conversationStore.isCurrentGroupChat">
          <template v-if="groupStore.currentGroupInfo?.memberCount">
            {{ groupStore.currentGroupInfo?.memberCount }}人
          </template>
          &nbsp;
        </span>
      </div>
    </div>
    <div class="ctrl">
      <el-tooltip content="聊天记录" placement="bottom" effect="light">
        <div class="icon" @click="openMsgSearch">
          <img :src="searchImg" alt="" />
        </div>
      </el-tooltip>
      <el-tooltip
        v-if="
          conversationStore.currentConversation?.groupID &&
          groupStore.currentGroupInfo?.status !== 2
        "
        content="邀请"
        placement="bottom"
        effect="light"
      >
        <div
          class="icon"
          @click="
            inviteFriendInGroup(conversationStore.currentConversation.groupID)
          "
        >
          <img :src="inviteImg" alt="" />
        </div>
      </el-tooltip>
      <el-tooltip
        content="设置"
        placement="bottom"
        effect="light"
        v-if="groupStore.currentGroupInfo?.status !== 2"
      >
        <div class="icon" @click="settingDrawer = true">
          <img :src="settingImg" alt="" />
        </div>
      </el-tooltip>
    </div>
    <el-drawer
      v-model="settingDrawer"
      :showClose="false"
      :withHeader="false"
      class="drawer"
      :size="300"
      destroy-on-close
    >
      <div class="main">
        <div class="header">
          <span v-if="showGroupMemberDetailList">
            <el-icon
              @click="showGroupMemberDetailList = false"
              class="icon"
              :size="24"
            >
              <i-ep-arrowLeft />
            </el-icon>
            群成员
          </span>
          <span v-else> 设置 </span>
        </div>
        <div class="body">
          <SettingInChat v-show="!showGroupMemberDetailList" />
          <GroupMemberDetailList v-if="showGroupMemberDetailList" />
        </div>
      </div>
    </el-drawer>
  </div>
</template>
<script lang="ts" setup>
import { Avatar, MessageSearch } from "~/components";
import { useConversationStore, useGroupStore } from "~/stores";
import searchImg from "~/assets/icons/search.svg";
import inviteImg from "~/assets/icons/invite.svg";
import settingImg from "~/assets/icons/setting.svg";
import { SettingInChat, GroupMemberDetailList } from "~/components";
import { h, provide, ref, watch } from "vue";
import { inviteFriendInGroup } from "~/utils/imsdk";
import { ElMessageBox } from "element-plus";

const conversationStore = useConversationStore();
const groupStore = useGroupStore();

const settingDrawer = ref(false);
const showGroupMemberDetailList = ref(false);

watch(settingDrawer, () => {
  showGroupMemberDetailList.value = false;
});

provide("showGroupMoreMemberList", () => {
  showGroupMemberDetailList.value = true;
});

const openMsgSearch = () => {
  ElMessageBox({
    title: "聊天记录",
    showConfirmButton: false,
    customClass: "histroy-message-box",
    message: () => h(MessageSearch),
  });
};
</script>
<style lang="scss">
.histroy-message-box {
  width: 624px;
  padding: 0;
  .el-message-box__header {
    padding: 0;
    .el-message-box__title {
      padding: 12px;
    }
    position: relative;
    &::after {
      content: "";
      position: absolute;
      width: 100%;
      height: 1px;
      background-color: var(--el-border-color);
      bottom: 0;
    }
  }
  .el-message-box__message {
    width: 100%;
  }
}
</style>
<style lang="scss" scoped>
.header {
  position: relative;
  .container {
    display: flex;
    align-items: center;
    padding-left: 15px;
    flex: 1;
    .title {
      display: flex;
      flex-direction: column;
      justify-content: space-around;
      margin-left: 15px;
      height: 40px;
      flex: 1;
      width: 0;

      p {
        line-height: 20px;
        font-size: 14px;
        overflow: hidden;
        text-wrap: nowrap;
        text-overflow: ellipsis;
      }
      span {
        color: #999;
        font-size: 12px;
      }
    }
  }
  .ctrl {
    display: flex;
    height: 100%;
    align-items: end;
    padding-bottom: 10px;
    .icon {
      padding: 4px;
      border-radius: 4px;
      margin-right: 2px;
      cursor: pointer;
      &:hover {
        background-color: #e9e9e9;
      }
    }
  }

  :deep(.drawer) {
    .el-drawer__body {
      padding: 0;
    }
    .main {
      height: 100%;
      display: flex;
      flex-direction: column;
      .header {
        height: 64px;
        border-bottom: 1px solid var(--el-border-color);
        font-size: 18px;
        line-height: 64px;
        span {
          display: flex;
          align-items: center;
          padding-left: 28px;
          .icon {
            margin-right: 10px;
            cursor: pointer;
          }
        }
      }
      .body {
        flex: 1;
        background-color: #f7f8f9;
        height: 0;
      }
    }
  }
}
</style>
