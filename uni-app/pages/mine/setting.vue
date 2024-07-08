<template>
	<view class="page-view-container">
		<u-navbar :title="$t('pages.accountSetting')" autoBack border>
		</u-navbar>

		<view class="account-setting">
			<view class="account-setting-block">
				<u-cell :title="$t('acSet.allowBeep')">
					<template #value>
						<u-switch :activeValue="1" :inactiveValue="2" v-model="acSettingModel.allowBeep"
							@change="changeOpt($event,'allowBeep')"></u-switch>
					</template>
				</u-cell>
				<u-cell :title="$t('acSet.allowVibration')" :border="false">
					<template #value>
						<u-switch :activeValue="1" :inactiveValue="2" v-model="acSettingModel.allowVibration"
							@change="changeOpt($event,'allowVibration')"></u-switch>
					</template>
				</u-cell>
			</view>

			<view class="account-setting-block dnd">
				<u-cell :title="$t('acSet.dnd')" :border="false">
					<template #value>
						<u-switch :activeValue="2" :inactiveValue="0" v-model="acSettingModel.globalRecvMsgOpt"
							@change="changeOpt($event,'globalRecvMsgOpt')"></u-switch>
					</template>
				</u-cell>
			</view>
			<view class="dnd-desc">
				<text>{{ $t('acSet.dndHint') }}</text>
			</view>
			<view class="account-setting-block">
				<!-- 禁止添加好友 -->
				<!-- 		<u-cell :title="$t('acSet.allowAddFriend')">
					<template #value>
						<u-switch :activeValue="0" :inactiveValue="1" v-model="acSettingModel.allowAddFriend"
							@change="changeOpt($event,'allowAddFriend')"></u-switch>
					</template>
				</u-cell> -->
				<!-- 				
				<u-cell :title="$t('acSet.lang')" isLink :value="$t('acSet.followingSystem')"
					@click="switchLang"></u-cell> -->
				<u-cell :title="$t('pages.blacklist')" isLink @click="appNavigator.toBlacklist()"></u-cell>
				<u-cell :title="$t('acSet.clearChatHistory')" isLink @click="showClearCHModal = true">
				</u-cell>
			</view>
		</view>
		<u-modal :show="showClearCHModal" showCancelButton asyncClose @confirm="clearChatHistory"
			@cancel="showClearCHModal = false">
			<text class="modal-hint-text"> {{ $t('acSet.clearChatHistoryHint')  }}</text>
		</u-modal>
	</view>
</template>

<script setup>
	import {
		ref,
		reactive,
	} from "vue";
	import {
		appNavigator
	} from "@/utils";
	import api from "@/api";
	import crimsdk, {
		MessageReceiveOptType
	} from "@cloudroom/crimsdk";
	import useUserStore from "@/store/user";
	import useConversation from "@/store/conversation";
	import {
		storeToRefs
	} from "pinia";

	const userStore = useUserStore();
	const conversationStore = useConversation();

	const {
		userInfo
	} = storeToRefs(userStore);
	const {
		allowBeep,
		allowVibration,
		globalRecvMsgOpt,
		allowAddFriend
	} = userInfo.value;

	const acSettingModel = reactive({
		allowBeep: allowBeep,
		allowVibration: allowVibration,
		globalRecvMsgOpt: globalRecvMsgOpt,
		allowAddFriend: allowAddFriend,
	});

	const changeOpt = async (val, type) => {
		try {
			if (type === "globalRecvMsgOpt") {
				await crimsdk.setGlobalRecvMsgOpt(val);
				userInfo.value.globalRecvMsgOpt = val;
			} else {
				await userStore.setSelfUserInfo({
					[type]: val,
				});
				userInfo.value[type] = val;
			}
			uni.$u.$itoast("setSucc");
		} catch (e) {
			uni.$u.$itoast("setFail");
		}
	};

	const showClearCHModal = ref(false);
	const clearChatHistory = async () => {
		try {
			await conversationStore.clearChatHistory();
			showClearCHModal.value = false;
			uni.$u.$itoast("acSet.clearChatHistorySucc");
		} catch (e) {
			uni.$u.toast(`${uni.$u.$t("handleFail")}: ${e.stack}`);
		}
	};
	// const switchLang = () => {};
	// const setLang = (lang) => {
	// 	i18n.global.setLocaleMessage(lang);
	// }
</script>

<style lang="scss" scoped>
	.account-setting {

		&-block {
			padding: 0 15px;
			background-color: #fff;
		}

		.dnd {
			margin-top: 15px;
		}

		.dnd-desc {
			padding: 5px 15px;
			font-size: 12px;
			line-height: 2;
			color: #999999;
		}

		:deep(.u-cell) {
			.u-cell__body {
				height: 60px;
			}

			.u-cell__title-text {
				font-size: 16px;
				color: #000;
			}

			.u-cell__value {
				font-size: 16px;
				color: #3d3d3d;
			}
		}
	}
</style>