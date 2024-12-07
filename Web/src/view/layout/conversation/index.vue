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
        <el-dropdown
          ref="dropdownRef"
          trigger="contextmenu"
          style="width: 100%"
          placement="right"
          @command="handleCommand"
          @visible-change="
            // @ts-ignore
            dropdownVisibleChange($event, $refs.dropdownRef[index])
          "
        >
          <Card
            :active="item.conversationID === activeName"
            :name="item.showName"
            :imgSrc="item.faceURL"
            :conversationItem="item"
            :avatarType="item.groupID ? 'group' : 'user'"
          />
          <template #dropdown>
            <el-dropdown-menu>
              <el-dropdown-item
                :command="{
                  type: 'togglePinned',
                  conversation: item,
                }"
              >
                {{ item.isPinned ? "取消置顶" : "置顶" }}
              </el-dropdown-item>
              <el-dropdown-item
                v-if="item.unreadCount"
                :command="{ type: 'MsgAsRead', conversation: item }"
              >
                标记已读
              </el-dropdown-item>
              <el-dropdown-item
                :command="{ type: 'hidden', conversation: item }"
              >
                隐藏
              </el-dropdown-item>
              <el-dropdown-item :command="{ type: 'del', conversation: item }">
                移除会话
              </el-dropdown-item>
            </el-dropdown-menu>
          </template>
        </el-dropdown>
      </li>
      <Empty v-if="list.length === 0" text="暂无会话" />
    </template>
  </Layout>
</template>

<script setup lang="ts">
import { Search, Card, Layout, Empty } from "~/components";
import { useRoute, useRouter } from "vue-router";
import { computed, inject, onUnmounted, ref } from "vue";
import { useConversationStore } from "~/stores";
import { IMSDK } from "~/utils/imsdk";

const $route = useRoute();
const $router = useRouter();
const conversationStore = useConversationStore();
const searchText = ref("");
const dropdownVisibleChange = inject("dropdownVisibleChange");
const dropdownRef = ref();
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

const handleCommand = async (command: any) => {
  console.log(command);
  const conversation = command.conversation;
  if (command.type === "togglePinned") {
    IMSDK.pinConversation({
      conversationID: conversation.conversationID,
      isPinned: !conversation.isPinned,
    });
  }
  if (command.type === "MsgAsRead") {
    await IMSDK.markConversationMsgAsRead(conversation.conversationID);
  }
  if (command.type === "hidden") {
    await IMSDK.hideConversation(conversation.conversationID);
    const idx = conversationStore.conversationList?.findIndex(
      ({ conversationID }) => conversation.conversationID === conversationID
    ) as number;
    if (idx > -1) {
      conversationStore.conversationList?.splice(idx, 1);
    }
  }
  if (command.type === "del") {
    await IMSDK.deleteConversationAndDeleteAllMsg(conversation.conversationID);
    const idx = conversationStore.conversationList?.findIndex(
      ({ conversationID }) => conversation.conversationID === conversationID
    ) as number;
    if (idx > -1) {
      conversationStore.conversationList?.splice(idx, 1);
      if (activeName.value === conversation.conversationID) {
        conversationStore.changeConversation(null);
        $router.push({ name: "chat" });
      }
    }
  }
};
</script>
