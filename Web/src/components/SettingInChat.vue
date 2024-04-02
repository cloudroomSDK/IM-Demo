<template>
  <div class="body1">
    <el-scrollbar class="scrollbar">
      <template v-if="groupID && groupInfo && conversationInfo && myMemberInfo">
        <GroupMemberList :groupInfo="groupInfo" :myMemberInfo="myMemberInfo" />
        <GroupSetting
          :groupInfo="groupInfo"
          :conversationInfo="conversationInfo"
          :myMemberInfo="myMemberInfo"
        />
      </template>
      <template v-if="userID && conversationInfo">
        <div class="card">
          <InputItem
            type="btn"
            :iconUrl="conversationInfo.faceURL"
            :text="conversationInfo.showName"
            :height="56"
            showArrow
            @click="openUserInfo(conversationInfo.userID)"
          />
        </div>
        <MemberSetting :conversationInfo="conversationInfo" />
      </template>
    </el-scrollbar>
    <div class="btn-group">
      <template v-if="groupID && myMemberInfo">
        <el-button
          type="danger"
          @click="dissolutionHandle"
          v-if="myMemberInfo?.roleLevel === 100"
        >
          解散群组
        </el-button>
        <el-button type="danger" @click="exitHandle" v-else>退出群组</el-button>
      </template>
      <template v-if="userID">
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
import { useFriendStore } from "~/stores";
import { IMSDK, IMTYPE } from "~/utils/imsdk";
import { ElMessageBox } from "element-plus";
import { computed, ComputedRef, h, onBeforeMount } from "vue";
import { openUserInfo } from "~/utils";
const props = defineProps<{
  userID?: string;
  groupID?: string;
  conversationID?: string;
  groupInfo?: IMTYPE.GroupItem;
  conversationInfo?: IMTYPE.ConversationItem;
  myMemberInfo?: IMTYPE.GroupMemberItem;
}>();

let isFriend: ComputedRef<boolean>;
const friendStore = useFriendStore();

if (props.userID) {
  friendStore.getList();
  isFriend = computed(() => {
    return (
      !!friendStore.list &&
      friendStore.list.findIndex((item) => item.userID === props.userID) > -1
    );
  });
}

onBeforeMount(async () => {});

const dissolutionHandle = async () => {
  await ElMessageBox.confirm(
    `确认要解散群组"${props.conversationInfo!.showName}"吗?`,
    "解散群组",
    {
      confirmButtonText: "确定",
      cancelButtonText: "取消",
      type: "warning",
    }
  );

  await IMSDK.dismissGrp(props.groupID!);
};
const exitHandle = async () => {
  await ElMessageBox.confirm(
    `确认要退出群组"${props.conversationInfo!.showName}"吗?`,
    "退出群组",
    {
      confirmButtonText: "确定",
      cancelButtonText: "取消",
      type: "warning",
    }
  );

  await IMSDK.quitGrp(props.groupID!);
};

const del = async function () {
  await ElMessageBox.confirm("确认解除好友关系吗?", "解除好友", {
    confirmButtonText: "确定",
    cancelButtonText: "取消",
    type: "warning",
  });
  await IMSDK.deleteFriend(props.userID!);
};

const add = () => {
  let str = "";
  ElMessageBox({
    title: "好友验证",
    message: h(AddFriend, {
      nickname: props.conversationInfo?.showName,
      userID: props.userID,
      imgSrc: props.conversationInfo?.faceURL,
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
            toUserID: props.userID!,
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
