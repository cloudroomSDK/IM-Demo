<template>
  <div class="container">
    <div class="left" :style="{ width: appStore.sideWidth + 'px' }">
      <div class="search">
        <slot name="search"></slot>
      </div>
      <el-scrollbar class="scrollbar">
        <ul class="list">
          <slot name="list"></slot>
        </ul>
      </el-scrollbar>
    </div>
    <router-view class="right"></router-view>
  </div>
</template>

<script setup lang="ts">
import { useAppStore } from "~/stores";
const appStore = useAppStore();
</script>

<style lang="scss" scoped>
.container {
  display: flex;
  .left {
    height: 100%;
    display: flex;
    flex-direction: column;
    border-right: 1px solid var(--el-border-color);
    max-width: 80%;
    min-width: 160px;
    .search {
      height: 64px;
      padding: 10px;
      border-bottom: 1px solid var(--el-border-color);
    }
    .scrollbar {
      flex: 1;
      .list {
        padding: 10px;
      }
    }
  }
  .right {
    flex: 1;
    overflow-y: auto;
  }
}
</style>
