<template>
	<view class="page-view-container user-card common-cells">
		<u-navbar :title="$t('pages.userCard')" autoBack border></u-navbar>

		<view class="block-container">
			<view class="block-container-box">
				<im-cell class="cell-avatar" :title="infomation.nickname" :avatar="infomation.publicInfo.faceURL"
					:cell-border="false">
				</im-cell>
			</view>
		</view>

		<view class="block-container">
			<view class="block-container-box">
				<u-cell :title="$t('user.idNum')">
					<template #value>
						<text>{{ infomation.userID }}</text>
						<image class="copy" :src="images.copy" @click="copyUserID"></image>
					</template>
				</u-cell>
			</view>
		</view>

		<view class="block-container">
			<view class="block-container-box">
				<u-cell :title="$t('user.remark')" :value="remark" isLink v-if="isFriend" @click="toChangeUserAttr">
				</u-cell>
				<u-cell :title="$t('user.sex')" :value="sex">
				</u-cell>
				<u-cell :title="$t('user.birthday')" :value="birth">
				</u-cell>
				<u-cell :title="$t('user.phoneNumber')" :value="infomation.phoneNumber">
				</u-cell>
			</view>
		</view>

		<view class="block-container" v-if="isFriend">
			<view class="block-container-box">
				<u-cell :title="$t('userCard.recommend')" isLink @click="recommend">
				</u-cell>
			</view>
		</view>

		<view class="block-container" v-if="isFriend">
			<view class="block-container-box">
				<u-cell :title="$t('userCard.addBlacklist')" :value="infomation.userID">
					<template #value>
						<u-switch v-model="isInBlacklist" @change="switchBlacklist"></u-switch>
					</template>
				</u-cell>
			</view>
		</view>

		<view class="btns flex flex-justify-space-around" v-if="isFriend && !isMySelfCard">
			<view class="btn flex-1" @click="appNavigator.toChat(infomation)">
				<image :src="images.sendMsg"></image>
				<view class="text">{{ $t("userCard.sendMsg")}}</view>
			</view>
			<!-- 	<view class="btn flex-1" @click="videoCall">
				<image :src="images.videoCall"></image>
				<view class="text">{{ $t("userCard.videoCall")}}</view>
			</view> -->
			<view class="btn flex-1" @click="delFriModalVisible = true">
				<image :src="images.del"></image>
				<view class="text unfriend">{{ $t("userCard.unfriend")}}</view>
			</view>
		</view>
		<view class="btns" v-else-if="!isMySelfCard">
			<u-button class="add-btn" type="primary" :text="$t('userCard.addFri')" @click="addFri"
				v-if="infomation.allowAddFriend == 1"></u-button>
		</view>

		<u-modal :show="delFriModalVisible" showCancelButton asyncClose @confirm="delFri"
			@cancel="delFriModalVisible = false">
			<text class="del-text">{{ $t("userCard.delFriHint") }}</text>
		</u-modal>
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
		images
	} from '@/utils';
	import api from "@/api";
	import crimsdk from "@cloudroom/crimsdk";
	import useContacts from "@/store/contacts";

	const contactsStore = useContacts();
	const isMySelfCard = ref(false);
	const isFriend = ref(false);
	const infomation = ref({
		publicInfo: {},
		friendInfo: {},
		blackInfo: null
	});
	const delFriModalVisible = ref(false);
	const getUserInfomation = async (userid) => {
		try {
			const {
				users
			} = await api.searchUserInfo(userid);
			const {
				data
			} = await crimsdk.getUsersInfo([userid]);
			infomation.value = Object.assign({}, data[0], users[0]);
			isInBlacklist.value = infomation.value.blackInfo != null;
		} catch (e) {
			console.log(e);
		}
	}

	const checkFriend = async (userid) => {
		const {
			data
		} = await crimsdk.checkFriend([userid]);
		const item = data[0];
		item && (isFriend.value = item.result === 1);
	};

	onLoad(({
		userID
	}) => {
		isMySelfCard.value = userID === crimsdk.getLoginUserID();
		checkFriend(userID)
		getUserInfomation(userID);
	});

	const isInBlacklist = ref(false);
	const switchBlacklist = async (val) => {
		const uid = infomation.value.userID;
		if (val) {
			await crimsdk.addBlack(uid);
		} else {
			await crimsdk.removeBlack(uid);
		}
	};

	const remark = computed(() => infomation.value.friendInfo.remark ? infomation.value.friendInfo.remark : infomation
		.value.friendInfo
		.nickname);
	const sex = computed(() => infomation.value.gender === 1 ? uni.$u.$t("user.man") : infomation.value.gender === 2 ? uni
		.$u.$t(
			"user.woman") : uni
		.$u.$t(
			"user.unknow"));
	const birth = computed(() => uni.$u.timeFormat(infomation.value.birth));

	const copyUserID = () => {
		uni.setClipboardData({
			showToast: false,
			data: infomation.value.userID,
			success: function() {
				uni.$u.$itoast("copySucc");
			},
		});
	};

	const toChangeUserAttr = () => {
		appNavigator.toChangeUserAttr({
			userID: infomation.value.userID,
			remark: remark.value,
			handle: "changeRemark",
		}, {
			events: {
				changeAttr({
					data
				}) {
					infomation.value.friendInfo.remark = data.remark;
				}
			}
		});
	}

	const recommend = () => {
		appNavigator.toChooseUser({
			multipleChoice: false,
			type: "sendUsercard",
			exincludes: infomation.value.userID || "",
			targetUserID: infomation.value.userID,
			targetNickname: infomation.value.nickname,
			targetFaceURL: infomation.value.faceURL,
		});
	};

	const delFri = async () => {
		try {
			await crimsdk.deleteFriend(infomation.value.userID);
			isFriend.value = false;
		} catch (e) {
			uni.$u.$itoast("deleteFail");
		}
		delFriModalVisible.value = false;
	};

	const addFri = () => {
		appNavigator.toSendAddReq({
			toUserID: infomation.value.userID
		});
	};
</script>

<style lang="scss" scoped>
	.user-card {
		height: auto;
		min-height: 100vh;

		.cell-block {
			padding: 0 16px;
			box-sizing: border-box;
			background: #FFFFFF;
		}

		.gap {
			padding: 15px 0;
		}

		.cell {
			:deep(.u-cell__body) {
				height: 62px;
				padding-right: 0;
			}
		}

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

	.del-text {
		padding: 10px 0;
		font-size: 16px;
		font-weight: 400;
		color: #333;
	}

	.addFri {
		width: 60%;
	}
</style>