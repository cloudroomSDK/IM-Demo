<template>
  <div class="msg-search">
    <Search
      v-model="searchText"
      class="search"
      :disabled="['图片', '视频', '位置'].indexOf(activeName) > -1"
    />
    <ul class="tabs">
      <li
        v-for="(item, idx) in Object.keys(typeObj)"
        :key="idx"
        :class="{ activeName: activeName === item }"
      >
        <span @click="activeName = item">{{ item }}</span>
      </li>
    </ul>
    <div v-loading="!list">
      <div
        v-if="list && list.length"
        class="msg-list"
        v-infinite-scroll="getSearchList"
      >
        <ul class="message">
          <li v-for="item in list" :key="item.clientMsgID">
            <MessageItem
              class="message-item"
              :source="item"
              leftLayout
              disabledRightClick
            />
            <el-divider class="divider" />
          </li>
        </ul>
      </div>
      <Empty v-else class="msg-list" />
    </div>
  </div>
</template>

<script lang="ts" setup>
import { ref } from "vue";
import { Search, Empty, MessageItem } from ".";
import { IMSDK, IMTYPE } from "~/utils/imsdk";
import { useConversationStore } from "~/stores";
import { watch } from "vue";
const searchText = ref("");
const typeObj = {
  文本: [101, 106, 114],
  图片: [102],
  视频: [104],
  文件: [105],
  位置: [109],
};

const activeName = ref("文本");

const list = ref<IMTYPE.MessageItem[] | undefined>([]);
const conversationStore = useConversationStore();
let text = "";
let searchPage = 1;
let end = false;

watch(
  () => [searchText.value, activeName.value],
  () => {
    start();
  }
);

const start = () => {
  text = searchText.value;

  if (["图片", "视频", "位置"].indexOf(activeName.value) > -1) {
    text = "";
  }
  searchPage = 1;
  end = false;
  list.value = undefined;
  getSearchList();
};

const getSearchList = async () => {
  if (end) return;
  console.log({
    conversationID: conversationStore.currentConversation?.conversationID || "",
    keywordList: [text],
    // @ts-ignore
    messageTypeList: typeObj[activeName.value],
    pageIndex: searchPage,
    count: 20,
  });
  const { data } = await IMSDK.searchLocalMsgs({
    conversationID: conversationStore.currentConversation?.conversationID || "",
    keywordList: [text],
    // @ts-ignore
    messageTypeList: typeObj[activeName.value],
    pageIndex: searchPage++,
    count: 20,
  });
  if (!list.value) {
    list.value = [];
  }
  if (data.searchResultItems) {
    console.log(data.searchResultItems[0].messageList);
    if (data.searchResultItems[0].messageList.length < 20) {
      end = true;
    }
    list.value = list.value.concat(data.searchResultItems[0].messageList);
  }
};
start();
</script>
<style lang="scss" scoped>
.msg-search {
  padding-top: 20px;
  .search {
    padding: 0 20px;
  }
}
.tabs {
  display: flex;
  font-size: 14px;
  line-height: 30px;
  color: #333;
  li {
    flex: 1;
    text-align: center;
    position: relative;

    span {
      cursor: pointer;
    }
    &::after {
      position: absolute;
      content: "";
      width: 0;
      height: 2px;
      background-color: var(--el-color-primary);
      bottom: 0;
      left: 50%;
      transform: translateX(-50%);
      transition: width 0.3s ease;
    }

    &.activeName {
      &::after {
        width: 20px;
      }
    }
  }
}
.msg-list {
  margin-top: 10px;
  height: 300px;
  overflow-y: auto;
  .message-item {
    padding: 10px;
  }
  .divider {
    margin: 0;
  }
}
</style>
