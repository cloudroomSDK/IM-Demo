<template>
  <div v-loading="!friendInfo" class="container">
    <template v-if="friendInfo">
      <div class="box">
        <div class="header">
          <Avatar :size="60" :src="friendInfo.faceURL" />
          <span class="name">{{ friendInfo.nickname }}</span>
        </div>
        <el-divider />
        <template v-if="conversationStore.currentConversation">
          <div class="card">
            <InputItem
              type="btn"
              text="详细资料"
              showArrow
              @click="
                openUserInfo(conversationStore.currentConversation.userID)
              "
            />
          </div>
          <MemberSetting
            :key="conversationStore.currentConversation.conversationID"
            :conversationInfo="conversationStore.currentConversation"
          />
        </template>
      </div>
      <div class="btn-groun">
        <el-button class="btn" type="danger" @click="del">
          解除好友关系
        </el-button>
        <el-button
          class="btn"
          type="primary"
          @click="
            conversationStore.gotoConversationChat({
              userID: friendInfo.userID,
            })
          "
        >
          聊天
        </el-button>
      </div>
    </template>
  </div>
</template>

<script setup lang="ts">
import { Avatar, MemberSetting, InputItem } from "~/components";
import { useRoute, useRouter } from "vue-router";
import { IMSDK, IMTYPE } from "~/utils/imsdk";
import { openUserInfo } from "~/utils";
import { onBeforeUnmount, ref, watch } from "vue";
import { ElMessageBox } from "element-plus";
import { useConversationStore, useFriendStore } from "~/stores";

const $route = useRoute();
const $router = useRouter();
const conversationStore = useConversationStore();
const friendStore = useFriendStore();

let friendInfo = ref();
const userChange = async function (userID: string) {
  friendInfo.value = undefined;
  const userInfo = await friendStore.getFriendInfo(userID);
  if (!userInfo) {
    return $router.replace({ name: "friend" });
  }
  friendInfo.value = userInfo;

  const { data: conversationItem } = await IMSDK.getOneConversation({
    sourceID: userID,
    sessionType: 1,
  });
  conversationStore.changeConversation(conversationItem);
};
userChange($route.params.userID as string);
watch(() => $route.params.userID as string, userChange);

const del = async function () {
  await ElMessageBox.confirm("确认解除好友关系吗?", "解除好友", {
    confirmButtonText: "确定",
    cancelButtonText: "取消",
    type: "warning",
  });
  await IMSDK.deleteFriend($route.params.userID as string);
};

const OnFriendDeleted = ({ data }: { data: IMTYPE.FriendUserItem }) => {
  if (data.userID === conversationStore.currentConversation?.userID) {
    $router.replace({ name: "friend" });
  }
};

IMSDK.on("OnFriendDeleted", OnFriendDeleted);

onBeforeUnmount(() => {
  IMSDK.off("OnFriendDeleted", OnFriendDeleted);
});
</script>

<style lang="scss" scoped>
.container {
  padding: 20px;
  flex-direction: column;
  .box {
    width: 440px;
    margin: 0 auto;
    height: 50%;
    .header {
      display: flex;
      align-items: center;
      padding: 0 36px;
      .name {
        margin-left: 20px;
        width: 0;
        flex: 1;
      }
    }
  }
  .btn-groun {
    flex: 1;
    text-align: center;
    .btn {
      width: 92px;
      height: 32px;
      // padding: 0;
    }
  }
}
</style>
