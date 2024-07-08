<template>
	<view v-show="isInGrp">
		<view class="chat-footer">
			<view class="input-content">
				<v-custom-editor class="custom-editor" ref="customEditorRef" :placeholder="placeholder"
					@ready="editorReady" @focus="editorFocus" @blur="editorBlur" @input="editorInput"
					@tryAt="showAtPanel" />
				<view v-if="msgStore.quoteMsg" class="quote-message">
					<view class="content">
						{{ msgStore.replyQuoteMsg }}
					</view>
					<u-icon @click="cancelQuote" name="close-circle-fill" color="rbga(0,0,0,0.3)" size="16"></u-icon>
				</view>
			</view>

			<view class="footer-action-area">
				<!-- <image :src="images.msgFace" @click.prevent="showFaceToolbar = !showFaceToolbar"></image> -->
				<image :src="images.msgAdd" @click="showToolbarFn()"></image>
				<u-button :disabled="disabledSendBtn" class="send-btn" type="primary" :text="$t('chat.send')"
					@touchend.prevent="sendTextMsg"></u-button>
			</view>
		</view>
		<v-custom-toolbar v-show="showToolbar" @actionClick="actionClick" />
		<u-action-sheet Bottomround="12" :actions="actionSheetMenu" @select="selectClick" safeAreaInset
			closeOnClickOverlay closeOnClickAction :cancelText="$t('cancel')" :show="showActionSheet"
			@close="showActionSheet = false">
		</u-action-sheet>
	</view>
	<view class="no-in-grp" v-show="!isInGrp">
		{{ currentGrpInfo.status === GroupStatus.Dismissed ? $t("chat.dismissed") : $t("chat.noInGrp") }}
	</view>
</template>

