<template>
  <el-popover placement="right" :width="160" :visible="true">
    <template #reference>
      <i :style="{ top: top, left: left }" style="position: absolute" />
    </template>
    <el-mention
      v-model="value"
      :options="options"
      @select="onSelect"
      @blur="$emit('hideMentionModal')"
      ref="mention"
    />
  </el-popover>
</template>

<script setup lang="ts">
import type { MentionOption } from "element-plus";
import { onMounted, ref, nextTick } from "vue";
import { useGroupStore } from "~/stores";
const groupStore = useGroupStore();

const value = ref("@");
const options = groupStore.currentGroupMemberList?.map((item) => ({
  label: item.nickname,
  value: item.userID,
}));

const $emit = defineEmits(["hideMentionModal", "insertMention"]);
const mention = ref(),
  top = ref(""),
  left = ref("");

onMounted(() => {
  // 获取光标位置
  const domSelection = document.getSelection();
  const domRange = domSelection?.getRangeAt(0);
  if (domRange == null) return;
  const rect = domRange.getBoundingClientRect();

  // 定位 modal
  top.value = `${rect.top + 10}px`;
  left.value = `${rect.left - 60}px`;

  nextTick(() => {
    mention.value.input.focus();
  });
});

const onSelect = (option: MentionOption) => {
  $emit("hideMentionModal"); // 隐藏 modal
  $emit("insertMention", option.value, option.label);
};
</script>

<style lang="scss" scoped>
.mention-modal {
  background-color: #fff;
  padding: 5px;
  input {
    outline: none;
  }
  ul {
    padding: 0;
    margin: 0;
    max-height: 88px;
    overflow: auto;
    li {
      list-style: none;
      cursor: pointer;
      padding: 3px 0;
      text-align: left;
      // max-width: 140px;
      white-space: nowrap;
      overflow: hidden;
      text-overflow: ellipsis;
      &:hover {
        text-decoration: underline;
      }
    }
  }
}
</style>
