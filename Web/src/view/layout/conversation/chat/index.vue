<template>
  <div class="container">
    <Header class="header" />
    <div class="chat">
      <List
        v-if="conversationStore.currentConversation?.conversationID"
        :key="conversationStore.currentConversation?.conversationID"
      />
    </div>
    <Footer class="footer" />
  </div>
</template>

<script setup lang="ts">
import { useRoute, useRouter } from "vue-router";
import { useConversationStore } from "~/stores";
import List from "./list.vue";
import Header from "./header.vue";
import Footer from "./footer.vue";

const $route = useRoute();
const $router = useRouter();
const conversationStore = useConversationStore();

const conversationChange = async function (conversationID: string) {
  if (!conversationStore.currentConversation) {
    try {
      await conversationStore.gotoConversationChat({ conversationID });
    } catch (error) {
      console.log(error);
      $router.push({ name: "chat" });
    }
  }
};
conversationChange($route.params.conversationID as string);
</script>
<style lang="scss" scoped>
.container {
  display: flex;
  flex-direction: column;
  .header {
    height: 64px;
    display: flex;
    align-items: center;
    border-bottom: 1px solid var(--el-border-color);
  }
  .chat {
    flex: 1;
    overflow: hidden;
    height: 100%;
  }
  .footer {
    border-top: 1px solid var(--el-border-color);
    height: 170px;
  }
}
</style>
