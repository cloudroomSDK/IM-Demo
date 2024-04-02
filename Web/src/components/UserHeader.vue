<template>
  <div>
    <div class="header">
      <CanEditAvatar
        :src="imgSrc"
        :disabled="userStore.userInfo?.userID !== userID"
        :size="42"
        @updateUrl="updateUrl"
      />
      <div class="r">
        <div class="name">{{ nickname }}</div>
        <div class="id">{{ userID }}</div>
      </div>
    </div>
  </div>
</template>
<script lang="ts" setup>
defineProps<{
  nickname: string;
  userID: string;
  imgSrc?: string;
}>();
import { CanEditAvatar } from ".";
import { useUserStore } from "~/stores";
import { updateBusinessUserInfo } from "~/api/login";
const userStore = useUserStore();

const updateUrl = (url: string) => {
  updateBusinessUserInfo({
    faceURL: url,
  });
};
</script>
<style lang="scss" scoped>
.header {
  display: flex;
  .r {
    margin-left: 10px;
    display: flex;
    flex-direction: column;
    justify-content: space-between;
    flex: 1;
    width: 0;
    .name {
      font-size: 20px;
      overflow: hidden;
      text-overflow: ellipsis;
      flex: 2;
    }
    .id {
      font-size: 12px;
      overflow: hidden;
      text-overflow: ellipsis;
      flex: 1;
    }
  }
}
</style>
