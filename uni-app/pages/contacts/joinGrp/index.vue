<template>
	<view class="page-view-container">
		<u-navbar :title="$t('pages.joinGrp')" autoBack border></u-navbar>

		<view class="join-grp">
			<im-cell :title="$t('joinGrp.scan')" :label="$t('joinGrp.scanQrcode')" :avatar="images.scanGrp"
				shape="square" isLink @click="scanQrCodeResult">
			</im-cell>
			<im-cell :title="$t('joinGrp.joinById')" :label="$t('joinGrp.joinByIdDesc')" :avatar="images.ID"
				shape="square" isLink border @click="appNavigator.toJoinGrpById">
			</im-cell>
		</view>
	</view>
</template>

<script setup>
	import {
		ref
	} from "vue";
	import {
		appNavigator,
		images,
		scanQrCodeResult,
	} from "@/utils";
</script>

<style lang="scss" scoped>
	.join-grp {
		// padding: 0 16px;
		// box-sizing: border-box;
		background: #fff;
	}
</style>