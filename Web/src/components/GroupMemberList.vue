<template>
  <div style="padding-bottom: 10px; background-color: #fff">
    <div class="memberList">
      <div
        class="memberItem"
        v-for="member in memberList.slice(0, isManager ? 13 : 14)"
      >
        <a href="javascript:;">
          <Avatar
            :size="30"
            :src="member.faceURL"
            @click="openUserInfo(member.userID, groupInfo.groupID)"
          />
        </a>
        <p>{{ member.nickname }}</p>
      </div>
      <div class="memberItem" @click="inviteFriendInGroup(groupInfo.groupID)">
        <div class="icon">
          <el-icon><i-ep-plus /></el-icon>
        </div>
      </div>
      <div class="memberItem" @click="kickoutHandle" v-if="isManager">
        <div class="icon">
          <el-icon><i-ep-minus /></el-icon>
        </div>
      </div>
    </div>
    <div class="memberMore">
      <el-link :underline="false" @click="showGroupMoreMemberList">
        查看更多<el-icon><i-ep-arrowDown /></el-icon>
      </el-link>
    </div>
  </div>
</template>
<script lang="ts" setup>
import { computed, inject, onBeforeMount, onUnmounted, ref } from "vue";
import { useAppStore } from "~/stores";
import { openUserInfo } from "~/utils";
import { IMSDK, IMTYPE, inviteFriendInGroup } from "~/utils/imsdk";
import { Avatar, MemberSelect } from ".";
const showGroupMoreMemberList = inject('showGroupMoreMemberList')

const appStore = useAppStore();

const props = defineProps<{
  groupInfo: IMTYPE.GroupItem;
  myMemberInfo: IMTYPE.GroupMemberItem;
}>();
const isManager = computed(
  () =>
    props.myMemberInfo.roleLevel === 100 || props.myMemberInfo.roleLevel === 60
);

const memberList = ref<IMTYPE.GroupMemberItem[]>([]);
const onGrpMemberAdded = ({ data }: { data: IMTYPE.GroupMemberItem }) => {
  if (props.groupInfo.groupID === data.groupID) {
    memberList.value.push(data);
  }
};
const onGrpMemberDeleted = ({ data }: { data: IMTYPE.GroupMemberItem }) => {
  if (props.groupInfo.groupID === data.groupID) {
    const idx = memberList.value.findIndex(
      (item) => item.userID === data.userID
    );
    if (idx > -1) {
      memberList.value.splice(idx, 1);
    }
  }
};
onBeforeMount(async () => {
  const { data } = await IMSDK.getGrpMemberList({
    groupID: props.groupInfo.groupID,
    offset: 0,
    count: 20,
    filter: 0,
  });
  memberList.value = data;

  IMSDK.on("OnGrpMemberAdded", onGrpMemberAdded);
  IMSDK.on("OnGrpMemberDeleted", onGrpMemberDeleted);
});
onUnmounted(() => {
  IMSDK.off("OnGrpMemberAdded", onGrpMemberAdded);
  IMSDK.off("OnGrpMemberDeleted", onGrpMemberDeleted);
});

const kickoutHandle = async () => {
  const selectUserList = await appStore.showDialog<
    {
      userID: string;
      imgSrc: string;
      showName: string;
    }[]
  >({
    component: MemberSelect,
    title: "选择联系人",
    width: 700,
    data: {
      type: "kickout",
      groupID: props.groupInfo.groupID,
    },
  });

  await IMSDK.kickGrpMember({
    groupID: props.groupInfo.groupID,
    reason: "",
    userIDList: selectUserList.map((item) => item.userID),
  });
};
</script>
<style lang="scss" scoped>
.memberList {
  display: flex;
  flex-wrap: wrap;
  padding: 25px 25px 10px;
  .memberItem {
    text-align: center;
    width: 50px;
    margin-bottom: 10px;
    p {
      text-wrap: nowrap;
      overflow: hidden;
      text-overflow: ellipsis;
      font-size: 12px;
    }
  }
  .icon {
    display: inline-block;
    cursor: pointer;
    width: 30px;
    height: 30px;
    border-radius: 4px;
    border: 1px solid var(--el-border-color);
    line-height: 30px;
    color: #5d5c68;
    font-size: 16px;
  }
}
.memberMore {
  text-align: center;
  font-size: 14px;
  line-height: 14px;
}
</style>
