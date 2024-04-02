<template>
  <Layout>
    <template #search>
      <Search v-model="searchText" />
    </template>
    <template #list>
      <li
        v-for="item in list"
        :key="item.conversationID"
        @click="
          conversationStore.gotoConversationChat({ conversationItem: item })
        "
      >
        <Card
          :active="item.conversationID === activeName"
          :name="item.showName"
          :imgSrc="item.faceURL"
          :conversationItem="item"
          :avatarType="item.groupID ? 'group' : 'user'"
        />
      </li>
    </template>
  </Layout>
</template>

<script setup lang="ts">
import { Search, Card, Layout } from "~/components";
import { useRoute } from "vue-router";
import { computed, onUnmounted, ref } from "vue";
import { useConversationStore } from "~/stores";

const $route = useRoute();
const conversationStore = useConversationStore();
const searchText = ref("");
conversationStore.getList();

const activeName = computed(() => {
  if ($route.name === "conversation") {
    return $route.params.conversationID;
  }
  return "";
});

const list = computed(() => {
  if (!conversationStore.conversationList) return [];

  return conversationStore.conversationList.filter((item) => {
    if (searchText.value.trim()) {
      return (
        item.userID
          .toLocaleLowerCase()
          .indexOf(searchText.value.toLocaleLowerCase()) > -1 ||
        item.showName
          .toLocaleLowerCase()
          .indexOf(searchText.value.toLocaleLowerCase()) > -1
      );
    }
    return true;
  });
});

onUnmounted(() => {
  conversationStore.changeConversation(null);
});
</script>
