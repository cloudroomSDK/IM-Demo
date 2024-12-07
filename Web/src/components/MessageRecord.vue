<template>
  <div class="msg-search">
    <div v-loading="!list">
      <div v-if="list && list.length" class="msg-list">
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
import { IMTYPE } from "~/utils/imsdk";
import { Empty, MessageItem } from ".";
defineProps<{
  list: IMTYPE.MessageItem[];
}>();
</script>
<style lang="scss" scoped>
.msg-list {
  max-height: 500px;
  overflow-y: auto;
  .message-item {
    padding: 10px;
  }
  .divider {
    margin: 0;
  }
}
</style>
