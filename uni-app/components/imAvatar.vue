<template>
	<u-avatar :shape="shape" :src="src" :size="size" :default-url="group ? images.head : images.myFri"></u-avatar>
</template>

<script setup name="imAvatar">
	import {
		ref
	} from "vue";
	import {
		images
	} from "@/utils";
	const props = defineProps({
		src: {
			type: String,
			default: images.head
		},
		size: {
			type: [String, Number],
			default: 40
		},
		group: {
			type: Boolean,
			default: false,
		},
		shape: {
			type: String,
			default: "circle"
		},
	});
</script>

<style lang="scss">
</style>