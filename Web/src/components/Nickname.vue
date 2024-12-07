<template>
  <template v-if="infos?.length">
    <template v-for="(item, idx) in infos">
      <span
        @click="
          openUserInfo(
            item.userID!,
            groupStore.currentGroupInfo,
            groupStore.isCurrentGroupAdmin
          )
        "
      >
        {{ showAt ? "@" : ""
        }}{{
          item.userID === userStore.userInfo?.userID ? "你" : item.nickname
        }}
      </span>
      <template v-if="idx !== infos.length - 1">、</template>
    </template>
  </template>
  <template v-else>
    <span
      @click="
        openUserInfo(
          userID!,
          groupStore.currentGroupInfo,
          groupStore.isCurrentGroupAdmin
        )
      "
    >
      {{ showAt ? "@" : ""
      }}{{ userStore.userInfo?.userID === userID ? "你" : nickname }}
    </span>
  </template>
</template>
<script lang="ts" setup>
import { useUserStore, useGroupStore } from "~/stores";
import { openUserInfo } from "~/utils";
const userStore = useUserStore();
const groupStore = useGroupStore();
type userInfo = { nickname?: string; userID?: string };
defineProps<
  userInfo & {
    infos?: userInfo[];
    showAt?: boolean;
  }
>();
</script>
<style lang="scss" scoped>
span {
  color: var(--el-color-primary);
  cursor: pointer;
}
</style>
