<template>
	<view class="page-view-container common-cells">
		<u-navbar :title="$t('pages.setChatBackground')" autoBack border>
			<template #right>
				<text class="main-color" @click="restore">{{ $t("setChatBackground.restore")}}</text>
			</template>
		</u-navbar>

		<view class="block-container">
			<view class="block-container-box">
				<u-cell :title="$t('album1')" isLink @click="clickChooseImage('album')">
				</u-cell>
				<u-cell :title="$t('photograph')" isLink @click="clickChooseImage('camera')">
				</u-cell>
			</view>
		</view>
	</view>
</template>

<script setup>
	import {
		onLoad
	} from "@dcloudio/uni-app";
	import {
		ref,
		computed
	} from "vue";
	import {
		chooseImage
	} from '@/utils/uniUtils';

	const clickChooseImage = async (type) => {
		const paths = await chooseImage([type], 1);
		const path = paths[0];
		uni.showToast({
			title: path
		});
	};

	const restore = async () => {
		uni.showToast({
			title: "功能未开发"
		});
	};
</script>

<style>
</style>