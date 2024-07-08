<template>
	<view class="chat">
		<u-navbar :title="msgStore.currentConversation.showName" height="52px" autoBack border>
			<template #center>
				<view class="chat-title-center">
					<im-avatar :src="msgStore.currentConversation.faceURL" :size="36" :group="!isSingle"></im-avatar>
					<view class="name truncate">{{ msgStore.currentConversation.showName }} </view>
					<view class="member-count">{{ memberCount }} </view>
				</view>
			</template>
			<template #right>
				<u-icon name="more-dot-fill" size="30" color="#000000" @click="more" v-if="isInGrp"></u-icon>
			</template>
		</u-navbar>
		<v-chat-content ref="chatContent" />
		<v-chat-footer ref="chatFooter" @scrollToBottom="scrollToBottom" @onIsInGrp="onIsInGrp" />
	</view>
</template>

<script setup>
	import {
		onLoad,
		onUnload
	} from "@dcloudio/uni-app";
	import {
		ref,
		reactive,
		computed,
		onMounted
	} from "vue";
	import {
		appNavigator,
		effect,
		GlobalEvents
	}
	from "@/utils";
	import {
		GroupStatus,
		SessionType,
		GroupMemberRole
	} from "@cloudroom/crimsdk";
	import {
		storeToRefs
	} from "pinia";
	import useMsgStore from "@/store/msg";
	import useContacts from "@/store/contacts";
	import vChatContent from "./content";
	import vChatFooter from "./footer";

	const contactsStore = useContacts();
	const msgStore = useMsgStore();

	const {
		currentGrpInfo
	} = storeToRefs(contactsStore);

	const chatContent = ref(null);
	const chatFooter = ref(null);

	const isSingle = ref(true);
	const isInGrp = ref(true);
	const memberCount = computed(() => isSingle.value ? "" : ` (${currentGrpInfo.value.memberCount})`);
	const onIsInGrp = (val) => isInGrp.value = val;
	const scrollToBottom = () => {
		chatContent.value?.scrollToBottom();
	};

	const longPressAtUser = ({
		userID,
		nickname
	}) => {
		if (isSingle.value) return;
		effect.vibrateFeedback();
		chatFooter.value?.insertAt([{
			userID,
			nickname
		}]);
	};

	onLoad(async (params) => {
		isSingle.value = !params.groupID;
		contactsStore.initCurrentGrpAndMyMemInfo(params.groupID);
		await msgStore.initCurrentConversation(params);
		chatContent.value?.getMsgList(true);
		uni.$on(GlobalEvents.ScrollToBottom, scrollToBottom);
		uni.$on(GlobalEvents.AtUser, longPressAtUser);
	});

	const more = () => {
		const {
			groupID,
			userID
		} = msgStore.currentConversation;
		appNavigator.toChatSetting({
			userID,
			groupID
		});
	};

	onUnload(() => {
		uni.$on(GlobalEvents.ScrollToBottom, scrollToBottom);
		uni.$off(GlobalEvents.AtUser, longPressAtUser);
	});
</script>

<style lang="scss" scoped>
	.chat {
		@include pageView(52px);
		overflow: hidden;

		&-title-center {
			width: 100%;
			display: flex;
			align-items: center;
			flex: 1;
			padding: 0 50px;
			box-sizing: border-box;

			.name {
				padding-left: 10px;
			}
		}
	}

	.member-count {
		display: inline-block;
		width: 3em;
		padding-left: 5px;
	}
</style>