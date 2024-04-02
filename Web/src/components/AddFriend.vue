<template>
  <div>
    <UserHeader :nickname="nickname" :userID="userID" :imgSrc="imgSrc" />

    <p class="text">验证信息</p>
    <el-input
      v-model="textarea"
      type="textarea"
      :maxlength="50"
      :rows="6"
      resize="none"
    />
  </div>
</template>
<script lang="ts" setup>
import { UserHeader } from ".";
import { onMounted, ref, watch } from "vue";
const props = defineProps<{
  nickname: string;
  userID: string;
  imgSrc: string;
  modelVale: string;
  onUpdateValue: (str: string) => void;
}>();
const textarea = ref("");
onMounted(() => {
  textarea.value = props.modelVale;
});
watch(textarea, (newVal) => {
  props.onUpdateValue(newVal);
});
</script>
<style lang="scss" scoped>
.text {
  font-size: 14px;
  margin-top: 28px;
  margin-bottom: 14px;
}
</style>
