<template>
  <div class="scroll">
    <ul
      class="ul"
      v-infinite-scroll="getList"
      :infinite-scroll-disabled="!canLoading && scrollDisabled"
    >
      <li v-for="item in list" :key="item.userID">
        <div class="item" @click="openUserInfo(item.userID, groupID)">
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
      </li>
      <p v-if="canLoading" style="text-align: center">Loading...</p>
    </ul>
  </div>
</template>
<script lang="ts" setup>
import { ref } from "vue";
import { IMSDK, IMTYPE } from "~/utils/imsdk";
import { Avatar } from "~/components";
import { openUserInfo } from "~/utils";
const props = defineProps<{
  groupID: string;
  myMemberInfo: IMTYPE.GroupMemberItem;
}>();
console.log(props.groupID, props.myMemberInfo);
const count = 100; //分页拉取成员数量
let offset = 0;
const canLoading = ref(true);
const list = ref<IMTYPE.GroupMemberItem[]>([]);
const scrollDisabled = ref(false);

const getList = async () => {
  if (scrollDisabled.value) return;
  scrollDisabled.value = true;
  const { data } = await IMSDK.getGrpMemberList({
    groupID: props.groupID,
    filter: 0,
    offset: offset,
    count: count,
  });
  canLoading.value = data.length >= count;
  offset += count;
  list.value = list.value.concat(data);
  scrollDisabled.value = false;
};

getList();
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
      height: 50px;
      padding: 0 6px;

      .item {
        height: 100%;
        padding-left: 10px;
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
          background: #f3f8fe;
        }
      }
    }
  }
}
</style>
