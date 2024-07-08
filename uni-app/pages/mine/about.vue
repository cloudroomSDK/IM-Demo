<template>
	<view class="page-view">
		<u-navbar :title="$t('pages.about')" autoBack border>
		</u-navbar>
		<view class="about">
			<image src="@/static/images/logo.png"></image>
			<view class="version">{{ $t("about.ver") }} {{ version }}</view>
		</view>
	</view>
</template>

<script setup>
	import {
		ref
	} from 'vue';
	const version = ref("");
	plus.runtime.getProperty(plus.runtime.appid, (inf) => {
		version.value = inf.version;
	});
</script>

<style lang="scss" scoped>
	.about {
		padding-top: 86px;
		text-align: center;


		image {
			width: 120px;
			height: 120px;
			border-radius: 10px;
			box-shadow: 0px 3px 10px 0px rgba(0, 0, 0, 0.1);
		}

		.version {
			margin-top: 18px;
			font-size: 16px;
			line-height: 1.5;
			font-weight: 400;
			color: #3D3D3D;
		}
	}
</style>