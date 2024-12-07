<template>
  <div
    class="wrap"
    v-if="groupStore.currentGroupInfo && groupStore.currentMemberInGroup"
  >
    <el-scrollbar class="scroll">
      <div class="w header">
        <CanEditAvatar
          :src="groupStore.currentGroupInfo?.faceURL"
          :size="60"
          :disabled="!groupStore.isCurrentGroupAdmin"
          type="group"
          @updateUrl="updateAvatar"
        />
        <div class="text">
          <div class="grounpName">
            群名称：{{ groupStore.currentGroupInfo?.groupName }}
          </div>
          <div class="grounpID">
            群ID：{{ groupStore.currentGroupInfo?.groupID }}
          </div>
        </div>
      </div>
      <el-divider />
      <div class="w memberList">
        <GroupMemberList
          v-if="
            groupStore.currentGroupMemberList && groupStore.currentMemberInGroup
          "
        />
      </div>
      <el-divider />
      <div class="w setting">
        <GroupSetting />
      </div>
    </el-scrollbar>
    <div class="btn-group" v-if="groupStore.currentMemberInGroup">
      <el-button
        type="danger"
        @click="dissolutionHandle"
        v-if="groupStore.currentMemberInGroup.roleLevel === 100"
      >
        解散群组
      </el-button>
      <el-button type="danger" @click="exitHandle" v-else> 退出群组 </el-button>
      <el-button
        type="primary"
        @click="
          conversationStore.gotoConversationChat({
            conversationItem: conversationStore.currentConversation,
          })
        "
      >
        聊天
      </el-button>
    </div>

    <el-drawer
      v-model="openMemberList"
      :showClose="false"
      :withHeader="false"
      class="drawer"
      :size="300"
      destroy-on-close
    >
      <div class="main">
        <div class="header">群成员</div>
        <div class="body">
          <GroupMemberDetailList />
        </div>
      </div>
    </el-drawer>
  </div>
</template>
<script lang="ts" setup>
import { onBeforeUnmount, provide, ref, watch } from "vue";
import { useRoute, useRouter } from "vue-router";
import { IMSDK, IMTYPE } from "~/utils/imsdk";
import {
  CanEditAvatar,
  GroupMemberList,
  GroupSetting,
  GroupMemberDetailList,
} from "~/components";
import { useConversationStore, useGroupStore } from "~/stores";
import { ElMessageBox } from "element-plus";

const $route = useRoute();
const $router = useRouter();
const groupStore = useGroupStore();
const conversationStore = useConversationStore();

const openMemberList = ref(false);

const groupIDChange = async function (groupID: string) {
  try {
    const { data: conversationItem } = await IMSDK.getOneConversation({
      sourceID: groupID,
      sessionType: 3,
    });

    await conversationStore.changeConversation(conversationItem);
  } catch (error) {
    $router.replace({ name: "group" });
  }
};
groupIDChange($route.params.groupID as string);
watch(() => $route.params.groupID as string, groupIDChange);

const onJoinedGrpDeleted = ({ data }: { data: IMTYPE.GroupItem }) => {
  if (data.groupID === groupStore.currentGroupInfo?.groupID!) {
    $router.replace({ name: "group" });
  }
};

IMSDK.on("OnJoinedGrpDeleted", onJoinedGrpDeleted);

onBeforeUnmount(() => {
  IMSDK.off("OnJoinedGrpDeleted", onJoinedGrpDeleted);
});

const updateAvatar = async (url: string) => {
  await IMSDK.setGrpInfo({
    groupID: groupStore.currentGroupInfo?.groupID!,
    faceURL: url,
  });
};

const dissolutionHandle = async () => {
  await ElMessageBox.confirm(
    `确认要解散群组"${groupStore.currentGroupInfo?.groupName}"吗?`,
    "解散群组",
    {
      confirmButtonText: "确定",
      cancelButtonText: "取消",
      type: "warning",
    }
  );

  await IMSDK.dismissGrp(groupStore.currentGroupInfo?.groupID!);
};

const exitHandle = async () => {
  await ElMessageBox.confirm(
    `确认要退出群组"${groupStore.currentGroupInfo?.groupName}"吗?`,
    "退出群组",
    {
      confirmButtonText: "确定",
      cancelButtonText: "取消",
      type: "warning",
    }
  );

  await IMSDK.quitGrp(groupStore.currentGroupInfo?.groupID!);
};

provide("showGroupMoreMemberList", () => {
  openMemberList.value = true;
});
</script>
<style lang="scss" scoped>
.w {
  padding: 0 34px;
}
.wrap {
  padding: 50px;
  display: flex;
  flex-direction: column;
  .scroll {
    flex: 1;
    .header {
      display: flex;
      .text {
        flex: 1;
        display: flex;
        flex-direction: column;
        justify-content: center;
        margin-left: 30px;
        text-wrap: nowrap;
        width: 0;
        font-size: 14px;
        line-height: 20px;
        .grounpName {
          overflow: hidden;
          text-overflow: ellipsis;
        }
      }
    }
  }
  .btn-group {
    height: 50px;
    display: flex;
    justify-content: center;
    align-items: center;
  }
}
</style>
