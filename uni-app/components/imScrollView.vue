<template>
	<scroll-view class="scroll-view" scroll-y="true" refresher-enabled="true" :scroll-top="scrollTop"
		:scroll-with-animation="true" :refresher-triggered="triggered" @refresherrefresh="onRefresh"
		@refresherrestore="onRestore" @scroll="scroll" @scrolltolower="loadData">
		<view class="scroll-container">
			<slot></slot>
		</view>
	</scroll-view>
</template>

<script setup name="imScrollView">
	import {
		ref
	} from "vue";

	const props = defineProps({
		autoLoad: {
			type: Boolean,
			default: false
		},
		disabled: {
			type: Boolean,
			default: false
		},
		// load需要是Promise<boolean>
		load: {
			// type: Promise,
			required: true,
		},
		// refresh需要是Promise
		refresh: {
			required: true,
		}
	});
	const emits = defineEmits([""]);

	const triggered = ref(false);
	const scrollTop = ref(0);
	const scroll = (e) => {
		e.preventDefault();
	};

	const getElRect = (el) => {
		return new Promise((resolve) => {
			uni.createSelectorQuery()
				.select(el)
				.boundingClientRect(resolve)
				.exec();
		});
	};
	// :refresher-threshold="threshold" 默认45
	const isContinueLoad = async () => {
		if (!props.disabled) {
			const parentNode = await getElRect(".scroll-view");
			const childNode = await getElRect(".scroll-container");
			return parentNode.height > childNode.height;
		}
		return false;
	}

	const onRefresh = async () => {
		// console.log("onRefresh");
		triggered.value = true;
		await props.refresh();
		await new Promise(resolve => setTimeout(resolve, 1000));
		triggered.value = false;
	};

	const onRestore = async () => {
		// 刷新回弹完成
		// console.log("onRestore");
		if (await isContinueLoad()) {
			loadData();
		}
	};
	const loadData = async () => {
		// console.log("loadData");
		if (!props.disabled) {
			await props.load();
			if (await isContinueLoad()) {
				loadData();
			}
		}
	};

	props.autoLoad && loadData();
</script>

<style lang="scss" scoped>
	.scroll-view {
		height: 100%;
	}

	.scroll-container {}
</style>