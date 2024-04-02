<template>
  <template v-if="infos?.length">
    <template v-for="(item, idx) in infos">
      <span
        @click="
          openUserInfo(
            item.userID!,
            conversationStore.currentConversation?.groupID
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
        openUserInfo(userID!, conversationStore.currentConversation?.groupID)
      "
    >
      {{ showAt ? "@" : ""
      }}{{ userStore.userInfo?.userID === userID ? "你" : nickname }}
    </span>
  </template>
</template>
<script lang="ts" setup>
import { useUserStore, useConversationStore } from "~/stores";
import { openUserInfo } from "~/utils";
const userStore = useUserStore();
const conversationStore = useConversationStore();
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
