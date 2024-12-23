<template>
  <div
    class="item"
    :style="{
      cursor: type === 'btn' ? 'pointer' : '',
      height: height + 'px',
    }"
  >
    <div class="key">
      <Avatar class="img" v-if="iconUrl" :src="iconUrl" :size="30" />
      <span class="text"> {{ text }} </span>
    </div>
    <div class="value">
      <div v-if="type === 'text'" class="text">{{ value }}</div>

      <el-switch
        v-else-if="type === 'switch'"
        v-model="model"
        @change="emit('change', $event)"
      />
      <el-select
        v-else-if="type === 'select'"
        v-model="model"
        :style="`width: ${selectWidth}px`"
        @change="emit('change', $event)"
      >
        <el-option
          v-for="item in options"
          :key="item.value"
          :label="item.text"
          :value="item.value"
        />
      </el-select>
      <el-icon class="icon" v-if="showArrow"><i-ep-arrowRight /></el-icon>
      <el-popover
        v-if="canCopy"
        placement="bottom"
        :width="150"
        :visible="copyToast"
        effect="dark"
        content="已复制"
      >
        <template #reference>
          <el-icon @click="copy" class="icon"> <i-ep-documentCopy /></el-icon>
        </template>
      </el-popover>

      <el-icon @click="edit" class="icon" v-if="canEdit"><i-ep-edit /></el-icon>
    </div>
  </div>
</template>
<script lang="ts" setup>
import { ElMessageBox } from "element-plus";
import { ref } from "vue";
import { copyToClipboard } from "~/utils";
import { Avatar } from ".";
const model = defineModel<boolean | number>();
const emit = defineEmits(["change", "updateValue"]);

interface Option {
  text: string;
  value: number | string;
}
interface Props {
  type?: "text" | "switch" | "btn" | "showIcon" | "select";
  height?: number;
  text: string;
  value?: string | boolean | number;
  canEdit?: boolean;
  canCopy?: boolean;
  showArrow?: boolean;
  iconUrl?: string;
  options?: Option[];
  selectWidth?: number;
}
const props = withDefaults(defineProps<Props>(), {
  type: "text",
  height: 44,
  canEdit: false,
  canCopy: false,
  showArrow: false,
  selectWidth: 90,
});

let copyToast = ref(false);

const edit = async () => {
  try {
    const { value } = await ElMessageBox.prompt(
      "输入新的群名称",
      "修改群名称",
      {
        confirmButtonText: "确定",
        cancelButtonText: "取消",
      }
    );
    emit("updateValue", value);
  } catch (error) {}
};
const copy = async () => {
  await copyToClipboard(props.value as string);
  copyToast.value = true;
  setTimeout(() => {
    copyToast.value = false;
  }, 1000);
};
</script>
<style lang="scss" scoped>
.item {
  display: flex;
  padding: 0 30px;
  justify-content: space-between;
  align-items: center;
  font-size: 14px;
  height: 50px;
  line-height: 50px;
  color: #333;
  border-radius: 4px;
  &:hover {
    background-color: #f5f5f5;
  }
  .key {
    display: flex;
    justify-content: center;
    align-items: center;
    overflow: hidden;
    .text {
      color: #000;
      text-wrap: nowrap;
      overflow: hidden;
      flex: 1;
      text-overflow: ellipsis;
    }
    .img {
      margin-right: 10px;
    }
  }
  .value {
    display: flex;
    align-items: center;
    justify-content: right;
    flex: 1;
    width: 0;
    text-align: right;
    margin-left: 20px;
    .text {
      color: #000;
      text-wrap: nowrap;
      overflow: hidden;
      flex: 1;
      text-overflow: ellipsis;
    }
    .icon {
      margin-left: 4px;
      cursor: pointer;
    }
  }
}
</style>
