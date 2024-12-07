<template>
  <div>
    <div class="card">
      <InputItem
        text="群聊名称"
        :value="groupStore.currentGroupInfo?.groupName"
        :canEdit="groupStore.isCurrentGroupAdmin"
        @updateValue="updateGroupName"
      />
      <InputItem
        text="群聊ID"
        :value="groupStore.currentGroupInfo?.groupID!"
        canCopy
      />
    </div>
    <div class="card">
      <InputItem
        type="switch"
        text="聊天置顶"
        v-model="conversationStore.currentConversation!.isPinned"
        @change="changePinned"
      />
      <InputItem
        type="switch"
        text="消息免打扰"
        v-model="MsgDisturbing"
        @change="changeRecvMsgOpt"
      />
      <InputItem
        v-if="groupStore.isCurrentGroupAdmin"
        type="switch"
        text="全体禁言"
        v-model="allBannedToPost"
        @change="changeGroupStatus"
      />
      <InputItem
        type="btn"
        text="清空聊天记录"
        showArrow
        @click="clearHistory"
      />
    </div>
    <div class="card" v-if="groupStore.isCurrentGroupAdmin">
      <InputItem
        type="select"
        text="群聊验证"
        v-model="groupStore.currentGroupInfo!.needVerification"
        :options="needVerificationList"
        :selectWidth="200"
        @change="changeNeedVerification"
      />
      <InputItem
        type="switch"
        text="不允许查看其他群成员资料"
        v-model="lookMemberInfo"
        @change="changeLookMemberInfo"
      />
      <InputItem
        type="switch"
        text="不允许添加群成员为好友"
        v-model="applyMemberFriend"
        @change="changeApplyMemberFriend"
      />
    </div>
    <div class="card" v-if="groupStore.currentMemberInGroup?.roleLevel === 100">
      <InputItem type="btn" text="转让群主" showArrow @click="transferOwner" />
    </div>
  </div>
</template>
<script lang="ts" setup>
import { clearConversationMsg, IMSDK } from "~/utils/imsdk";
import { InputItem, MemberSelect } from ".";
import { ElMessageBox } from "element-plus";
import { useAppStore, useGroupStore, useConversationStore } from "~/stores";
import { computed, ref } from "vue";

const groupStore = useGroupStore();
const conversationStore = useConversationStore();
const appStore = useAppStore();

const MsgDisturbing = computed(
  () => conversationStore.currentConversation?.recvMsgOpt === 2
);
const allBannedToPost = computed(
  () => groupStore.currentGroupInfo?.status === 3
);

const needVerificationList = ref([
  {
    text: "群成员邀请无需验证",
    value: 0,
  },
  {
    text: "需要发送验证消息",
    value: 1,
  },
  {
    text: "允许所有人加群",
    value: 2,
  },
]);

const changeNeedVerification = (val: number) => {
  IMSDK.setGrpInfo({
    groupID: groupStore.currentGroupInfo!.groupID,
    needVerification: val,
  });
};
const lookMemberInfo = computed(() =>
  Boolean(groupStore.currentGroupInfo!.lookMemberInfo)
);
const changeLookMemberInfo = (val: number) => {
  IMSDK.setGrpInfo({
    groupID: groupStore.currentGroupInfo!.groupID,
    lookMemberInfo: Number(val),
  });
};

const applyMemberFriend = computed(() =>
  Boolean(groupStore.currentGroupInfo!.applyMemberFriend)
);
const changeApplyMemberFriend = (val: number) => {
  IMSDK.setGrpInfo({
    groupID: groupStore.currentGroupInfo!.groupID,
    applyMemberFriend: Number(val),
  });
};

const updateGroupName = (name: string) => {
  IMSDK.setGrpInfo({
    groupID: groupStore.currentGroupInfo?.groupID!,
    groupName: name,
  });
};

const changePinned = (val: boolean) => {
  IMSDK.pinConversation({
    conversationID: conversationStore.currentConversation!.conversationID,
    isPinned: val,
  });
};
const changeRecvMsgOpt = (val: boolean) => {
  IMSDK.setConversationRecvMsgOpt({
    conversationID: conversationStore.currentConversation!.conversationID,
    opt: val ? 2 : 0,
  });
};
const changeGroupStatus = (val: boolean) => {
  IMSDK.changeGrpMute({
    groupID: groupStore.currentGroupInfo?.groupID!,
    isMute: val,
  });
};
const clearHistory = async () => {
  await ElMessageBox.confirm(
    `确认删除群聊中 ${
      conversationStore.currentConversation!.showName
    } 的聊天记录吗?`,
    "清空聊天记录",
    {
      confirmButtonText: "确定",
      cancelButtonText: "取消",
      type: "warning",
    }
  );

  clearConversationMsg(conversationStore.currentConversation!.conversationID);
};

const transferOwner = async () => {
  const data = await appStore.showDialog<
    {
      userID?: string;
      imgSrc: string;
      showName: string;
      groupID?: string;
    }[]
  >({
    component: MemberSelect,
    title: "转让群主",
    data: {
      type: "groupMember",
      groupID: groupStore.currentGroupInfo?.groupID!,
      multiple: false,
    },
    width: 700,
  });
  IMSDK.transferGrpOwner({
    groupID: groupStore.currentGroupInfo?.groupID!,
    newOwnerUserID: data[0].userID!,
  });
};
</script>
<style lang="scss" scoped>
.card {
  margin-top: 8px;
  background-color: #fff;
}
</style>
