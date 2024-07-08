<template>
	<view class="page-view-container member-card common-cells">
		<u-navbar title="" autoBack border></u-navbar>

		<view class="block-container">
			<view class="block-container-box">
				<im-cell class="cell-avatar" :title="infomation.nickname" :avatar="infomation.faceURL"
					:cell-border="false">
				</im-cell>
			</view>
		</view>

		<view class="block-container">
			<view class="block-container-box">
				<u-cell class="cell" :title="$t('user.idNum')">
					<template #value>
						<text>{{ infomation.userID }}</text>
						<image class="copy" :src="images.copy" @click="copyUserID"></image>
					</template>
				</u-cell>
			</view>
		</view>

		<view class="block-container" v-if="isOwner && !isMySelfCard">
			<view class="block-container-box">
				<u-cell class="cell" :title="$t('grpMemberCard.setAsAdmin')" :value="infomation.userID">
					<template #value>
						<u-switch v-model="isAdmin" @change="setAsAdmin"></u-switch>
					</template>
				</u-cell>
			</view>
		</view>

		<view class="block-container">
			<view class="block-container-box">
				<u-cell :title="$t('grpMemberCard.profile')" isLink @click="toUserCard">
				</u-cell>
			</view>
		</view>

		<view class="btns flex flex-justify-space-around" v-if="!isMySelfCard">
			<view class="btn flex-1" @click="appNavigator.toChat(infomation)">
				<image :src="images.sendMsg"></image>
				<view class="text">{{ $t("userCard.sendMsg")}}</view>
			</view>
			<!-- 	<view class="btn flex-1" @click="videoCall">
				<image :src="images.videoCall"></image>
				<view class="text">{{ $t("userCard.videoCall")}}</view>
			</view> -->
		</view>
	</view>
</template>

<script setup>
	import {
		onLoad,
		onHide,
	} from "@dcloudio/uni-app";
	import {
		ref,
		reactive,
		computed,
	} from "vue";
	import {
		appNavigator,
		images,
	} from '@/utils';
	import api from "@/api";
	import crimsdk, {
		GroupMemberRole
	} from "@cloudroom/crimsdk";

	const isOwner = ref(false);
	const infomation = ref({});
	const isMySelfCard = ref(false);
	const getMemberInfomation = async (groupID, userid) => {
		try {
			const {
				data
			} = await crimsdk.getSpecifiedGrpMembersInfo({
				groupID,
				userIDList: [userid]
			});
			infomation.value = data[0];
		} catch (e) {
			console.log(e);
		}
	}

	onLoad((params) => {
		isOwner.value = params.isOwner === "true";
		isMySelfCard.value = params.userID === crimsdk.getLoginUserID();
		getMemberInfomation(params.groupID, params.userID);
	});

	const isAdmin = ref(false);
	const setAsAdmin = async (val) => {
		const {
			userID,
			groupID
		} = infomation.value;
		try {
			await crimsdk.setGrpMemberRoleLevel({
				groupID,
				userID,
				roleLevel: val ? GroupMemberRole.Admin : GroupMemberRole.Nomal,
			});
		} catch (e) {
			isAdmin.value = !isAdmin.value;
		}
	};

	const copyUserID = () => {
		uni.setClipboardData({
			showToast: false,
			data: infomation.value.userID,
			success: function() {
				uni.$u.$itoast("copySucc");
			},
		});
	};

	const toUserCard = () => {
		appNavigator.toFriCard({
			userID: infomation.value.userID,
		});
	}

	const videoCall = () => {
		uni.$u.toast("电话");
	};
</script>

<style lang="scss" scoped>
	.member-card {
		height: auto;
		min-height: 100vh;

		.cell-avatar {
			height: 80px;
		}

		.copy {
			width: 20px;
			height: 20px;
			margin-left: 30px;
		}

		.btns {
			padding: 84px 20px 80px;
			box-sizing: border-box;
		}

		.btn {
			text-align: center;

			image {
				width: 60px;
				height: 60px;
			}

			.text {
				font-size: 16px;
				color: #3d3d3d;
				line-height: 1;
				margin-top: 20px;

				&.unfriend {
					color: red;
				}
			}
		}
	}
</style>