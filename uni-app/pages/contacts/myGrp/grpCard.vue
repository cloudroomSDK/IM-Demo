<template>
	<view class="page-view-container">
		<u-navbar :title="infomation.groupName" autoBack border>
		</u-navbar>
		<view class="chat-setting common-cells">
			<u-cell class="grp-name block-container" :border="false">
				<template #icon>
					<view class="grp-avatar">
						<im-avatar :src="infomation.faceURL" :size="52" group></im-avatar>
					</view>
				</template>
				<template #title>
					<view class="grp-name-title">{{ infomation.groupName }}</view>
					<view class="grp-name-sub">{{ subTitle }}</view>
				</template>
			</u-cell>

			<view class="grp-member block-container" v-if="isInGrp">
				<im-members layout="members" :members="members" @click="clickMember" />
				<view>
					<u-icon class="grp-member-more" :label="membersMore" labelPos="left" name="arrow-right" size="15"
						color="#000000" @click="moreMembers"></u-icon>
				</view>
			</view>

			<!-- 	<view class="block-container">
				<view class="block-container-box">
					<u-cell :title="$t('grp.grpName')" :value="infomation.groupName">
					</u-cell> -->
			<!-- 群公告 -->
			<!-- <u-cell :title="$t('grp.grpAnnouncement')" isLink>
				</u-cell> -->
			<!-- 群昵称 -->
			<!-- 			<u-cell :title="$t('grp.grpNickname')" isLink>
				</u-cell> -->
			<!-- 	</view>
			</view> -->

			<view class="block-container" v-if="isInGrp">
				<view class="block-container-box">
					<u-cell :title="$t('grp.qrcode')" isLink @click="toImQrcode">
					</u-cell>
				</view>
			</view>

			<view class="block-container">
				<view class="block-container-box">
					<u-cell :title="$t('grp.grpID')" isLink :value="infomation.groupID" @click="toImCopyId">
					</u-cell>
				</view>
			</view>

			<view class="join" v-if="!isInGrp">
				<u-button class="join-btn" type="primary" :text="$t('joinGrp.joinGrpBtn')" @click="joinGrp"></u-button>
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
		computed
	} from "vue";
	import {
		appNavigator,
		images,
		GlobalEvents
	} from "@/utils";
	import {
		calendarTime
	} from "@/utils/imUtils";
	import {
		v4 as uuidv4
	} from "uuid";
	import {
		storeToRefs
	} from "pinia";
	import crimsdk, {
		SessionType,
		MessageReceiveOptType,
		GroupMemberFilter
	} from "@cloudroom/crimsdk";
	import useConversation from "@/store/conversation";

	const conversationStore = useConversation();
	const {
		conversationList
	} = storeToRefs(conversationStore);

	const myUid = crimsdk.getLoginUserID();
	const groupID = ref("");
	const infomation = ref({});
	const members = ref([]);
	const membersMore = computed(() => uni.$u.$fprint('members.more', [infomation.value.memberCount ?? 0]));
	const conversation = computed(() => groupID.value === "" ? {} : conversationList.value.find((con) => con.groupID ===
		groupID.value));
	const isInGrp = computed(() => members.value.findIndex(mem => mem.userID === myUid) > -1);
	const subTitle = computed(() => isInGrp.value ? infomation.value.groupID : calendarTime(infomation.value
		.createTime));
	// const membersLayout = computed(() => isOwner.value ? "members,add" : "members");

	let joinSource;
	onLoad((params) => {
		groupID.value = params.groupID;
		joinSource = params.joinSource;
		getGrpInfo(params.groupID);
		getMemberInfo(params.groupID);
	});

	const getGrpInfo = async (groupID) => {
		try {
			const {
				data
			} = await crimsdk.getSpecifiedGrpsInfo([groupID]);
			infomation.value = data[0];
		} catch (e) {
			console.log(e);
		}
	};

	const getMemberInfo = async (groupID) => {
		try {
			const grpMemberListData = await crimsdk.getGrpMemberList({
				groupID,
				filter: GroupMemberFilter.All,
				offset: 0,
				count: 10,
			});
			members.value = grpMemberListData.data;
		} catch (e) {
			console.log(e);
		}
	}

	const clickMember = (mem) => {
		appNavigator.toGrpMemberCard(Object.assign({
			isOwner: false,
		}, mem));
	}

	const toGrpMemberCard = (item) => {
		appNavigator.toGrpMemberCard({
			groupID: groupID.value,
			userID: item.userID,
		});
	};

	const moreMembers = () => {
		appNavigator.toGrpMembers({
			groupID: groupID.value,
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

	const joinGrp = async () => {
		appNavigator.toSendJoinGrpReq({
			groupID: infomation.value.groupID,
			joinSource,
		});
	}
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
			padding: 0;
		}
	}

	.grp-avatar {
		position: relative;
	}

	.grp-member {
		margin-top: 20px;
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

	.join {
		position: absolute;
		bottom: 0;
		left: 0;
		width: 100%;
		height: 70px;
		padding: 15px;
		box-sizing: border-box;
		background-color: #ffffff;
	}
</style>