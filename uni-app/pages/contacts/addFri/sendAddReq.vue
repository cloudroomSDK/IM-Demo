<template>
	<view class="page-view">
		<u-navbar :title="$t('pages.sendAddReq')" autoBack border></u-navbar>
		<view class="send-add-req">
			<view class="msg-title">
				{{ $t('addFri.msgTitle') }}
			</view>
			<u-textarea class="send-add-req-textarea" v-model="reqMsg"></u-textarea>
			<u-button class="send-req" type="primary" :text="$t('addFri.send')" @click="send"></u-button>
		</view>
	</view>
</template>

<script setup>
	import {
		onLoad
	} from "@dcloudio/uni-app";
	import {
		ref
	} from "vue";
	import crimsdk from "@cloudroom/crimsdk";
	import useUserStore from "@/store/user";
	import {
		appNavigator
	} from "@/utils";

	let toUserID;
	onLoad((params) => {
		toUserID = params.toUserID;
	});
	const userStore = useUserStore();
	const reqMsg = ref(uni.$u.$fprint("addFri.reqMsg", [userStore.nickname]));
	const send = async () => {
		try {
			await crimsdk.addFriend({
				toUserID,
				reqMsg: reqMsg.value
			});
			uni.$u.$itoast('addFri.sendSuss');
			appNavigator.back({
				delay: 1000,
			});
		} catch (e) {
			if (e.errCode === 1304) {
				uni.$u.$itoast('errCode.relationshipAlreadyError');
			} else {
				uni.$u.$itoast('addFri.sendFail');
			}
		}
	}
</script>

<style lang="scss" scoped>
	.send-add-req {
		padding: 0 16px 16px;

		&-textarea {
			background-color: #F2F2F2 !important;

			:deep(.u-textarea__field) {
				height: 60px !important;
			}
		}
	}

	.msg-title {
		padding-top: 15px;
		font-size: 12px;
		line-height: 38px;
		color: #999999;
	}

	.send-req {
		width: 165px;
		height: 40px;
		margin-top: 35px;
	}
</style>