<script setup>
	import {
		onMounted,
		onUnmounted,
		ref,
		computed,
		nextTick,
		watchEffect,
	} from "vue";
	import {
		images,
		getPurePath,
		appNavigator,
	}
	from "@/utils";
	import {
		formatTime,
		html2Text,
		formatInputHtml,
		offlinePushInfo
	}
	from "@/utils/imUtils";
	import {
		chooseImage,
		chooseVideo,
		chooseFile,
	} from "@/utils/uniUtils";
	import permission from "@/utils/permission";
	import crimsdk, {
		MessageType,
		GroupStatus,
		GroupMemberRole,
	} from "@cloudroom/crimsdk";
	import {
		storeToRefs
	} from "pinia";
	import useContacts from "@/store/contacts";
	import useMsgStore from "@/store/msg";
	import vCustomEditor from "./customEditor";
	import vCustomToolbar from "./customToolbar";

	const contactsStore = useContacts();
	const msgStore = useMsgStore();
	const {
		currentGrpInfo,
		currentGrpMemberInfo
	} = storeToRefs(contactsStore);
	const isGroup = computed(() => "groupID" in msgStore.currentConversation);
	const isInGrp = ref(true);
	const emits = defineEmits(["scrollToBottom", "onIsInGrp"]);
	const customEditorRef = ref(null);
	const inputHtml = ref("");
	const placeholder = ref("");
	let muted = false;
	const disabledSendBtn = computed(() => html2Text(inputHtml.value) === "" || muted);
	const isInputFocus = ref(false);
	let editorCtx = null;
	const editorReady = (ctx) => {
		editorCtx = ctx;
	};
	const editorFocus = () => isInputFocus.value = true;
	const editorBlur = () => isInputFocus.value = false;
	const editorInput = (e) => {
		inputHtml.value = e.html;
		uni.$u.throttle(updateTyping, 2000);
	};
	const updateTyping = () => {
		if (!isGroup.value) {
			crimsdk.typingStatusUpdate({
				recvID: msgStore.currentConversation.userID,
				msgTip: ""
			});
		}
	};

	const showToolbarFn = () => {
		if (muted) return;
		showToolbar.value = !showToolbar.value;
	}

	const showAtPanel = () => {
		if (isGroup.value) {
			appNavigator.toChooseMember({
				multipleChoice: true,
				groupID: msgStore.currentConversation.groupID,
			}, {
				events: {
					choose: async ({
						data
					}) => {
						console.log("showAtPanel res", data);
						if (data.length > 0) {
							editorCtx.getContents({
								success({
									html,
									text,
									delta,
								}) {
									// 删了调起面板的@
									const item = delta.ops[delta.ops.length - 1];
									if (item.insert === "@\n") {
										delta.ops.splice(delta.ops.length - 1, 1);
									} else {
										item.insert = item.insert.replace(/@(\n)?$/, "$1");
									}
									editorCtx.setContents({
										delta,
										success(e) {
											// 插入@
											insertAt(data);
										}
									});
								}
							});
						}
					}
				}
			})
		}
	};

	const cancelQuote = () => msgStore.quoteMsg && (msgStore.quoteMsg = null);

	const sendTextMsg = async () => {
		if (disabledSendBtn.value) return;
		let textMsg = "";
		const {
			text,
			atUserList
		} = formatInputHtml(inputHtml.value);
		const atlen = atUserList.length;
		if (uni.$u.trim(text) === "" && atlen === 0) {
			// if (text === "" && atlen === 0) {
			// console.log("内容不能为空");
			return;
		}
		if (atlen === 0) {
			if (msgStore.quoteMsg) {
				textMsg = crimsdk.createQuoteMsg({
					text,
					message: msgStore.quoteMsg,
				});
			} else {
				textMsg = crimsdk.createTextMsg(text);
			}
		} else {
			const quoteMessage = msgStore.quoteMsg ?? "";
			textMsg = crimsdk.createTextAtMsg({
				text,
				atUserIDList: atUserList.map((item) => item.atUserID),
				atUsersInfo: atUserList,
				message: quoteMessage,
			});
		}
		sendMsg(textMsg);
	};

	const sendImageMsg = async (path) => {
		const imageMsg = crimsdk.createImageMsgFromFullPath(getPurePath(path));
		sendMsg(imageMsg);
	}

	const sendVideoMsg = async ({
		path,
		videoType,
		duration,
		snapshotPath
	}) => {
		const params = {
			videoPath: getPurePath(path),
			videoType,
			duration,
			snapshotPath,
		};
		const videoMsg = crimsdk.createVideoMsgFromFullPath(params);
		sendMsg(videoMsg);
	};

	const needClearTypes = [
		MessageType.TextMessage,
		MessageType.AtTextMessage,
		MessageType.QuoteMessage,
	];
	const sendMsg = async (msg) => {
		if (muted) return;
		try {
			msgStore.sendNewMessage(msg);
			if (needClearTypes.includes(msg.contentType)) {
				customEditorRef.value?.clearEditor();
			}
			// 有引用、清除
			cancelQuote();
			nextTick(() => {
				emits("scrollToBottom");
			});
		} catch (e) {
			console.log("send msg", e, msg);
		}
	};

	const albumChoose = [{
			name: uni.$u.$t("picture"),
			type: "Album",
			idx: 0,
		},
		{
			name: uni.$u.$t("photograph"),
			type: "Album",
			idx: 1,
		},
	];
	const cameraChoose = [{
			name: uni.$u.$t("video"),
			type: "Camera",
			idx: 0,
		},
		{
			name: uni.$u.$t("record"),
			type: "Camera",
			idx: 1,
		},
	];

	// 图片，拍照
	const actionSheetType = ref("");
	const actionSheetMenu = computed(() => {
		if (actionSheetType.value === "Album") {
			return albumChoose;
		} else if (actionSheetType.value === "Camera") {
			return cameraChoose;
		}
		return [];
	});
	const showActionSheet = ref(false);
	const showToolbar = ref(false);
	const actionClick = (type) => {
		actionSheetType.value = type;
		switch (type) {
			case "Album":
			case "Camera":
				showActionSheet.value = true;
				break;
			case "File":
				chooseFile();
				break;
			case "Location":
				sendLocation();
				break;
			case "Card":
				sendUserCard();
				break;
			default:
				break;
		}
	}
	const selectClick = async ({
		idx
	}) => {
		if (actionSheetType.value === "Album") {
			const paths = await chooseImage([idx === 0 ? "album" : "camera"]);
			paths.forEach(sendImageMsg);
		} else if (actionSheetType.value === "Camera") {
			const videoInfo = await chooseVideo([idx === 0 ? "album" : "camera"], ["mp4"]);
			sendVideoMsg(videoInfo);
		}
	};

	const sendUserCard = () => {
		const {
			userID,
			faceURL,
			showName
		} = msgStore.currentConversation;
		appNavigator.toChooseUser({
			multipleChoice: false,
			type: "sendUsercard",
			exincludes: userID || "",
			targetUserID: userID,
			targetNickname: showName,
			targetFaceURL: faceURL,
		});
	}

	const sendLocation = () => {
		const enable = permission.checkSystemEnableLocation();
		if (enable) {
			appNavigator.previewMap({
				isPerview: false,
			}, {
				events: {
					choose: async ({
						data
					}) => {
						console.log(data);
						const [longitude, latitude] = data.location.split(",");
						const locMsg = crimsdk.createLocationMsg({
							description: JSON.stringify({
								name: data.name,
								addr: data.address,
							}),
							latitude,
							longitude,
						});
						console.log(locMsg);
						sendMsg(locMsg);
					}
				}
			});
		} else {
			uni.$u.$itoast("please open locaion");
		}
	}

	// const showFaceToolbar = ref(false);

	const insertAt = (mems) => {
		const {
			atUserList
		} = formatInputHtml(inputHtml.value);
		const atMems = [];
		for (let i = 0; i < mems.length; i++) {
			const mem = mems[i];
			const idx = atUserList.findIndex((item) => item.atUserID === mem.userID);
			if (idx === -1) {
				atMems.push({
					userID: mem.userID,
					nickname: mem.nickname
				});
			};
		}
		atMems.length > 0 && customEditorRef.value?.createCanvasData(atMems);
	};

	let timer = null;
	watchEffect(() => {
		try {
			if (isGroup.value) {
				const cGrpInfo = currentGrpInfo.value;
				const cGrpMemberInfo = currentGrpMemberInfo.value;
				if (cGrpInfo.groupID) {
					// 解散或退出
					if (cGrpInfo.status === GroupStatus.Dismissed || cGrpMemberInfo ===
						null) {
						isInGrp.value = false;
						emits("onIsInGrp", false);
					} else if (cGrpMemberInfo != null) {
						if (cGrpInfo.status === GroupStatus.Muted) {
							// 群禁言 只对普通成员生效
							if (cGrpMemberInfo.roleLevel === GroupMemberRole.Nomal) {
								muted = true;
								placeholder.value = uni.$u.$t("chat.grpMuted");
							}
						} else if (cGrpMemberInfo.muteEndTime > Date.now()) {
							// 群成员禁言
							muted = true;
							placeholder.value = uni.$u.$t("chat.muted");
							const diff = cGrpMemberInfo.muteEndTime - Date.now();
							timer && clearTimeout(timer);
							timer = setTimeout(() => {
								muted = false;
								placeholder.value = "";
								timer = null;
							}, diff);
						} else {
							muted = false;
							placeholder.value = "";
						}
					}
				}
			} else {
				// const target = contactsStore.blackList.find(
				// 	(black) => black.userID === msgStore.currentConversation.userID,
				// );
				// placeholder.value = target ? uni.$u.$t("chat.targetInBackList") : "";
			}
		} catch (e) {
			console.log(e);
		}
	});

	onUnmounted(() => {
		timer && clearTimeout(timer);
	});

	defineExpose({
		insertAt
	});

	const keyboardChangeHander = ({
		height
	}) => {
		height > 0 && (showActionSheet.value = false);
	};

	onMounted(() => {
		// uni.onKeyboardHeightChange(keyboardChangeHander);
	});
