<template>
  <div class="wrap">
    <img v-if="chat.contentType === 102" :src="src" />
    <video v-if="chat.contentType === 104" :src="src" autoplay controls />
    <el-button class="button" type="primary" @click="downloadUrl(src)">
      下载
    </el-button>
  </div>
</template>
<script lang="ts" setup>
import { onBeforeMount, ref } from "vue";
import { IMTYPE } from "~/utils/imsdk";
import { downloadUrl } from "~/utils";

const props = defineProps<{
  chat: IMTYPE.MessageItem;
}>();

const src = ref("");
onBeforeMount(() => {
  switch (props.chat.contentType) {
    case 102:
      src.value = props.chat.pictureElem.sourcePicture.url;
      break;
    case 104:
      src.value = props.chat.videoElem.videoUrl;
      break;

    default:
      break;
  }
});
</script>
<style lang="scss" scoped>
.wrap {
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  max-width: 90vw;
  max-height: 90vh;
  img,
  video {
    max-width: 90vw;
    max-height: 70vh;
  }
  .button {
    margin-top: 20px;
    width: 200px;
  }
}
</style>
