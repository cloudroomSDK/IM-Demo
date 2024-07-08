<template>
	<view class="page-view-container">
		<u-navbar :title="$t('pages.grpChatSetting')" autoBack border>
		</u-navbar>
		<view class="chat-setting">
			<u-cell class="grp-name" :border="false">
				<template #icon>
					<view class="grp-avatar" @click="changeGrpAvatar">
						<im-avatar :src="infomation.faceURL" :size="52" group></im-avatar>
						<view v-if="isOwnerOrAdmin" class="grp-avatar-modifiy-icon"></view>
					</view>
				</template>
				<template #title>
					<view class="grp-name-title truncate">{{ infomation.groupName }}</view>
					<view class="grp-name-sub truncate">{{ infomation.groupID }}</view>
				</template>
			</u-cell>

			<view class="grp-member">
				<im-members :layout="membersLayout" :showname="false" :members="members" :groupID="groupID"
					:choose="chooseMember" @click="clickMember" />
				<view>
					<u-icon class="grp-member-more" :label="membersMore" labelPos="left" name="arrow-right" size="15"
						color="#000000" @click="moreMembers"></u-icon>
				</view>
			</view>

			<view class="common-cells clamp1">
				<view class="block-container">
					<view class="block-container-box">
						<u-cell :title="$t('chatSetting.grpName')" :isLink="isOwnerOrAdmin"
							:value="infomation.groupName" @click="changeGrpName">
						</u-cell>
						<!-- 群公告 -->
						<!-- <u-cell :title="$t('chatSetting.grpAnnouncement')" isLink>
					</u-cell> -->
						<!-- 群昵称 -->
						<u-cell :title="$t('chatSetting.grpNickname')" :value="myNickName">
						</u-cell>
					</view>
				</view>

				<view class="block-container">
					<view class="block-container-box">
						<u-cell :title="$t('chatSetting.grpCode')" isLink @click="toImQrcode"><template #value>
								<image class="qrcode" :src="images.qrcode"></image>
							</template>
						</u-cell>
					</view>
				</view>

				<view class="block-container">
					<view class="block-container-box">
						<u-cell :title="$t('chatSetting.grpID')" isLink @click="toImCopyId">
						</u-cell>
					</view>
				</view>

				<view class="block-container">
					<view class="block-container-box">
						<u-cell :title="$t('chatSetting.searchChatHistory')" isLink @click="toSearchChatHistory">
						</u-cell>
						<!-- 消息免打扰 -->
						<u-cell :title="$t('chatSetting.dnd')">
							<template #value>
								<u-switch v-model="recvMsgOpt" @change="changeOpt($event,'recvMsgOpt')"></u-switch>
							</template>
						</u-cell>
						<!-- 置顶聊天 -->
						<u-cell :title="$t('chatSetting.pinned')">
							<template #value>
								<u-switch v-model="conversation.isPinned"
									@change="changeOpt($event,'pinned')"></u-switch>
							</template>
						</u-cell>
						<u-cell :title="$t('chatSetting.clearChatHistory')" isLink @click="showClearCHModal = true">
						</u-cell>
					</view>
				</view>
			</view>

			<u-button class="quit" type="default"
				:text="isOwner ? $t('chatSetting.dismissGrp') : $t('chatSetting.quitGrp')"
				@click="quitGrpModalVisible = true"></u-button>
		</view>
		<u-action-sheet :actions="avatarChoose" closeOnClickOverlay closeOnClickAction :show="selAvatarVisible"
			:round="16" :cancelText="$t('cancel')" @select="selAvatar"
			@close="selAvatarVisible = false"></u-action-sheet>

		<u-modal :show="quitGrpModalVisible" showCancelButton @confirm="quitGrp" @cancel="quitGrpModalVisible = false">
			<!--  $t("chatSetting.transformGrpHint") -->
			<text
				class="quit-text">{{ isOwner ? $t("chatSetting.dismissGrpHint") :  $t("chatSetting.quitGrpHint") }}</text>
		</u-modal>

		<u-modal :show="showClearCHModal" showCancelButton asyncClose @confirm="clearChatHistory"
			@cancel="showClearCHModal = false">
			<text> {{$t("chatSetting.clearChatHistory") }}</text>
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
	import {
		appNavigator,
		images,
		getPurePath,
		GlobalEvents
	} from "@/utils";
	import {
		chooseImage,
		chooseVideo,
	} from "@/utils/uniUtils";
	import {
		v4 as uuidv4
	} from "uuid";
	import {
		storeToRefs
	} from "pinia";
	import crimsdk, {
		GroupMemberRole,
		SessionType,
		MessageReceiveOptType,
		GroupMemberFilter
	} from "@cloudroom/crimsdk";
	import useConversation from "@/store/conversation";
	import useContacts from "@/store/contacts";
	import useMsgStore from "@/store/msg";

	const conversationStore = useConversation();
	const contactsStore = useContacts();
	const msgStore = useMsgStore();

	const myUid = crimsdk.getLoginUserID();
	const groupID = ref("");
	const infomation = ref({});
	const members = ref([]);
	const membersMore = computed(() => uni.$u.$fprint('members.more', [members.value.length]));
	const isOwner = computed(() => infomation.value.ownerUserID === myUid);
	const isOwnerOrAdmin = computed(() => {
		const mem = members.value.find(mem => mem.userID === myUid);
		return mem ? mem.roleLevel !== GroupMemberRole.Nomal : false;
	});

	const membersLayout = computed(() => isOwnerOrAdmin.value ? "members,add,remove" : "members,add");
	const {
		currentConversation: conversation
	} = storeToRefs(conversationStore);

	const myNickName = computed(() => {
		const item = members.value.find(mem => mem.userID === myUid);
		return item ? item.nickname : "";
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
	const quitGrpModalVisible = ref(false);

	onLoad((params) => {
		const gid = params.groupID;
		groupID.value = gid;
		getGrpInfo(gid);
		getMemberInfo(gid);
	});

	const getGrpInfo = async (groupID) => {
		infomation.value = await contactsStore.getSpecifiedGrpsInfo(groupID);
	};

	const getMemberInfo = async (groupID) => {
		members.value = await contactsStore.getGrpMemberList(groupID);
	}

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

			await crimsdk.setGrpInfo({
				groupID: groupID.value,
				faceURL: url,
			});

			infomation.value.faceURL = url;
		} catch (e) {
			uni.$u.$itoast("uploadFail");
		}
	};

	const clickMember = (mem) => {
		appNavigator.toGrpMemberCard(Object.assign({
			isOwner: isOwner.value,
		}, mem));
	}

	// 添加、删除成员的回调
	const chooseMember = async ({
		type,
		data
	}) => {
		// console.log(type, data);
		if (data.length === 0) return;
		const userIDList = data.map(({
			userID
		}) => userID);
		if (type === "add") {
			// add
			try {
				await crimsdk.inviteUserToGrp({
					groupID: groupID.value,
					userIDList,
					reason: "",
				});
				// members改成store了
				// members.value.push(...data);
			} catch (e) {
				console.log(`inviteUserToGrp: ${e}`);
			}
		} else if (type === "remove") {
			try {
				await crimsdk.kickGrpMember({
					groupID: groupID.value,
					userIDList,
					reason: "",
				});
				// 
				// members改成store了
				// userIDList.forEach(uid => {
				// 	const idx = members.value.findIndex(mem => mem.userID === uid);
				// 	idx > -1 && (members.value.splice(idx, 1));
				// });
			} catch (e) {
				console.log(`kickGrpMember: ${e}`);
			}
		}
	};

	const changeGrpAvatar = () => {
		// 判断能否改群头像
		if (!isOwnerOrAdmin.value) return;
		selAvatarVisible.value = true;
	};

	const toGrpMemberCard = (item) => {
		appNavigator.toGrpMemberCard({
			groupID: groupID.value,
			userID: item.userID,
		});
	};

	const changeGrpName = () => {
		// 判断能否改群信息
		if (!isOwnerOrAdmin.value) return;
		appNavigator.toChangeGrpAttr({
			groupID: groupID.value,
			grpName: infomation.value.groupName
		}, {
			events: {
				changeAttr({
					data
				}) {
					infomation.value.groupName = data.grpName;
				}
			}
		});
	}

	const moreMembers = () => {
		appNavigator.toGrpMembers({
			groupID: groupID.value,
		});
	}

	const toSearchChatHistory = () => {
		appNavigator.toSearchChatHistory({
			conversationID: conversation.value.conversationID,
		});
	}

	const toImQrcode = () => {
		appNavigator.toImQrcode({
			groupID: groupID.value,
			showname: infomation.value.groupName,
			faceURL: infomation.value.faceURL,
		});
	};

	const toImCopyId = () => {
		appNavigator.toImCopyId({
			groupID: groupID.value,
		});
	};

	const changeOpt = async (value, type) => {
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
				console.log(`e: ${e}`);
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
		}
	}

	// 退出群聊
	const quitGrp = async () => {
		quitGrpModalVisible.value = false;
		// 判断是否群主 - > 先转让群
		// if (isOwner.value && infomation.value.memberCount > 1) {
		// appNavigator.toChooseMember({
		// 	multipleChoice: true,
		// 	groupID: groupID.value,
		// }, {
		// 	events: {
		// 		choose: async ({
		// 			data
		// 		}) => {
		// 			await crimsdk.transferGrpOwner({
		// 				groupID: groupID.value,
		// 				newOwnerUserID: data[0].userID,
		// 			});
		// 			await crimsdk.quitGrp(groupID.value);
		// 			appNavigator.toConversation();
		// 		}
		// 	}
		// })
		// }

		// 只有自己 -> 解散群？
		if (isOwner.value) {
			await crimsdk.dismissGrp(groupID.value);
		} else {
			await crimsdk.quitGrp(groupID.value);
		}
		appNavigator.toConversation();
	};

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
	.page-view-container {
		overflow: hidden;
	}

	.chat-setting {
		padding-bottom: 80px;
		overflow-y: scroll;
	}

	.grp-name {
		height: 76px;
		padding: 0 20px;
		justify-content: center;
		background-color: #fff;

		&-title {
			font-size: 16px;
			color: #000;
			padding-left: 10px;
		}

		&-sub {
			font-size: 14px;
			color: #666;
			padding-left: 10px;
		}

		:deep(.u-cell__body) {
			width: 100%;
			padding: 0;

			.u-cell__body__content {
				width: 100%;

				.u-cell__title {
					overflow: hidden;
				}
			}
		}
	}

	.grp-avatar {
		position: relative;

		&-modifiy-icon {
			position: absolute;
			right: 0;
			bottom: 0;
			width: 18px;
			height: 18px;
			border-radius: 50%;
			background-image: url(~@/static/images/modifiy.png);
			background-size: cover;
		}
	}

	.qrcode {
		width: 30px;
		height: 30px;
	}

	.grp-member {
		margin: 20px 0;
		padding: 15px 20px;
		box-sizing: border-box;
		background-color: #fff;

		&-more {
			padding-top: 20px;
			justify-content: center;
		}
	}

	.quit {
		position: absolute;
		bottom: 0;
		left: 0;
		width: 100%;
		height: 60px;
		color: #FF0000;
		background-color: #ffffff;
	}

	.quit-text {
		padding: 10px 0;
		font-size: 16px;
		font-weight: 400;
		color: #333;
	}
</style>