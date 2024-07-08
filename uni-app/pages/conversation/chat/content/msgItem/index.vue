<template>
	<view class="msg-item-notice" id="" v-if="msgType.notice"> <mp-html @linktap="linktap" :previewImg="false"
			:showImgMenu="false" :lazyLoad="false" :content="tipMsg" /></view>
	<view class="msg-item" :class="{'self': isMySender}" v-else>
		<im-avatar v-if="!isMySender" size="28" :desc="msgItem.senderNickname" :src="msgItem.senderFaceUrl"
			@longpress="atUser" @click="clickMemAvatar" />
		<view class="msg-item-container">
			<view class="msg-item-sender truncate" v-if="!isMySender">
				<text>{{msgItem.senderNickname }}</text>
			</view>
			<view class="msg-item-send-state-box">
				<view class="h-full flex flex-justify-items-center flex-items-center">
					<view class="msg-item-send-state">
						<u-loading-icon v-if="showSending  && !isPreview" />
						<u-icon size="1.4em" name="error-circle-fill" color="#ff0000" @click="reSendMsg"
							v-if="sendFail && !isPreview"></u-icon>
					</view>
				</view>

				<im-popover ref="popover" :getPlacement="getPlacement" actionsDirection="horizontal"
					:trigger="isScroll ? 'none' : 'longpress'" :options="popoverItems" theme="dark"
					@select="selectPopover">
					<view class="msg-item-content-wrap">
						<v-text-msg v-if="msgType.text" :msg-item="msgItem" @clickAtUser="toMemCard" />
						<v-media-msg v-else-if="msgType.media" :msg-item="msgItem" />
						<v-file-msg v-else-if="msgType.file" :msg-item="msgItem" />
						<v-card-msg v-else-if="msgType.card" :msg-item="msgItem" />
						<v-merge-msg v-else-if="msgType.merge" :msg-item="msgItem" />
						<v-location-msg v-else-if="msgType.location" :msg-item="msgItem" />
						<v-error-msg v-else :msg-item="msgItem" />
					</view>
				</im-popover>
			</view>
		</view>
	</view>
</template>

<script setup>
	import {
		ref,
		computed
	} from "vue";
	import {
		appNavigator,
		GlobalEvents,
		getElRect
	} from "@/utils";
	import {
		isMySelf,
		formatMsgTime,
		tipMsgFormat,
		getMsgType,
	} from "@/utils/imUtils";
	import crimsdk, {
		MessageType,
		SessionType,
		MessageStatus
	} from "@cloudroom/crimsdk";
	import useMsgStore from "@/store/msg";
	import vTextMsg from "./textMsg";
	import vMediaMsg from "./mediaMsg";
	import vFileMsg from "./fileMsg";
	import vCardMsg from "./cardMsg";
	import vMergeMsg from "./mergeMsg";
	import vLocationMsg from "./locationMsg";
	import vErrorMsg from "./errorMsg";

	const msgStore = useMsgStore();
	const props = defineProps({
		msgItem: {
			type: Object,
			default: {}
		},
		isPreview: {
			type: Boolean,
			default: false,
		},
		isScroll: {
			type: Boolean,
			default: false,
		}
	});
	const {
		msgItem
	} = props;

	const emits = defineEmits(["handleMsg"]);
	const isMySender = isMySelf(msgItem.sendID);
	const msgTime = computed(() => formatMsgTime(msgItem.sendTime));
	const msgType = getMsgType(msgItem.contentType);
	const tipMsg = computed(() => tipMsgFormat(msgItem));
	const showSending = computed(() => msgItem.status === MessageStatus.Sending); //  && sendingDelay.value
	const sendSucc = computed(() => msgItem.status === MessageStatus.Succeed);
	const sendFail = computed(() => msgItem.status === MessageStatus.Failed);
	const popoverItems = [];

	if (msgType.text) {
		popoverItems.push({
			text: "复制",
			value: 0
		});
	}
	popoverItems.push({
		text: "删除",
		value: 1
	});
	if (isMySender && Date.now() - msgItem.sendTime < 120000) {
		popoverItems.push({
			text: "撤回",
			value: 2
		});
	}
	// popoverItems.push({
	// 	text: "转发",
	// 	value: 3
	// }, {
	// 	text: "回复",
	// 	value: 4
	// }, {
	// 	text: "多选",
	// 	value: 5
	// });
	const getPlacement = async (popoverBtnRect, popoverClientRect) => {
		const {
			top,
		} = await getElRect("#chatcontent");
		if (popoverBtnRect.top - popoverClientRect.height < top) {
			return isMySender ? 'bottom-end' : 'bottom-start';
		}
		return isMySender ? 'top-end' : 'top-start';
	}
	const selectPopover = (item) => {
		switch (item.value) {
			case 0:
				uni.setClipboardData({
					showToast: false,
					data: msgItem.contentType === MessageType.TextMessage ? msgItem.textElem?.content : msgItem
						.contentType === MessageType.AtTextMessage ? msgItem.atTextElem.text : msgItem
						.quoteElem.text,
					success: function() {},
				});
				break;
			case 1:
				emits("handleMsg", "del", msgItem);
				break;
			case 2:
				emits("handleMsg", "revoked", msgItem);
				break;
		}
	}

	const atUser = () => {
		if (!isMySender && !props.isPreview) {
			const {
				sendID,
				senderNickname
			} = msgItem;
			uni.$emit(GlobalEvents.AtUser, {
				userID: sendID,
				nickname: senderNickname,
			});
		}
	};

	const linktap = ({
		href
	}) => {
		if (props.isPreview) return;
		const sourceID = href ?? msgItem.sendID;
		toMemCard(sourceID);
	};
	const clickMemAvatar = () => {
		toMemCard(msgItem.sendID);
	}

	const toMemCard = async (userID) => {
		if (props.isPreview) return;
		if (msgItem.sessionType === SessionType.Single) {
			appNavigator.toFriCard({
				userID
			});
		} else {
			const groupID = msgItem.groupID;
			const {
				data
			} = await crimsdk.getSpecifiedGrpMembersInfo({
				groupID,
				userIDList: [userID],
			});
			const member = data[0];
			appNavigator.toGrpMemberCard({
				groupID,
				userID,
				memberInfo: member ? JSON.stringify(member) : "",
				// disableAdd: this.$store.getters.storeCurrentGroup.applyMemberFriend ===
				// 	AllowType.NotAllowed,
			});
		}
	};

	const reSendMsg = () => {
		msgStore.reSendMessage(msgItem);
	};
