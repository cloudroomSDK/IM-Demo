<template>
	<view class="mine bottom-tabbar">
		<view class="mine-header flex flex-items-end">
			<view class="mine-info flex-1 flex flex-items-center">
				<view class="mine-info-avatar">
					<im-avatar :src="userStore.faceURL" :size="80"></im-avatar>
				</view>
				<view class="mine-info-nickname flex-1">
					<view class="name truncate">{{ userStore.nickname }}</view>
					<view class="uid truncate">ID：{{ userStore.userID }}</view>
				</view>
				<view class="mine-info-qrcode">
					<image src="@/static/images/qrcode.png" @click="appNavigator.toMyQrcode"></image>
				</view>
			</view>
		</view>

		<view class="mine-functions">
			<u-cell :title="$t('pages.myInfo')" isLink @click="appNavigator.toMyInfo"></u-cell>
			<u-cell :title="$t('pages.accountSet')" isLink @click="appNavigator.toSetting"></u-cell>
			<u-cell :title="$t('pages.about')" isLink @click="appNavigator.toAbout"></u-cell>
			<u-cell :title="$t('mine.logout')" isLink @click="showLogoutModal"></u-cell>
		</view>

		<im-tabbar />

		<u-modal :show="show" showCancelButton asyncClose @confirm="logout" @cancel="closeLogoutModal">
			<text class="lougout-text">{{$t("mine.logoutHint")}}</text>
		</u-modal>
	</view>
</template>

<script setup>
	import {
		ref,
		reactive
	} from "vue";
	import {
		appNavigator
	} from "@/utils";
	import appStorage from "@/common/storage";
	import useUserStore from "@/store/user";
	import crimsdk from "@cloudroom/crimsdk";

	const userStore = useUserStore();

	// 退出登录
	const show = ref(false);
	const showLogoutModal = () => {
		// uni.hideTabBar({
		// 	success() {
		// 		show.value = true;
		// 	}
		// });
		show.value = true;
	};
	const closeLogoutModal = () => {
		show.value = false;
		// uni.showTabBar();
	};
	const logout = async () => {
		await crimsdk.logout();
		show.value = false;
		// uni.showTabBar();
		appStorage.rmLoginResult();
		appNavigator.toLogin();
	};
</script>

<style lang="scss" scoped>
	.mine {
		@include pageView(0);

		&-header {
			height: 210px;
			padding: 0 16px;
			border-radius: 16px 16px 0 0;
			background-color: #5495E3;
		}

		&-info {
			height: 140px;
			padding: 0 18px;
			border-radius: 16px 16px 0 0;
			background-color: #fff;
			overflow: hidden;

			&-avatar {
				image {
					height: 80px;
					width: 80px;
				}
			}

			&-nickname {
				line-height: 1.416666;
				padding: 0 16px;
				overflow: hidden;

				.name {
					font-size: 24px;
					font-weight: 500;
				}

				.uid {
					font-size: 15px;
					font-weight: 300;
				}
			}

			&-qrcode {
				image {
					height: 30px;
					width: 30px;
				}
			}
		}

		&-functions {
			padding: 0 16px;

			:deep(.u-cell) {
				// padding: 0 16px;

				.u-cell__body {
					padding: 15px 15px 15px 21px;
				}

				.u-cell__title-text {
					font-size: 16px;
					font-weight: 400;
					color: #000;
				}
			}
		}

		.lougout-text {
			padding: 10px 0;
			font-size: 16px;
			font-weight: 400;
			color: #333;
		}
	}
</style>