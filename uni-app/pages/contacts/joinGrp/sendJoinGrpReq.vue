<template>
	<view class="page-view">
		<u-navbar title="" autoBack border></u-navbar>
		<view class="send-add-req">
			<view class="msg-title">
				{{ $t('joinGrp.msgTitle') }}
			</view>
			<u-textarea class="send-add-req-textarea" v-model="reqMsg"></u-textarea>
			<u-button class="send-req" type="primary" :text="$t('send')" @click="send"></u-button>
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
	import {
		appNavigator
	} from "@/utils";

	let groupID;
	let joinSource;
	onLoad((params) => {
		groupID = params.groupID;
		joinSource = params.joinSource;
	});

	const reqMsg = ref("");

	const send = async () => {
		try {
			const {
				data
			} = await crimsdk.joinGrp({
				groupID,
				reqMsg: reqMsg.value,
				joinSource,
			})
			uni.$u.$itoast('sendSuss');
		} catch (e) {
			uni.$u.$itoast('sendFail');
		}
		appNavigator.back({
			delay: 1000,
		});
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