</script>

<style lang="scss" scoped>
	.msg-bg-con {
		padding: 8px 12px;
		min-height: 32px;
		box-sizing: border-box;
		border-radius: 6px;
		background-color: #f0f0f0;
	}

	.msg-item {
		display: flex;
		flex-direction: row;
		padding: 8px 22px;
		position: relative;

		&.self {
			flex-direction: row-reverse;

			.check-wrap {
				margin-right: 0;
				margin-left: 24px;
			}

			.msg-item-container {
				margin-left: 0;
				margin-right: 0px;
				align-items: flex-end;

				.msg-item-content-wrap {
					flex-direction: row-reverse;

					.msg-bg-con {
						color: #fff;
						background-color: $u-primary !important;
					}
				}
			}

			.msg-item-send-state-box {
				flex-direction: row;
			}

			.msg-item-send-state {
				margin-left: 0px;
				margin-right: 12px;
			}
		}

		&-notice {
			position: relative;
			padding: 0 15px 6px;
			text-align: center;
			font-size: 12px;
			color: #999;
		}
	}

	.msg-item-container {
		display: flex;
		flex-direction: column;
		align-items: flex-start;
		margin-left: 10px;
		max-width: 80%;
		position: relative;

		.msg-item-sender {
			display: flex;
			max-width: 240px;
			font-size: 12px;
			color: #666;
			margin-bottom: 3px;
		}

		.msg-item-content-wrap {
			display: flex;
			align-items: center;
			text-align: start;
			font-size: 14px;
			color: $uni-text-color;
			width: fit-content;
			max-width: 100%;
			word-break: break-all;

			.bg-container {
				padding: 16px 24px;
				border-radius: 0px 12px 12px 12px;
				background-color: #f0f0f0;
			}
		}
	}

	.msg-item-send-state-box {
		display: flex;
		flex-direction: row-reverse;
	}

	.msg-item-send-state {
		display: flex;
		justify-content: center;
		align-items: center;
		margin-left: 6px;
		width: 24px;
		height: 24px;

		.read-limit-count {
			font-size: 12px;
			color: #999;
		}

		image {
			width: 8px;
			height: 8px;
		}
	}

	:deep(.emoji-display) {
		width: 12px;
		height: 9px;
		vertical-align: sub;
	}

	:deep(.zb-popover__action-text) {
		font-size: 12px;
	}
</style>