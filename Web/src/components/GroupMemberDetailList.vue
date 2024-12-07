<template>
  <div class="scroll">
    <ul class="ul">
      <li
        v-for="(item, index) in groupStore.currentGroupMemberList"
        :key="item.userID"
      >
        <el-dropdown
          @command="handleCommand"
          trigger="contextmenu"
          style="width: 100%"
          placement="left"
          ref="dropdownRef"
          type="primary"
          @visible-change="
            // @ts-ignore
            visibleChange($event, $refs.dropdownRef[index], item)
          "
        >
          <template #dropdown>
            <el-dropdown-menu>
              <el-dropdown-item v-for="item in list" @click="item.fn">
                {{ item.text }}
              </el-dropdown-item>
            </el-dropdown-menu>
          </template>
          <div
            class="item"
            @click="
              openUserInfo(
                item.userID,
                groupStore.currentGroupInfo,
                groupStore.isCurrentGroupAdmin
              )
            "
          >
            <Avatar :src="item.faceURL" :size="36" />
            <div class="r">
              <span class="name">{{ item.nickname }}</span>
              <span class="identity owner" v-if="item.roleLevel === 100">
                群主
              </span>
              <span class="identity admin" v-else-if="item.roleLevel === 60">
                管理员
              </span>
            </div>
          </div>
        </el-dropdown>
      </li>
    </ul>
  </div>
</template>
<script lang="ts" setup>
import { Avatar } from "~/components";
import { openUserInfo } from "~/utils";
import { useGroupStore } from "~/stores";
import { inject, Ref, ref } from "vue";
import { IMSDK, IMTYPE } from "~/utils/imsdk";

const groupStore = useGroupStore();

const dropdownVisibleChange = inject("dropdownVisibleChange") as (
  e: boolean,
  ref: any
) => void;
const dropdownRef = ref();

const handleCommand = (command: string) => {};
const list = ref<any[]>([]);

const visibleChange = (
  isOpen: boolean,
  ref: Ref,
  item: IMTYPE.GroupMemberItem
) => {
  dropdownVisibleChange(isOpen, ref);
  if (!isOpen) return;
  list.value = [];
  list.value.push({
    text: "查看资料",
    fn: () => {
      openUserInfo(
        item.userID,
        groupStore.currentGroupInfo,
        groupStore.isCurrentGroupAdmin
      );
    },
  });
  if (groupStore.currentMemberInGroup?.roleLevel! > item.roleLevel) {
    const isMute = !!item.muteEndTime;
    list.value.push({
      text: isMute ? "取消禁言" : "禁言",
      fn: () => {
        IMSDK.changeGrpMemberMute({
          groupID: groupStore.currentGroupInfo?.groupID!,
          userID: item.userID,
          mutedSeconds: isMute ? 0 : 315360000, //禁言十年大礼包
        });
      },
    });
    list.value.push({
      text: "移出本群",
      fn: () => {
        IMSDK.kickGrpMember({
          groupID: groupStore.currentGroupInfo?.groupID!,
          reason: "reason",
          userIDList: [item.userID],
        });
      },
    });
  }
  if (
    groupStore.currentMemberInGroup?.roleLevel! === 100 &&
    groupStore.currentMemberInGroup?.userID !== item.userID
  ) {
    list.value.push({
      text: "转让群主",
      fn: () => {
        IMSDK.transferGrpOwner({
          groupID: groupStore.currentGroupInfo?.groupID!,
          newOwnerUserID: item.userID,
        });
      },
    });
    const isManager = item.roleLevel === 60;
    list.value.push({
      text: isManager ? "取消管理员" : "设为管理员",
      fn: () => {
        IMSDK.setGrpMemberInfo({
          groupID: groupStore.currentGroupInfo?.groupID!,
          userID: item.userID,
          roleLevel: isManager ? 20 : 60,
        });
      },
    });
  }
};
</script>
<style lang="scss" scoped>
.scroll {
  height: 100%;
  background-color: #fff;
  .ul {
    height: 100%;
    overflow: auto;
    padding-top: 10px;
    li {
      padding: 0 6px;

      .item {
        width: 100%;
        height: 100%;
        padding: 10px;
        display: flex;
        align-items: center;
        border-radius: 4px;
        font-size: 12px;
        line-height: 20px;
        cursor: pointer;
        .r {
          flex: 1;
          margin-left: 10px;
          width: 0;
          text-wrap: nowrap;
          display: flex;
          .name {
            text-overflow: ellipsis;
            overflow: hidden;
          }
          .identity {
            padding: 0px 4px;
            color: #fff;
            border-radius: 4px;
            margin-left: 10px;
            &.owner {
              background-color: #ff970f;
            }
            &.admin {
              background-color: var(--el-color-primary);
            }
          }
        }
        &:hover {
          background: #e9ebf4;
        }
      }
    }
  }
}
</style>