</script>

<style lang="scss" scoped>
	.chat-footer {
		display: flex;
		align-items: center;
		max-height: 120px;
		padding: 12px 10px;
		box-sizing: border-box;
		background: #ffffff;

		.input-content {
			flex: 1;
			min-height: 30px;
			max-height: 120px;
			margin: 0 12px;
			border-radius: 4px;
			position: relative;
		}

		.quote-message {
			display: flex;
			justify-content: center;
			align-items: center;
			justify-content: space-between;
			margin-top: 6px;
			padding: 4px;
			border-radius: 3px;
			background-color: #fff;
			color: #666;

			.content {
				:deep(uni-view) {
					overflow: hidden;
					text-overflow: ellipsis;
					display: -webkit-box;
					-webkit-line-clamp: 2;
					-webkit-box-orient: vertical;
				}
			}
		}

		.footer-action-area {
			display: flex;
			align-items: center;

			image {
				width: 26px;
				height: 26px;

				~image {
					margin-left: 11px;
				}

			}
		}

		.send-btn {
			width: 50px;
			height: 30px;
			line-height: 30px;
			border-radius: 6px 6px 6px 6px;
			margin-left: 10px;
		}
	}

	.no-in-grp {
		height: 60px;
		line-height: 60px;
		color: red;
		font-size: 14px;
		text-align: center;
		background-color: #f2f2f2;
	}
</style>