<template>
	<view class="page-view">
		<u-navbar :title="$t('pages.newFri')" autoBack></u-navbar>
		<view class="hint"> {{ $t("newFri.hint") }} </view>
		<view class="fri-list">
			<im-cell v-for="item in store.friendRecipientList" :key="item.fromUserID" :avatar="item.fromFaceURL">
				<template #title>
					<text class="truncate">
						{{ item.fromNickname }}
					</text>
				</template>
				<template #value>
					<u-button class="look" type="primary" text="查看" color="#dcdcdc"
						v-if="item.handleResult === 0 || item.handleResult === -1"
						@click="appNavigator.toHandleNewFri(item)"></u-button>
					<text v-else
						class="handle-text">{{ item.handleResult === 1 ? $t("newFri.agreed") : $t("newFri.rejected")}}</text>
				</template>
			</im-cell>
		</view>
	</view>
</template>

<script setup>
	import {
		ref,
		computed
	} from "vue";
	import {
		appNavigator
	} from "@/utils";

	import useContacts from "@/store/contacts";

	const store = useContacts();
	// store.getFriendReqListAsRecipient();
</script>

<style lang="scss" scoped>
	.fri-list {
		flex: 1;
		overflow-y: auto;
	}

	.hint {
		font-size: 12px;
		line-height: 34px;
		color: #999999;
		padding: 0 16px;
		box-sizing: border-box;
	}

	.look {
		width: 64px;
		height: 30px;
		margin-left: 15px;
		color: #333333 !important;
	}

	.handle-text {
		font-size: 16px;
		color: #666666;
	}
</style>