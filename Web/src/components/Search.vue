<template>
  <div class="search">
    <el-input
      :prefix-icon="Search"
      clearable
      class="input"
      v-model="model"
      placeholder="搜索"
    />
  </div>
</template>

<script lang="ts" setup>
import { Search } from "@element-plus/icons-vue";
const model = defineModel();
</script>
<style lang="scss" scoped>
.search {
  display: flex;
  align-items: center;
  justify-content: center;
  height: 100%;

  .input {
    --el-input-bg-color: #f0f0f0;
    --el-input-text-color: #666;
    --el-input-border: transparent;
    --el-input-hover-border: transparent;
    --el-input-border-color: transparent;
    --el-input-icon-color: #666;
    --el-input-placeholder-color: #666;
    --el-input-hover-border-color: transparent;
    --el-input-clear-hover-color: #666;
    // --el-input-focus-border: transparent;
    // --el-input-focus-border-color: transparent;
  }
}
</style>
