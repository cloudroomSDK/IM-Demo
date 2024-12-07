<template>
  <img :src="src" />
</template>

<script setup lang="ts">
import { computed } from "vue";
import { getFaceUrl } from "~/utils/face";

const props = defineProps({
  value: {
    // 图片的unicode编码,这里和文件名一致
    type: String,
    required: true,
  },
  size: {
    type: Number,
    default: 16,
  },
});

const src = computed(() => getFaceUrl(props.value));
</script>
