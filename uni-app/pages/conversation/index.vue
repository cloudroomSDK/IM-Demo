<template>
	<view class="conversation bottom-tabbar">
		<u-navbar height="64px" leftIcon="">
			<template #left>
				<im-avatar :src="userStore.faceURL" :size="40" @click.native="appNavigator.toMyInfo"></im-avatar>
			</template>
			<template #center>
				<view class="online-status">
					<view class="nickname truncate">
						{{ userStore.nickname }}
					</view>
					<view class="status">
						<view class="status-icon online"></view>{{ $t("conversation.online") }}
					</view>
				</view>
			</template>
			<template #right>
				<im-menus />
			</template>
		</u-navbar>
		<view class="search-container">
			<u-search shape="square" :placeholder="$t('search')" :showAction="false" disabled
				@click="appNavigator.toSearch"></u-search>
		</view>
		<view class="flex-1 overflow-hidden">
			<im-empty v-if="isEmpty" :text="$t('noConversationData')" :icon="images.msgEmpty" />
			<im-scroll-view :auto-load="!conversationStore.syncServer" :refresh="refresh" :load="load"
				:disabled="conversationListIsNoData" v-else>
				<u-swipe-action>
					<v-conversation-item :item="item" v-for="item in conversationList"
						:key="`${item.conversationID}${item.isPinned}${item.unreadCount === 0}`"></v-conversation-item>
				</u-swipe-action>
			</im-scroll-view>
		</view>

		<im-tabbar />
	</view>
</template>

<script setup>
	import {
		onShow,
	} from "@dcloudio/uni-app";
	import {
		computed,
		ref,
	} from "vue";
	import {
		appNavigator,
		images,
		parseMessageByType,
		formatConversionTime
	} from "@/utils";
	import useUserStore from "@/store/user";
	import useConversation from "@/store/conversation";
	import {
		storeToRefs
	} from "pinia";
	import vConversationItem from "./component/conversationItem";

	const userStore = useUserStore();
	const conversationStore = useConversation();

	const {
		conversationList,
		conversationListIsNoData
	} = storeToRefs(conversationStore);

	let isFristLoad = true;
	const isEmpty = computed(() => conversationList.value.length === 0 && (!isFristLoad || conversationListIsNoData
		.value));

	const refresh = async () => await conversationStore.getConversationListSplit(true);
	const load = async () => {
		await conversationStore.getConversationListSplit();
		isFristLoad = false;
	}
</script>

<style lang="scss" scoped>
	.conversation {
		@include pageView(64px, 48px);

		.online-status {
			width: 100%;
			display: flex;
			flex-direction: column;
			flex: 1;
			padding: 0 70px;
			box-sizing: border-box;

			.nickname {
				font-size: 18px;
				color: #3d3d3d;
			}

			.status {
				font-size: 14px;
				color: #3d3d3d;
			}

			.status-icon {
				display: inline-block;
				width: 8px;
				height: 8px;
				border-radius: 4px;
				margin-right: 4px;

				&.online {
					background: #38FF26;
				}

				&.offline {
					background: #FF0000;
				}
			}

		}

		.search-container {
			padding-top: 4px !important;
		}
	}
</style>