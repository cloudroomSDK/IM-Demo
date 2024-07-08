<template>
	<view class="page-view-container">
		<u-navbar :title="title" autoBack border>
			<template #right>
				<text v-if="isChangeNickname" class="main-color" @click="saveNickname">{{ $t("save") }}</text>
			</template>
		</u-navbar>
		<view class="bg-witle">
			<u-cell :title="userInfo.value.userID" v-if="isCopyId">
				<template #value>
					<u-button class="copy-btn" type="primary" :text="$t('copy')" @click="copyUserID"></u-button>
				</template>
			</u-cell>

			<view class="change-nickname" v-else>
				<u-input v-model="nickname" border="none" :maxlength="50" clearable>
				</u-input>
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
		computed,
	} from "vue";
	import api from "@/api";
	import useUserStore from "@/store/user";
	import {
		storeToRefs
	} from "pinia";
	import {
		appNavigator
	} from "@/utils";

	const userStore = useUserStore();
	const {
		userInfo
	} = storeToRefs(userStore);
	const type = ref("0");
	const title = computed(() => type.value === "0" ? uni.$u.$t("user.idNum") : uni.$u.$t("user.nickname"));
	const isCopyId = computed(() => type.value === "0");
	const isChangeNickname = computed(() => type.value === "1");
	onLoad((params) => {
		type.value = params.type;
	});
	const copyUserID = () => {
		uni.setClipboardData({
			showToast: false,
			data: userInfo.value.userID,
			success: function() {
				uni.$u.$itoast("copySucc");
			},
		});
	};

	const nickname = ref(userInfo.value.nickname);
	const saveNickname = async () => {
		try {
			await userStore.setSelfUserInfo({
				nickname: nickname.value,
			});
			userInfo.value.nickname = nickname.value;
			appNavigator.back();
		} catch (e) {
			nickname.value = userInfo.value.nickname;
			uni.$u.$itoast("saveFail");
		}
	};
</script>

<style lang="scss" scoped>
	.copy-btn {
		width: 64px;
		height: 30px;
		line-height: 30px;
	}

	.change-nickname {
		display: flex;
		height: 60px;
		padding: 0 15px;
		border-bottom: 1px solid $uni-border-color;
	}

	:deep(.u-cell) {
		.u-cell__body {
			padding: 10px 15px 10px 36px;
		}

		.u-cell__title-text {
			font-size: 16px;
			font-weight: 400;
			color: #000;
		}
	}
</style>