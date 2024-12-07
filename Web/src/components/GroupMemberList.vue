<template>
  <div style="padding-bottom: 10px; background-color: #fff">
    <div class="memberList">
      <div class="memberItem" v-for="member in memberList">
        <a href="javascript:;">
          <Avatar
            :size="30"
            :src="member.faceURL"
            @click="
              openUserInfo(
                member.userID,
                groupStore.currentGroupInfo,
                groupStore.isCurrentGroupAdmin
              )
            "
          />
        </a>
        <p>{{ member.nickname }}</p>
      </div>
      <div
        class="memberItem"
        @click="inviteFriendInGroup(groupStore.currentGroupInfo?.groupID!)"
      >
        <div class="icon">
          <el-icon><i-ep-plus /></el-icon>
        </div>
      </div>
      <div
        v-if="groupStore.isCurrentGroupAdmin"
        class="memberItem"
        @click="kickoutHandle"
      >
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
import { computed, inject } from "vue";
import { useAppStore, useGroupStore } from "~/stores";
import { openUserInfo } from "~/utils";
import { IMSDK, inviteFriendInGroup } from "~/utils/imsdk";
import { Avatar, MemberSelect } from ".";
const showGroupMoreMemberList = inject("showGroupMoreMemberList") as (
  evt: MouseEvent
) => any;

const appStore = useAppStore();
const groupStore = useGroupStore();

const memberList = computed(() => {
  return groupStore.currentGroupMemberList!.slice(
    0,
    groupStore.isCurrentGroupAdmin ? 13 : 14
  );
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
      type: "groupMember",
      groupID: groupStore.currentGroupInfo?.groupID!,
    },
  });

  await IMSDK.kickGrpMember({
    groupID: groupStore.currentGroupInfo?.groupID!,
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
