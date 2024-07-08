<template>
	<u-swipe-action-item :options="options" @click="action" :threshold="60" :name="item.conversationID">
		<u-cell class="conversation-item" :class="{'is-pinned': item.isPinned}" :border="false" @click="toChat(item)">
			<template #icon>
				<im-avatar :size="60" :src="item.faceURL" :group="!!item.groupID"></im-avatar>
			</template>
			<template #title>
				<view class="nickname truncate">{{ item.showName }}</view>
				<view class="lastmsg truncate">{{ parseConversionMsg(item) }}</view>
			</template>
			<template #value>
				<view class="text-right">
					<view class="time">{{ formatConversionTime(item.latestMsgSendTime) }}</view>
					<view class="badge">
						<view class="nobell" v-if="item.recvMsgOpt === MessageReceiveOptType.NotNotify">
							<image :src="images.noBell"></image>
						</view>
						<u-badge max="99" :value="item.unreadCount" v-else></u-badge>
					</view>
				</view>
			</template>
		</u-cell>
	</u-swipe-action-item>
</template>

<script setup name="conversationItem">
	import {
		ref,
		computed,
	} from "vue";
	import {
		appNavigator,
		images,
		parseMessageByType,
		formatConversionTime
	} from "@/utils";
	import useConversation from "@/store/conversation";
	import crimsdk, {
		MessageReceiveOptType,
		GroupAtType,
		SessionType,
	} from "@cloudroom/crimsdk";

	const conversationStore = useConversation();

	const props = defineProps({
		item: {
			type: Object,
			default: ""
		},
	});

	const toChat = async (item) => {
		conversationStore.markConversationMsgAsRead(item.conversationID);
		appNavigator.toChat(item);
	}

	const messagePrefix = (item) => {
		if (item.draftText !== "") {
			let text = item.draftText;
			return "[草稿]";
		}
		let prefix = "";

		if (
			item?.recvMsgOpt !== MessageReceiveOptType.Nomal &&
			item.unreadCount > 0
		) {
			prefix = `[${item.unreadCount}条] `;
		}

		if (item.groupAtType !== GroupAtType.AtNormal) {
			switch (item.groupAtType) {
				case GroupAtType.AtAll:
					prefix = "[所有人]";
					break;
				case GroupAtType.AtMe:
					prefix = "[有人@你]";
					break;
				case GroupAtType.AtAllAtMe:
					prefix = "[有人@你]";
					break;
				case GroupAtType.AtGroupNotice:
					prefix = "[群公告]";
					break;
			}
		}

		return prefix;
	};

	const parseConversionMsg = (item) => {
		const prefix = messagePrefix(item);
		let msgCtx = "";
		try {
			const msg = JSON.parse(item.latestMsg);
			msgCtx = parseMessageByType(msg);
		} catch (e) {
			console.log("parse ConversionMsg err: ", e);
		}
		return `${prefix}${msgCtx} `;
	}

	const options = computed(() => {
		const opts = [{
			text: props.item.isPinned ? uni.$u.$t("conversation.untop") : uni.$u.$t("conversation.top"),
			style: {
				backgroundColor: '#3981FC'
			},
			idx: 0,
		}];
		if (props.item.unreadCount > 0) {
			opts.push({
				text: uni.$u.$t("conversation.markRead"),
				style: {
					backgroundColor: '#FFBD4C'
				},
				idx: 1,
			});
		} else {
			opts.push({
				text: uni.$u.$t("conversation.hide"),
				style: {
					backgroundColor: '#666666'
				},
				idx: 3,
			});
		}
		opts.push({
			text: uni.$u.$t("conversation.remove"),
			style: {
				backgroundColor: '#FF6A6A'
			},
			idx: 2,
		});
		return opts;
	});

	const emits = defineEmits(["closeAllAction"]);
	const action = ({
		index,
		name
	}) => {
		// uviewplus 没暴露关闭action（expose: ['closeHandler']），改了源码手动调的
		const idx = options.value[index].idx;
		const {
			conversationID,
			isPinned
		} = props.item;
		switch (idx) {
			case 0:
				crimsdk.pinConversation({
					conversationID,
					isPinned: !isPinned
				});
				break;
			case 1:
				conversationStore.markConversationMsgAsRead(conversationID);
				break;
			case 2:
				conversationStore.deleteConversationAndDeleteAllMsg(conversationID);
				break;
			case 3:
				conversationStore.hideConversation(conversationID);
				break;
		}
	};
</script>

<style lang="scss" scoped>
	.is-pinned {
		background-color: #f2f2f2;
	}

	.conversation-item {
		height: 76px;
		padding: 0 16px;
		box-sizing: border-box;

		.nickname {
			font-size: 16px;
			color: #000000;
			height: 22px;
			line-height: 22px;
		}

		.lastmsg,
		.time {
			font-size: 14px;
			color: #666666;
			height: 20px;
			line-height: 20px;
		}

		:deep(.u-cell__body) {
			flex: 1;
			padding: initial;

			.u-cell__body__content {
				overflow: hidden;
			}

			.u-cell__title {
				overflow: hidden;
				padding: 0 10px;
				box-sizing: border-box;
			}

			.u-cell__title-text {
				font-size: 16px;
				color: #000;
			}
		}

		.badge {
			display: flex;
			justify-content: flex-end;
			align-items: center;
		}

		.nobell {
			image {
				width: 20px;
				height: 20px;
			}
		}
	}
</style>