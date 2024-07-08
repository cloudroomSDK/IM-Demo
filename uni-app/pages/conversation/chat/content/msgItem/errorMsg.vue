<template>
	<view class="msg-bg-con">
		<view>{{ $t("chat.unsupportiveMsg") }}</view>
	</view>
</template>

<script setup>
	import {
		ref
	} from "vue";

	const props = defineProps({
		msgItem: {
			type: Object,
			default: {}
		}
	});

	console.log(props.msgItem);
</script>

<style lang="scss" scoped>
</style>