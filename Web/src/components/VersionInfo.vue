<template>
  <div class="wrap">
    <div class="box">
      <el-image class="img" :src="CRLogo"></el-image>
    </div>
    <p>应用版本: {{ version }}</p>
    <p>SDK版本: {{ IMSDK.sdkVersion() }}</p>
  </div>
</template>
<script lang="ts" setup>
import { IMSDK } from "~/utils/imsdk";
import CRLogo from "~/assets/CR_logo.svg";
import { computed } from "vue";

const version = computed(() => __APP_VERSION__);
</script>
<style lang="scss" scoped>
.wrap {
  border-top: 1px solid var(--el-border-color);
  padding: 90px 0;
  .box {
    text-align: center;
    margin-bottom: 40px;
    .img {
      width: 160px;
    }
  }
  p {
    text-align: center;
    margin-bottom: 20px;
    color: #333;
  }
}
</style>
