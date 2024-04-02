<template>
  <template v-if="has">
    <el-popover :visible="visible" placement="top" :width="150">
      <template #reference>
        <div
          @contextmenu.prevent.stop="visible = true"
          style="display: inline-block"
        >
          <slot></slot>
        </div>
      </template>
      <ul class="popover-ul">
        <li><div @click="$emit('transmit')">转发</div></li>
        <li><div @click="$emit('multiple')">多选</div></li>
        <li><div @click="$emit('quote')">回复</div></li>
        <li v-if="message.sendID === userStore.userInfo?.userID">
          <div @click="$emit('revok')">撤回</div>
        </li>
        <li><div @click="$emit('del')">删除</div></li>
      </ul>
    </el-popover>
  </template>
  <template v-else>
    <slot></slot>
  </template>
</template>
<script lang="ts" setup>
import { onMounted, onUnmounted, ref } from "vue";
import { useUserStore } from "~/stores";
import { IMTYPE } from "~/utils/imsdk";

const userStore = useUserStore();

defineEmits(["transmit", "multiple", "quote", "revok", "del"]);
defineProps<{
  has: boolean;
  message: IMTYPE.MessageItem;
}>();
const visible = ref(false);

const closePopover = () => (visible.value = false);

onMounted(() => {
  window.addEventListener("click", closePopover);
  window.addEventListener("contextmenu", closePopover);
  window.addEventListener("resize", closePopover);
  document.addEventListener("scroll", closePopover);
  document.addEventListener("keydown", closePopover);
});
onUnmounted(() => {
  window.removeEventListener("click", closePopover);
  window.removeEventListener("contextmenu", closePopover);
  window.removeEventListener("resize", closePopover);
  document.removeEventListener("scroll", closePopover);
  document.removeEventListener("keydown", closePopover);
});
</script>
<style lang="scss" scoped>
.popover-ul {
  div {
    font-size: 14px;
    height: 40px;
    line-height: 40px;
    padding: 0 18px;
    border-radius: 4px;
    text-align: center;
    cursor: pointer;
    &:hover {
      background-color: #e9ebf4;
    }
  }
}
</style>
