<template>
	<view class="page-view-container" :class="{'is-agree': isAgree}">
		<u-navbar :title="title" autoBack></u-navbar>
		<view class="new-fri-handle" v-if="!isAgree">
			<view class="info flex flex-items-center">
				<im-avatar :src="info.fromFaceURL" :size="40"></im-avatar>
				<view class="flex-1 ml-15 truncate">
					<text>{{ info.fromNickname }}</text>
				</view>
			</view>
			<view class="req-msg">
				{{ info.reqMsg }}
			</view>
			<view class="flex">
				<view class="flex-1 pr-15">
					<u-button plain :text="$t('newFri.reject')" @click="reject"></u-button>
				</view>
				<view class="flex-1 pl-15">
					<u-button class="flex-1" type="primary" :text="$t('newFri.agree')" @click="agree"></u-button>
				</view>
			</view>
		</view>
		<view class="new-fri-verified" v-else>
			<view class="setremark-title">
				{{ $t('newFri.setRemark') }}
			</view>
			<u-textarea class="new-fri-textarea" v-model="remark" :placeholder="$t('newFri.setRemark')"></u-textarea>
			<u-button class="finished" type="primary" :text="$t('newFri.finished')" @click="finished"></u-button>
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
		appNavigator
	} from "@/utils";

	import useContacts from "@/store/contacts";

	const store = useContacts();

	const isAgree = ref(false);
	const title = computed(() => isAgree.value ? uni.$u.$t('newFri.verifiedSucc') : uni.$u.$t('pages.newFri'));

	const info = ref({
		fromUserID: '',
		fromNickname: '',
		fromFaceURL: '',
		reqMsg: '',
	});

	onLoad((params) => {
		info.value = params;
		store.lookFriendReq(params.fromUserID);
	});

	const agree = async () => {
		await store.acceptFriendReq(info.value.fromUserID, "");
		isAgree.value = true;
	};
	const reject = async () => {
		await store.refuseFriendReq(info.value.fromUserID, "");
		appNavigator.back();
	};

	const remark = ref("");
	const finished = async () => {
		await store.setFriendRemark(info.value.fromUserID, remark.value);
		// appNavigator.toContacts();
		appNavigator.toChat({
			userID: info.value.fromUserID
		});
	};
</script>

<style lang="scss" scoped>
	.is-agree {
		background-color: #fff !important;
	}

	.new-fri {
		&-handle {
			padding: 16px;
			box-sizing: border-box;
			background-color: #fff;
		}

		&-verified {
			padding: 0 16px 16px;
		}

		&-textarea {
			background-color: #F2F2F2 !important;

			:deep(.u-textarea__field) {
				height: 60px !important;
			}
		}
	}

	.req-msg {
		min-height: 80px;
		padding: 15px;
		box-sizing: border-box;
		margin-top: 24px;
		margin-bottom: 20px;
		font-size: 16px;
		background-color: #f2f2f2;
		word-break: break-all;
	}

	.setremark-title {
		font-size: 12px;
		line-height: 38px;
		color: #999999;
	}

	.finished {
		width: 165px;
		height: 40px;
		margin-top: 80px;
	}
</style>