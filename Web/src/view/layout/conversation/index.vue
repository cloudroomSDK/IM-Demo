<template>
  <Layout>
    <template #search>
      <Search v-model="searchText" />
    </template>
    <template #list>
      <li
        v-for="(item, index) in list"
        :key="item.conversationID"
        @click="
          conversationStore.gotoConversationChat({ conversationItem: item })
        "
      >
        <Popover
          :offset="-60"
          placement="right-start"
          :width="80"
          :getData="getData"
          :id="index"
        >
          <Card
            :active="item.conversationID === activeName"
            :name="item.showName"
            :imgSrc="item.faceURL"
            :conversationItem="item"
            :avatarType="item.groupID ? 'group' : 'user'"
          />
        </Popover>
      </li>
      <Empty v-if="list.length === 0" text="暂无会话" />
    </template>
  </Layout>
</template>

<script setup lang="ts">
import { Search, Card, Layout, Empty, Popover } from "~/components";
import { useRoute, useRouter } from "vue-router";
import { computed, onUnmounted, ref } from "vue";
import { useConversationStore } from "~/stores";
import { IMSDK } from "~/utils/imsdk";

const $route = useRoute();
const $router = useRouter();
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

const getData = (id: number) => {
  if (!conversationStore?.conversationList) return;
  const conversationItem = conversationStore.conversationList[id];
  if (!conversationItem) return;
  return [
    {
      text: conversationItem.isPinned ? "取消置顶" : "置顶",
      click() {
        IMSDK.pinConversation({
          conversationID: conversationItem.conversationID,
          isPinned: !conversationItem.isPinned,
        });
      },
    },
    {
      text: "移除会话",
      async click() {
        await IMSDK.deleteConversationAndDeleteAllMsg(
          conversationItem.conversationID
        );
        const idx = conversationStore.conversationList?.findIndex(
          ({ conversationID }) =>
            conversationItem.conversationID === conversationID
        ) as number;
        if (idx > -1) {
          conversationStore.conversationList?.splice(idx, 1);
          if (activeName.value === conversationItem.conversationID) {
            conversationStore.changeConversation(null);
            $router.push({ name: "chat" });
          }
        }
      },
    },
  ];
};
</script>
