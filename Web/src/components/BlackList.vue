<template>
  <div class="bt">
    <el-scrollbar height="400px" v-if="list && list.length">
      <div class="list">
        <div class="item" v-for="(item, index) in list">
          <div class="row">
            <Avatar :src="item.faceURL" :size="30" />
            <span class="nickname">{{ item.nickname }}</span>
            <el-button type="primary" text @click="remove(item.userID)">
              移除
            </el-button>
          </div>
          <el-divider class="divider" v-if="index !== list.length - 1" />
        </div>
      </div>
    </el-scrollbar>
    <Empty v-else class="empty" />
  </div>
</template>
<script lang="ts" setup>
import { computed } from "vue";
import { useFriendStore } from "~/stores";
import { IMSDK } from "~/utils/imsdk";
import { Avatar, Empty } from ".";
const friendStore = useFriendStore();
friendStore.getBlackList();

const list = computed(() => friendStore.blackList);

const remove = (userID: string) => {
  IMSDK.removeBlack(userID);
};
</script>
<style lang="scss" scoped>
.bt {
  border-top: 1px solid var(--el-border-color);
}
.empty {
  display: flex;
  justify-content: center;
  align-items: center;
  height: 400px;
}
.list {
  padding: 10px 35px;
  .row {
    padding: 0 20px;
    display: flex;
    justify-content: center;
    align-items: center;
    .nickname {
      flex: 1;
      margin-left: 10px;
      text-wrap: nowrap;
      overflow: hidden;
      text-overflow: ellipsis;
    }
  }
  .divider {
    margin: 10px 0;
  }
}
</style>
