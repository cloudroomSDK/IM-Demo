<template>
  <div>
    <div class="card">
      <InputItem
        text="群聊名称"
        :value="groupInfo.groupName"
        :canEdit="
          myMemberInfo.roleLevel === 100 || myMemberInfo.roleLevel === 60
        "
        @updateValue="updateGroupName"
      />
      <InputItem text="群聊ID" :value="groupInfo.groupID" canCopy />
    </div>
    <div class="card">
      <InputItem
        type="switch"
        text="置顶"
        :value="conversationInfo.isPinned"
        @change="changePinned"
      />
      <InputItem
        type="switch"
        text="消息免打扰"
        :value="conversationInfo.recvMsgOpt === 2"
        @change="changeRecvMsgOpt"
      />
      <InputItem
        v-if="isManager"
        type="switch"
        text="全体禁言"
        :value="groupInfo.status === 3"
        @change="changeGroupStatus"
      />
      <InputItem
        type="btn"
        text="清空聊天记录"
        showArrow
        @click="clearHistory"
      />
    </div>
  </div>
</template>
<script lang="ts" setup>
import { clearConversationMsg, IMSDK, IMTYPE } from "~/utils/imsdk";
import { InputItem } from ".";
import { ElMessageBox } from "element-plus";
import { computed } from "vue";

const props = defineProps<{
  groupInfo: IMTYPE.GroupItem;
  conversationInfo: IMTYPE.ConversationItem;
  myMemberInfo: IMTYPE.GroupMemberItem;
}>();

const isManager = computed(
  () =>
    props.myMemberInfo.roleLevel === 100 || props.myMemberInfo.roleLevel === 60
);
const updateGroupName = (name: string) => {
  IMSDK.setGrpInfo({
    groupID: props.groupInfo.groupID,
    groupName: name,
  });
};

const changePinned = (val: boolean) => {
  IMSDK.pinConversation({
    conversationID: props.conversationInfo.conversationID,
    isPinned: val,
  });
};
const changeRecvMsgOpt = (val: boolean) => {
  IMSDK.setConversationRecvMsgOpt({
    conversationID: props.conversationInfo.conversationID,
    opt: val ? 2 : 0,
  });
};
const changeGroupStatus = (val: boolean) => {
  IMSDK.changeGrpMute({
    groupID: props.groupInfo.groupID,
    isMute: val,
  });
};
const clearHistory = async () => {
  await ElMessageBox.confirm(
    `确认删除群聊中 ${props.conversationInfo.showName} 的聊天记录吗?`,
    "清空聊天记录",
    {
      confirmButtonText: "确定",
      cancelButtonText: "取消",
      type: "warning",
    }
  );

  clearConversationMsg(props.conversationInfo.conversationID);
};
</script>
<style lang="scss" scoped>
.card {
  margin-top: 8px;
  background-color: #fff;
}
</style>
