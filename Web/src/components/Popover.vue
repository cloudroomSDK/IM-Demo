<template>
  <el-popover
    :offset="offset"
    :visible="visible"
    :placement="placement"
    :width="width"
    ref="popover"
  >
    <template #reference>
      <div @click.prevent.right="show" v-click-outside="onClickOutside">
        <slot />
      </div>
    </template>
    <ul class="popover-ul">
      <li v-for="item in list">
        <div @click="item.click">{{ item.text }}</div>
      </li>
    </ul>
  </el-popover>
</template>
<script lang="ts" setup>
import { ref } from "vue";
import { ClickOutside as vClickOutside } from "element-plus";
interface Data {
  text: "";
  click: Function;
}

interface Props {
  id?: any; //调用getData时将回传id
  data?: Data[];
  getData?: Function;
  offset?: number;
  width?: number;
  placement?:
    | "top"
    | "top-start"
    | "top-end"
    | "bottom"
    | "bottom-start"
    | "bottom-end"
    | "left"
    | "left-start"
    | "left-end"
    | "right"
    | "right-start"
    | "right-end";
}
const props = withDefaults(defineProps<Props>(), {
  width: 150,
  placement: "bottom",
});

const visible = ref(false);
const popover = ref();
const list = ref<Data[] | undefined>(props.data);
const show = () => {
  if (!props.data) {
    list.value = props.getData && props.getData(props.id);
  }
  visible.value = true;
};
const onClickOutside = () => {
  visible.value = false;
};
</script>
<style lang="scss" scoped>
.popover-ul {
  div {
    font-size: 14px;
    height: 40px;
    line-height: 40px;
    padding: 0 18px;
    border-radius: 4px;
    text-align: center;
    cursor: pointer;
    &:hover {
      background-color: #e9ebf4;
    }
  }
}
</style>
