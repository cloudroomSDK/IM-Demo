<template>
  <div class="imgContainer" @click="openSelectFile" v-if="!disabled">
    <Avatar :="$attrs" />
    <div class="zhezhao">
      <!-- @vue-ignore -->
      <el-icon class="icon" :size="$attrs.size / 2" color="#ffffff">
        <i-ep-edit />
      </el-icon>
    </div>
  </div>
  <Avatar v-bind="$attrs" v-else />
</template>
<script setup lang="ts">
import { Avatar } from ".";
import { getPicInfo, importFile } from "~/utils";
import { IMSDK } from "~/utils/imsdk";
import { v4 as uuidv4 } from "uuid";
import { ElMessage } from "element-plus";

withDefaults(
  defineProps<{
    disabled?: boolean;
  }>(),
  {
    disabled: true,
  }
);

const $emit = defineEmits(["updateUrl"]);

const openSelectFile = async () => {
  const [file] = await importFile({ accept: "image/*", multiple: false });
  if (file.size > 2 * 1024 * 1024) {
    return ElMessage.error("图片大小不能超过2MB");
  }
  const info = await getPicInfo(file);

  if (info.width > 512 || info.height > 512) {
    return ElMessage.error("图片分辨率不能超过512x512");
  }

  const { data } = await IMSDK.uploadFile({
    name: file.name,
    contentType: file.type,
    uuid: uuidv4(),
    file: file,
  });
  $emit("updateUrl", data.url);
};
</script>

<style lang="scss" scoped>
.imgContainer {
  position: relative;
  border-radius: 50%;
  overflow: hidden;
  font-size: 0;
  display: inline-block;

  .zhezhao {
    position: absolute;
    width: 100%;
    height: 100%;
    left: 0;
    top: 0;
    background-color: rgba(0, 0, 0, 0.5);
    opacity: 0;
    cursor: pointer;

    .icon {
      position: absolute;
      top: 50%;
      left: 50%;
      transform: translate(-50%, -50%);
    }
  }
  &:hover {
    .zhezhao {
      opacity: 1;
    }
  }
}
</style>
