<template>
  <div class="context">
    <div class="search">
      <el-input
        @keydown.enter="search"
        v-model="input"
        placeholder="请输入用户ID"
        ref="$input"
      >
        <template #append>
          <el-button @click="search"><i-ep-search /></el-button>
        </template>
      </el-input>
    </div>
    <div v-loading="loading">
      <div class="info" v-if="list.length">
        <div class="item" v-for="item in list" :key="item.userID">
          <UserInfo class="userInfo" :userID="item.userID" />
        </div>
      </div>
      <Empty v-else />
    </div>
  </div>
</template>
<script lang="ts" setup>
import { ref } from "vue";
import { BusinessUserInfo, searchBusinessUserInfo } from "~/api/login";
import { UserInfo, Empty } from "~/components";

const input = ref("");
const $input = ref<HTMLElement | null>(null);
const list = ref<BusinessUserInfo[]>([]);
const loading = ref(false);

const search = async () => {
  $input.value!.blur();
  if (!input.value) {
    list.value = [];
    return;
  }
  loading.value = true;
  try {
    const data = await searchBusinessUserInfo(input.value);
    list.value = data.total ? data.users : [];
  } catch (error) {}
  loading.value = false;
};
</script>

<style lang="scss" scoped>
.context {
  padding: 50px;
  .info {
    display: flex;
    justify-content: space-around;
    margin-top: 20px;
    flex-wrap: wrap;
    .item {
      padding: 0 5px;
      width: 280px;
      .userInfo {
        margin-bottom: 20px;
        box-shadow: var(--el-box-shadow-light);
        padding: 20px 26px;
      }
    }
  }
}
</style>
