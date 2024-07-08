<template>
	<scroll-view class="chat-content" id="chatcontent" scroll-y upper-threshold="0" :scroll-top="scrollTop"
		:scroll-into-view="scrollIntoView" :scroll-with-animation="withAnimation" @scroll="scroll"
		@scrolltoupper="scrolltoupper">
		<view class="chat-content-list" id="chatcontentlist">
			<!-- loading-text="" loadmore-text=""  -->
			<u-loadmore nomore-text="" :status="loadMoreStatus" />
			<view class="" v-for="(item, index) in msgList" :key="`${item.clientMsgID}${item.contentType}`">
				<view class="time-line" v-if="index > 0 && getTimeline(index)">{{ getTimeline(index) }}</view>
				<v-msg-item :msg-item="item" :id="`_${item.clientMsgID}`" @handleMsg="handleMsg" />
			</view>
		</view>
		<u-modal :show="show" showCancelButton asyncClose @confirm="confirmHandleMsg" @cancel="show = false">
			<text class="handle-text">{{ handleText }}</text>
		</u-modal>
	</scroll-view>
</template>

<script setup>
	import {
		onMounted,
		getCurrentInstance,
		ref,
		computed,
		nextTick
	} from "vue";
	import {
		getMsgTimeline
	} from "@/utils/imUtils";
	import {
		getElRect
	} from "@/utils";
	import crimsdk from "@cloudroom/crimsdk";
	import useMsgStore from "@/store/msg";
	import {
		storeToRefs
	} from "pinia";
	import vMsgItem from "./msgItem";

	const msgStore = useMsgStore();
	const {
		msgList
	} = storeToRefs(msgStore);

	let scrolling = false; // 滚动中
	let timer = null;
	const scroll = (e) => {
		// crimsdk.markMsgsAsReadByMsgID(msgid);
		scrolling = true;
		timer && clearTimeout(timer);
		timer = setTimeout(() => {
			scrolling = false;
		}, 300);
	};

	const loading = ref(false);
	let isFullScreen = false;
	const getMsgList = async (isFrist = false) => {
		loading.value = true;
		await msgStore.getAdvancedHistoryMsgList();
		if (isFrist) {
			scrollToBottom();
		} else {
			scrollToAnchor(`_${msgList.value[0]?.clientMsgID}`);
		}
		nextTick(async () => {
			loading.value = false;

			// 处理内容不满屏且还有数据
			if (!isFullScreen) {
				const {
					height: ccHeight
				} = await getElRect("#chatcontent");
				const {
					height
				} = await getElRect("#chatcontentlist");
				isFullScreen = height > ccHeight;
				!isFullScreen && scrolltoupper();
			}
		});
	};

	const scrollIntoView = ref("");
	const withAnimation = ref(true);
	const scrollTop = ref(0);
	const loadMoreStatus = computed(() => msgStore.msgListIsEnd ? "nomore" : loading.value ?
		"loading" : "loadmore");
	const scrolltoupper = () => {
		(loadMoreStatus.value === "loadmore") && getMsgList();
	};
	const scrollToBottom = async () => {
		if (scrolling) return;
		const {
			height
		} = await getElRect("#chatcontentlist");
		withAnimation.value = false;
		scrollTop.value = height;
		nextTick(() => {
			withAnimation.value = true;
		});
	};

	const getTimeline = (idx) => {
		if (idx === 0) return null;
		return getMsgTimeline(msgList.value[idx].sendTime, msgList.value[idx - 1].sendTime);
	}

	const scrollToAnchor = (anchor) => {
		withAnimation.value = false;
		nextTick(() => {
			scrollIntoView.value = anchor;
			withAnimation.value = true;
		});
	};

	const show = ref(false);
	const handleText = ref("");
	let handleType;
	let handleMsgData;
	const handleMsg = (type, msg) => {
		handleType = type;
		handleMsgData = msg;
		handleText.value = type === "del" ? uni.$u.$t("chat.delMsgHint") : uni.$u.$t("chat.revokedMsgHint");
		show.value = true;
	};
	const confirmHandleMsg = () => {
		if (handleType === "del") {
			crimsdk.deleteMsg({
				conversationID: msgStore.currentConversation.conversationID,
				clientMsgID: handleMsgData.clientMsgID
			});
		} else if (handleType === "revoked") {
			crimsdk.revokeMsg({
				conversationID: msgStore.currentConversation.conversationID,
				clientMsgID: handleMsgData.clientMsgID
			});
		}
		handleType = null;
		handleMsgData = null;
		show.value = false;
	};

	// onMounted(() => {
	// 	getMsgList(true);
	// });

	defineExpose({
		getMsgList,
		scrollToBottom
	});
</script>

<style lang="scss" scoped>
	.chat-content {
		flex: 1;
		overflow: hidden;

		&-list {
			.time-line {
				position: relative;
				padding: 0 15px 6px;
				text-align: center;
				font-size: 12px;
				color: #999;
			}
		}
	}

	.handle-text {
		padding: 10px 0;
		font-size: 16px;
		font-weight: 400;
		color: #333;
	}
</style>