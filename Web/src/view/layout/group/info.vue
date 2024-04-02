<template>
  <div class="wrap" v-if="groupInfo && myMemberInfo">
    <el-scrollbar class="scroll">
      <div class="w header">
        <CanEditAvatar
          :src="groupInfo?.faceURL"
          :size="60"
          :disabled="!isManager"
          type="group"
          @updateUrl="updateAvatar"
        />
        <div class="text">
          <div class="grounpName">群名称：{{ groupInfo?.groupName }}</div>
          <div class="grounpID">群ID：{{ groupInfo?.groupID }}</div>
        </div>
      </div>
      <el-divider />
      <div class="w memberList">
        <GroupMemberList
          :key="groupInfo"
          :groupInfo="groupInfo"
          :myMemberInfo="myMemberInfo"
        />
      </div>
      <el-divider />
      <div class="w setting">
        <GroupSetting
          :key="conversationInfo"
          :groupInfo="groupInfo"
          :conversationInfo="conversationInfo"
          :myMemberInfo="myMemberInfo"
        />
      </div>
    </el-scrollbar>
    <div class="btn-group" v-if="myMemberInfo">
      <el-button
        type="danger"
        @click="dissolutionHandle"
        v-if="myMemberInfo?.roleLevel === 100"
      >
        解散群组
      </el-button>
      <el-button type="danger" @click="exitHandle" v-else> 退出群组 </el-button>
      <el-button
        type="primary"
        @click="
          conversationStore.gotoConversationChat({
            conversationItem: conversationInfo,
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
          <GroupMemberDetailList
            :groupID="$route.params.groupID"
            :myMemberInfo="myMemberInfo"
          />
        </div>
      </div>
    </el-drawer>
  </div>
</template>
<script lang="ts" setup>
import { computed, onBeforeUnmount, provide, ref, watch } from "vue";
import { useRoute, useRouter } from "vue-router";
import { IMSDK, IMTYPE } from "~/utils/imsdk";
import {
  CanEditAvatar,
  GroupMemberList,
  GroupSetting,
  GroupMemberDetailList,
} from "~/components";
import { useUserStore, useConversationStore } from "~/stores";
import { ElMessageBox } from "element-plus";

const $route = useRoute();
const $router = useRouter();
const userStore = useUserStore();
const conversationStore = useConversationStore();

const openMemberList = ref(false);
const groupInfo = ref<IMTYPE.GroupItem>();
const myMemberInfo = ref<IMTYPE.GroupMemberItem>();
const conversationInfo = ref<IMTYPE.ConversationItem>();

const isManager = computed(() => {
  if (!myMemberInfo.value) return false;
  return (
    myMemberInfo.value.roleLevel === 100 || myMemberInfo.value.roleLevel === 60
  );
});

const groupIDChange = async function (groupID: string) {
  const { data } = await IMSDK.getSpecifiedGrpsInfo([groupID]);
  if (data.length && data[0].memberCount > 0) {
    groupInfo.value = data[0];

    const { data: conversation } = await IMSDK.getOneConversation({
      sourceID: groupID,
      sessionType: 3,
    });
    conversationInfo.value = conversation;

    const { data: memberList } = await IMSDK.getSpecifiedGrpMembersInfo({
      groupID: groupID,
      userIDList: [userStore.userInfo!.userID],
    });

    if (memberList.length) {
      myMemberInfo.value = memberList[0];
    }
  } else {
    $router.replace({ name: "group" });
  }
};
groupIDChange($route.params.groupID as string);
watch(() => $route.params.groupID as string, groupIDChange);

const onGrpInfoChanged = ({ data }: { data: IMTYPE.GroupItem }) => {
  if (data.groupID === groupInfo.value?.groupID) {
    groupInfo.value = data;
  }
};
const onConversationChanged = ({
  data,
}: {
  data: IMTYPE.ConversationItem[];
}) => {
  data.forEach((item) => {
    if (item.conversationID === conversationInfo.value?.conversationID) {
      conversationInfo.value = item;
    }
  });
};

const onJoinedGrpDeleted = ({ data }: { data: IMTYPE.GroupItem }) => {
  if (data.groupID === groupInfo.value?.groupID) {
    $router.replace({ name: "group" });
  }
};

IMSDK.on("OnGrpInfoChanged", onGrpInfoChanged);
IMSDK.on("OnJoinedGrpDeleted", onJoinedGrpDeleted);
IMSDK.on("OnConversationChanged", onConversationChanged);

onBeforeUnmount(() => {
  IMSDK.off("OnGrpInfoChanged", onGrpInfoChanged);
  IMSDK.off("OnJoinedGrpDeleted", onJoinedGrpDeleted);
  IMSDK.off("OnConversationChanged", onConversationChanged);
});

const updateAvatar = async (url: string) => {
  await IMSDK.setGrpInfo({
    groupID: groupInfo.value?.groupID!,
    faceURL: url,
  });
};

const dissolutionHandle = async () => {
  await ElMessageBox.confirm(
    `确认要解散群组"${groupInfo.value?.groupName}"吗?`,
    "解散群组",
    {
      confirmButtonText: "确定",
      cancelButtonText: "取消",
      type: "warning",
    }
  );

  await IMSDK.dismissGrp(groupInfo.value?.groupID!);
};

const exitHandle = async () => {
  await ElMessageBox.confirm(
    `确认要退出群组"${groupInfo.value?.groupName}"吗?`,
    "退出群组",
    {
      confirmButtonText: "确定",
      cancelButtonText: "取消",
      type: "warning",
    }
  );

  await IMSDK.quitGrp(groupInfo.value?.groupID!);
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
