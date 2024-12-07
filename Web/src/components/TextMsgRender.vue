<template>
  <span>
    <template v-for="item in props.arr">
      <FaceIcon v-if="item.type === 'face'" :value="item.value" class="icon" />
      <Nickname
        v-else-if="item.type === 'at'"
        class="at"
        :nickname="item.nickname"
        :userID="item.userID"
        showAt
      />
      <span v-else>{{ item.text }}</span>
    </template>
  </span>
</template>
<script lang="ts" setup>
import { FaceIcon, Nickname } from ".";

const props = defineProps<{
  arr: any[];
}>();
</script>
<style lang="scss" scoped>
.icon {
  margin: 0 .1em;
  width: 2em !important;
  height: 2em !important;
  vertical-align: -6px !important;
}
.at {
  padding: 0 .1em;
}
</style>
