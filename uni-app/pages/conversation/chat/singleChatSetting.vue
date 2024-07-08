<template>
	<view class="page-view-container common-cells">
		<u-navbar :title="$t('pages.chatSetting')" autoBack border />

		<view class="block-container members">
			<u-grid col="5">
				<u-grid-item @click="toUserCard">
					<im-avatar :size="52" :src="infomation.faceURL"></im-avatar>
					<view class="member-name truncate">{{ infomation.nickname }}</view>
				</u-grid-item>
				<u-grid-item @click="createGrp">
					<im-avatar :size="52" :src="images.add"></im-avatar>
					<text class="member-name">{{ $t('createGrp.add') }}</text>
				</u-grid-item>
			</u-grid>
		</view>

		<view class="block-container">
			<view class="block-container-box">
				<u-cell :title="$t('chatSetting.searchChatHistory')" isLink @click="toSearchChatHistory">
				</u-cell>
			</view>
		</view>

		<view class="block-container">
			<view class="block-container-box">
				<!-- 置顶聊天 -->
				<u-cell :title="$t('chatSetting.pinned')">
					<template #value>
						<u-switch v-model="conversation.isPinned" @change="changeOpt($event,'pinned')"></u-switch>
					</template>
				</u-cell>
				<!-- 消息免打扰 -->
				<u-cell :title="$t('chatSetting.dnd')">
					<template #value>
						<u-switch v-model="recvMsgOpt" @change="changeOpt($event,'recvMsgOpt')"></u-switch>
					</template>
				</u-cell>
				<!-- 阅后即焚 -->
				<u-cell :title="$t('chatSetting.snapchat')">
					<template #value>
						<u-switch v-model="conversation.isPrivateChat"
							@change="changeOpt($event,'snapchat')"></u-switch>
					</template>
				</u-cell>
			</view>
		</view>

		<view class="block-container">
			<view class="block-container-box">
				<u-cell :title="$t('chatSetting.setChatBackground')" isLink @click="toSetChatBackground">
				</u-cell>
				<!-- <u-cell :title="$t('chatSetting.setFontSize')" isLink @click="toSetFontSize"></u-cell> -->
			</view>
		</view>

		<view class="block-container">
			<view class="block-container-box">
				<u-cell :title="$t('chatSetting.clearChatHistory')" isLink @click="showClearCHModal = true">
				</u-cell>
			</view>
		</view>

		<u-modal :show="showClearCHModal" showCancelButton asyncClose @confirm="clearChatHistory"
			@cancel="showClearCHModal = false">
			<text> {{clearChatHistoryHint }}</text>
		</u-modal>
	</view>
</template>

<script setup>
	import {
		onLoad
	} from "@dcloudio/uni-app";
	import {
		ref,
		reactive,
		computed
	} from "vue";
	import api from "@/api";
	import crimsdk, {
		SessionType,
		MessageReceiveOptType
	} from "@cloudroom/crimsdk";
	import {
		appNavigator,
		images
	} from "@/utils";
	import {
		storeToRefs
	} from "pinia";
	import useConversation from "@/store/conversation";
	import useMsgStore from "@/store/msg";

	const conversationStore = useConversation();
	const msgStore = useMsgStore();

	onLoad(({
		userID
	}) => {
		// conversationStore.getCurrentConversation({ userID });
		getUserInfomation(userID);
	});

	const {
		currentConversation: conversation
	} = storeToRefs(conversationStore);

	const infomation = reactive({
		userID: "",
		faceURL: "",
		nickname: "",
	});

	const recvMsgOpt = computed({
		get() {
			return conversation.value.recvMsgOpt !== MessageReceiveOptType.Nomal;
		},
		set(isDnd) {
			conversation.value.recvMsgOpt = isDnd ? MessageReceiveOptType.NotNotify : MessageReceiveOptType
				.Nomal;
		}
	});

	const clearChatHistoryHint = computed(() => uni.$u.$fprint('chatSetting.clearChatHistoryHint', [infomation
		.nickname
	]));

	const getUserInfomation = async (userid) => {
		try {
			const {
				users
			} = await api.searchUserInfo(userid);
			const {
				data
			} = await crimsdk.getUsersInfo([userid]);
			Object.assign(infomation, data[0], users[0]);
		} catch (e) {
			console.log(e);
		}
	}

	const changeOpt = async (value, type) => {
		console.log(type, value);
		const {
			conversationID
		} = conversation.value;

		if (type === "pinned") {
			try {
				// 是否置顶
				await crimsdk.pinConversation({
					conversationID,
					isPinned: value
				});
			} catch (e) {
				conversation.value.isPinned = !value;
			}
		} else if (type === "recvMsgOpt") {
			try {
				// 是否勿扰
				const receiveOptType = value ?
					MessageReceiveOptType.NotNotify : MessageReceiveOptType.Nomal;
				await crimsdk.setConversationRecvMsgOpt({
					conversationID,
					opt: receiveOptType
				});
			} catch (e) {}
		} else if (type === "snapchat") {
			try {
				// 是否"阅后即焚"
				await crimsdk.setConversationPrivateChat({
					conversationID,
					isPrivate: value
				});
			} catch (e) {
				conversation.value.isPrivate = !value;
			}
		}
	}

	const toUserCard = () => {
		appNavigator.toFriCard({
			userID: infomation.userID
		});
	};

	const createGrp = () => {
		appNavigator.toCreateGrp({
			data: JSON.stringify([{
				userID: infomation.userID,
				nickname: infomation.nickname,
				faceURL: infomation.faceURL
			}]),
		});
	}

	const toSetChatBackground = () => {
		appNavigator.toSetChatBackground();
	}

	const toSearchChatHistory = () => {
		appNavigator.toSearchChatHistory({
			conversationID: conversation.value.conversationID,
		});
	}

	const showClearCHModal = ref(false);
	const clearChatHistory = async () => {
		try {
			await msgStore.clearConversationAndDeleteAllMsg(conversation.value.conversationID);
			showClearCHModal.value = false;
			uni.$u.$itoast("chatSetting.clearChatHistorySucc");
		} catch (e) {
			uni.$u.toast(`${uni.$u.$t("handleFail")}: ${e.stack}`);
		}
	};
</script>

<style lang="scss" scoped>
	.members {
		padding: 15px;
		box-sizing: border-box;
		background-color: #fff;
	}

	.member-name {
		width: 100%;
		font-size: 12px;
		color: #333;
		margin-top: 8px;
		padding: 0 4px;
		box-sizing: border-box;
		text-align: center;
	}
</style>