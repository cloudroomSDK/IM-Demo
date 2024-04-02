<template>
  <el-avatar :size="size" :src="src">
    <img :src="type === 'user' ? defaultUserImg : defaultGroupImg" />
  </el-avatar>
</template>
<script setup lang="ts">
import defaultGroupImg from "~/assets/icons/default_group.svg";
import defaultUserImg from "~/assets/icons/default_user.svg";
withDefaults(
  defineProps<{
    type?: "user" | "group";
    src?: string;
    size?: number;
  }>(),
  {
    type: "user",
    src: "",
    size: 42,
  }
);
</script>
