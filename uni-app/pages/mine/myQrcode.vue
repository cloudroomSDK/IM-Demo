<template>
	<view class="page-view">
		<u-navbar :title="$t('pages.myQrcode')" autoBack border>
		</u-navbar>
		<view class="my-qrcode">
			<view class="info flex">
				<view class="avatar">
					<im-avatar :src="userStore.faceURL"></im-avatar>
				</view>
				<view class="nickname truncate flex-1 flex-self-center">
					{{ userStore.nickname }}
				</view>
			</view>

			<view class="qrcode">
				<uqrcode canvas-id="myqrcode" :value="qrcodeVal" :size="200" :options="{ margin: 10 }">
				</uqrcode>
			</view>

			<view class="desc">
				<text>{{ $t('myQrcode.desc') }}</text>
			</view>
		</view>
	</view>
</template>

<script setup>
	import {
		ref,
		reactive
	} from "vue";
	import {
		getQrCodeLink
	} from "@/utils";
	import useUserStore from "@/store/user";

	const userStore = useUserStore();

	const qrcodeVal = ref(getQrCodeLink(userStore.userInfo));
</script>

<style lang="scss" scoped>
	.my-qrcode {
		padding: 64px 40px 0;

		.avatar {
			image {
				height: 80px;
				width: 80px;
			}
		}

		.nickname {
			padding: 0 16px;
			font-size: 24px;
			font-weight: 500;
		}

		.qrcode {
			width: 200px;
			height: 200px;
			margin: 40px auto 55px;
		}

		.desc {
			font-size: 16px;
			font-weight: 400;
			color: #666666;
		}
	}
</style>