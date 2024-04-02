<template>
  <div class="header">
    <div class="container">
      <Avatar :src="conversationStore.currentConversation?.faceURL"></Avatar>
      <div class="title">
        <p>{{ conversationStore.currentConversation?.showName }}</p>
        <span v-if="conversationStore.isCurrentGroupChat">
          <template v-if="conversationStore.currentGroupInfo?.memberCount">
            {{ conversationStore.currentGroupInfo?.memberCount }}人
          </template>
          &nbsp;
        </span>
      </div>
    </div>
    <div class="ctrl">
      <div class="icon">
        <img :src="searchImg" alt="" />
      </div>
      <div
        class="icon"
        v-if="conversationStore.currentConversation?.groupID"
        @click="
          inviteFriendInGroup(conversationStore.currentConversation.groupID)
        "
      >
        <img :src="inviteImg" alt="" />
      </div>
      <div class="icon" @click="settingDrawer = true">
        <img :src="settingImg" alt="" />
      </div>
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
          <SettingInChat
            v-show="!showGroupMemberDetailList"
            :userID="conversationStore.currentConversation?.userID"
            :groupID="conversationStore.currentConversation?.groupID"
            :groupInfo="conversationStore?.currentGroupInfo"
            :conversationInfo="conversationStore.currentConversation"
            :myMemberInfo="conversationStore?.currentMemberInGroup"
          />

          <GroupMemberDetailList
            v-if="showGroupMemberDetailList"
            :groupID="conversationStore.currentConversation?.groupID"
            :myMemberInfo="conversationStore?.currentMemberInGroup"
          />
        </div>
      </div>
    </el-drawer>
  </div>
</template>
<script lang="ts" setup>
import { Avatar } from "~/components";
import { useConversationStore } from "~/stores";
import searchImg from "~/assets/icons/search.svg";
import inviteImg from "~/assets/icons/invite.svg";
import settingImg from "~/assets/icons/setting.svg";
import { SettingInChat, GroupMemberDetailList } from "~/components";
import { provide, ref, watch } from "vue";
import { inviteFriendInGroup } from "~/utils/imsdk";

const conversationStore = useConversationStore();

const settingDrawer = ref(false);
const showGroupMemberDetailList = ref(false);

watch(settingDrawer, () => {
  showGroupMemberDetailList.value = false;
});

provide("showGroupMoreMemberList", () => {
  showGroupMemberDetailList.value = true;
});
</script>
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
