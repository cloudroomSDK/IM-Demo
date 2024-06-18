<template>
  <div>
    <el-empty :description="text" />
  </div>
</template>
<script setup lang="ts">
withDefaults(
  defineProps<{
    text?: string;
  }>(),
  {
    text: "暂无内容",
  }
);
</script>

<style lang="scss" scoped>
div {
  height: 100%;
  display: flex;
  justify-content: center;
  align-items: center;
}
</style>
