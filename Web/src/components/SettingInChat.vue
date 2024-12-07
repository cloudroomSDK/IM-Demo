<template>
  <div class="body1">
    <el-scrollbar class="scrollbar">
      <template v-if="conversationStore.isCurrentGroupChat">
        <GroupMemberList />
        <GroupSetting />
      </template>
      <template v-else-if="conversationStore.currentConversation">
        <div class="card">
          <InputItem
            type="btn"
            :iconUrl="conversationStore.currentConversation.faceURL"
            :text="conversationStore.currentConversation.showName"
            :height="56"
            showArrow
            @click="openUserInfo(conversationStore.currentConversation.userID)"
          />
        </div>
        <MemberSetting
          :conversationInfo="conversationStore.currentConversation"
        />
      </template>
    </el-scrollbar>
    <div class="btn-group">
      <template
        v-if="
          conversationStore.isCurrentGroupChat &&
          groupStore.currentMemberInGroup
        "
      >
        <el-button
          type="danger"
          @click="dissolutionHandle"
          v-if="groupStore.currentMemberInGroup.roleLevel === 100"
        >
          解散群组
        </el-button>
        <el-button type="danger" @click="exitHandle" v-else>退出群组</el-button>
      </template>
      <template v-if="!conversationStore.isCurrentGroupChat">
        <el-button type="danger" @click="del" v-if="isFriend">
          解除好友
        </el-button>
        <el-button type="primary" @click="add" v-else>添加好友</el-button>
      </template>
    </div>
  </div>
</template>
<script lang="ts" setup>
import {
  AddFriend,
  GroupMemberList,
  GroupSetting,
  MemberSetting,
  InputItem,
} from ".";
import { useConversationStore, useFriendStore, useGroupStore } from "~/stores";
import { IMSDK } from "~/utils/imsdk";
import { ElMessageBox } from "element-plus";
import { computed, ComputedRef, h } from "vue";
import { openUserInfo } from "~/utils";
const conversationStore = useConversationStore();
const groupStore = useGroupStore();

let isFriend: ComputedRef<boolean>;
const friendStore = useFriendStore();

if (conversationStore.currentConversation?.userID) {
  friendStore.getList();
  isFriend = computed(() => {
    return (
      !!friendStore.list &&
      friendStore.list.findIndex(
        (item) => item.userID === conversationStore.currentConversation?.userID
      ) > -1
    );
  });
}

const dissolutionHandle = async () => {
  await ElMessageBox.confirm(
    `确认要解散群组"${conversationStore.currentConversation!.showName}"吗?`,
    "解散群组",
    {
      confirmButtonText: "确定",
      cancelButtonText: "取消",
      type: "warning",
    }
  );

  await IMSDK.dismissGrp(conversationStore.currentConversation!.groupID);
};
const exitHandle = async () => {
  await ElMessageBox.confirm(
    `确认要退出群组"${conversationStore.currentConversation!.showName}"吗?`,
    "退出群组",
    {
      confirmButtonText: "确定",
      cancelButtonText: "取消",
      type: "warning",
    }
  );

  await IMSDK.quitGrp(conversationStore.currentConversation!.groupID);
};

const del = async function () {
  await ElMessageBox.confirm("确认解除好友关系吗?", "解除好友", {
    confirmButtonText: "确定",
    cancelButtonText: "取消",
    type: "warning",
  });
  await IMSDK.deleteFriend(conversationStore.currentConversation?.userID!);
};

const add = () => {
  let str = "";
  ElMessageBox({
    title: "好友验证",
    message: h(AddFriend, {
      nickname: conversationStore.currentConversation?.showName || "",
      userID: conversationStore.currentConversation?.userID || "",
      imgSrc: conversationStore.currentConversation?.faceURL || "",
      modelVale: str,
      onUpdateValue(newStr: string) {
        str = newStr;
      },
    }),
    showClose: true,
    confirmButtonText: "发送消息",
    beforeClose: async (action, instance, done) => {
      if (action === "confirm") {
        instance.confirmButtonLoading = true;
        instance.confirmButtonText = "Loading...";
        try {
          await IMSDK.addFriend({
            toUserID: conversationStore.currentConversation?.userID!,
            reqMsg: str,
          });
        } catch (error) {}
        setTimeout(() => {
          instance.confirmButtonLoading = false;
        }, 300);
      }
      done();
    },
  }).then((action) => {
    console.log(action);
  });
};
</script>
<style lang="scss" scoped>
.body1 {
  height: 100%;
  display: flex;
  flex-direction: column;
  .scrollbar {
    flex: 1;
    height: 0;
    .card {
      margin-bottom: 8px;
      background-color: #fff;
    }
  }
  .btn-group {
    margin: 10px auto 20px;
  }
}
</style>
