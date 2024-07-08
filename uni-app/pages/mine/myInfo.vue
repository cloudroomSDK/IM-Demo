<template>
	<view class="page-view-container">
		<u-navbar :title="$t('pages.myInfo')" autoBack border>
		</u-navbar>
		<view class="myinfo">
			<u-cell :title="$t('user.avatar')" isLink @click="selAvatarVisible = true">
				<template #value>
					<im-avatar class="avatar" :src="userInfo.faceURL"></im-avatar>
				</template>
			</u-cell>
			<u-cell :title="$t('user.nickname')" isLink :value="userInfo.nickname"
				@click="appNavigator.toMyInfoDetail({'type': 1})"></u-cell>
			<u-cell :title="$t('user.sex')" isLink :value="sex" @click="sexVisible = true"></u-cell>
			<u-cell :title="$t('user.birthday')" isLink :value="birth" @click="birthdayVisible = true"></u-cell>
			<u-cell :title="$t('user.phoneNumber')" :value="userInfo.phoneNumber"></u-cell>
			<u-cell :title="$t('user.qrcode')" isLink @click="appNavigator.toMyQrcode">
				<template #value>
					<image class="qrcode" src="@/static/images/qrcode.png"></image>
				</template>
			</u-cell>
			<u-cell :title="$t('user.idNum')" isLink :border="false" @click="toImCopyId"></u-cell>
		</view>
		<u-action-sheet :actions="avatarChoose" closeOnClickOverlay closeOnClickAction :show="selAvatarVisible"
			:round="16" :cancelText="$t('cancel')" @select="selAvatar"
			@close="selAvatarVisible = false"></u-action-sheet>
		<u-action-sheet :actions="sexChoose" closeOnClickOverlay closeOnClickAction :show="sexVisible" :round="16"
			:cancelText="$t('cancel')" @select="selSex" @close="sexVisible = false"></u-action-sheet>
		<u-datetime-picker :show="birthdayVisible" v-model="birthday" mode="date" :minDate="0"
			:formatter="uViewFormatterCn" @confirm="confirmBirthday" @cancel="birthdayVisible = false"
			closeOnClickOverlay></u-datetime-picker>
	</view>
</template>

<script setup>
	import {
		ref,
		reactive,
		computed,
	} from "vue";
	import {
		appNavigator,
		uViewFormatterCn,
		getPurePath
	} from '@/utils';
	import {
		v4 as uuidv4
	} from "uuid";
	import {
		chooseImage
	} from '@/utils/uniUtils';
	import api from "@/api";
	import crimsdk from "@cloudroom/crimsdk";
	import useUserStore from "@/store/user";
	import {
		storeToRefs
	} from "pinia";

	const userStore = useUserStore();
	const {
		userInfo
	} = storeToRefs(userStore);

	const avatarChoose = [{
		name: uni.$u.$t("album"),
		value: "album",
	}, {
		name: uni.$u.$t("photograph"),
		value: "camera",
	}];
	const selAvatarVisible = ref(false);
	const selAvatar = async ({
		value
	}) => {
		try {
			const paths = await chooseImage([value], 1);
			const path = paths[0];
			const fileName = path.slice(path.lastIndexOf("/") + 1);
			const fileType = path.slice(path.lastIndexOf(".") + 1);
			const {
				data: {
					url
				}
			} = await crimsdk.uploadFile({
				name: fileName,
				contentType: fileType,
				uuid: uuidv4(),
				filepath: getPurePath(path),
			});

			await userStore.setSelfUserInfo({
				faceUrl: url,
			});
		} catch (e) {
			uni.$u.$itoast("uploadFail");
		}
	};

	const sexChoose = [{
		name: uni.$u.$t("user.man"),
		value: 1,
	}, {
		name: uni.$u.$t("user.woman"),
		value: 2,
	}];
	const sexVisible = ref(false);
	const sex = computed(() => userInfo.value.gender === 1 ? uni.$u.$t("user.man") : userInfo.value.gender === 2 ? uni.$u
		.$t("user.woman") : "-");
	const selSex = async ({
		value
	}) => {
		await userStore.setSelfUserInfo({
			gender: value,
		});
	};
	const birthdayVisible = ref(false);
	const birthday = ref(userInfo.value.birth);
	const birth = computed(() => uni.$u.timeFormat(birthday.value));
	const confirmBirthday = async ({
		value
	}) => {
		try {
			await userStore.setSelfUserInfo({
				birth: value,
			});
			birthdayVisible.value = false;
		} catch (e) {
			birthday.value = userInfo.value.birth;
		}
	}

	const toImCopyId = () => {
		appNavigator.toImCopyId({
			userID: userInfo.value.userID
		});
	};
</script>

<style lang="scss" scoped>
	.myinfo {
		padding: 0 15px;
		border-bottom: 1px solid $uni-border-color;
		background-color: #fff;

		:deep(.u-cell) {
			// padding: 20px 0 20px 15px;
			overflow: hidden;

			.u-cell__body {
				height: 60px;
			}

			.u-cell__title {
				overflow: hidden;
				white-space: nowrap;
				text-overflow: ellipsis;
			}

			.u-cell__title-text {
				font-size: 16px;
				color: #000;
			}

			.u-cell__value {
				flex: 1;
				font-size: 16px;
				color: #3d3d3d;
				overflow: hidden;
				white-space: nowrap;
				text-overflow: ellipsis;
			}

			.u-cell__right-icon-wrap {
				margin-left: 14px;
			}
		}

		:deep(.u-action-sheet__cancel-text) {
			font-size: 16px;
			color: #000000;
		}

		.avatar {
			width: 40px;
			height: 40px;
		}

		.qrcode {
			width: 30px;
			height: 30px;
		}
	}
</style>