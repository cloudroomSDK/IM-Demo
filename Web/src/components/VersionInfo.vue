<template>
  <div class="wrap">
    <div class="box">
      <el-image class="img" :src="logo"></el-image>
    </div>
    <p>应用版本: {{ version }}</p>
    <p>SDK版本: {{ IMSDK.sdkVersion }}</p>
  </div>
</template>
<script lang="ts" setup>
import { IMSDK } from "~/utils/imsdk";
import logo from "~/assets/logo.svg";
import { computed } from "vue";

const version = computed(() => __APP_VERSION__);
</script>
<style lang="scss" scoped>
.wrap {
  border-top: 1px solid var(--el-border-color);
  padding: 40px 0;
  .box {
    text-align: center;
    margin-bottom: 40px;
    .img {
      width: 80px;
    }
  }
  p {
    text-align: center;
    margin-bottom: 20px;
    color: #333;
  }
}
</style>
