<template>
	<view class="im-empty">
		<u-empty :mode="mode" :text="text" :icon="icon" :width="size" :height="size" />
	</view>
</template>

<script setup name="imEmpty">
	import {
		ref
	} from "vue";
	const props = defineProps({
		mode: {
			type: String,
			default: "list"
		},
		text: {
			type: String,
			default: ""
		},
		icon: {
			type: String,
			default: ""
		},
		size: {
			type: [String, Number],
			default: 80
		}
	});
</script>

<style lang="scss">
	.im-empty {
		padding-top: 140px;
		box-sizing: border-box;
	}
</style>