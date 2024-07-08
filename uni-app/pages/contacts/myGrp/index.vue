<template>
	<view class="page-view-search my-grp">
		<u-navbar :title="$t('pages.myGrp')" autoBack @rightClick="startGrpChat">
			<template #right>
				<text>{{ $t('myGrp.startGrpChat') }}</text>
			</template>
		</u-navbar>
		<view class="search-container">
			<u-search shape="square" :placeholder="$t('myGrp.search')" :showAction="false" disabled
				@click="appNavigator.toFriOrGrpSearch(1)"></u-search>
		</view>
		<view class="tabs">
			<view class="tabs-label" v-for="tab in tabs">
				<view class="tabs-label-text" :class="{'active': tabidx === tab.value}" @click="changeTab(tab.value)">
					{{tab.title }}
				</view>
			</view>
		</view>
		<!-- <view class="tabs-panel">
			<view class="tabs-panel-content" v-show="tabidx === 0">
				<im-cell v-for="cell in contactsStore.myCreateGrpList" :key="cell.groupID" :title="cell.groupName"
					:avatar="cell.faceURL" groupAvatar @click="clickCell(cell)"></im-cell>
			</view>
			<view class="tabs-panel-content" v-show="tabidx === 1">
				<im-cell v-for="cell in contactsStore.myJoinGrpList" :key="cell.groupID" :title="cell.groupName"
					:avatar="cell.faceURL" groupAvatar @click="clickCell(cell)"></im-cell>
			</view>
		</view> -->

		<view class="tabs-panel">
			<scroll-view class="tabs-panel-content" scroll-y :scroll-top="scrollTop" :key="tabidx">
				<im-cell v-for="cell in lists" :key="cell.groupID" :title="cell.groupName" :avatar="cell.faceURL"
					groupAvatar @click="clickCell(cell)"></im-cell>
			</scroll-view>
		</view>

	</view>
</template>

<script setup>
	import {
		onLoad
	} from "@dcloudio/uni-app";
	import {
		ref,
		computed,
		nextTick,
		onMounted
	} from "vue";
	import {
		appNavigator
	} from "@/utils";
	import useContacts from "@/store/contacts";

	const tabidx = ref(0);
	const tabs = [{
		title: uni.$u.$t('myGrp.myCreate'),
		value: 0,
	}, {
		title: uni.$u.$t('myGrp.myJoin'),
		value: 1,
	}]

	const contactsStore = useContacts();
	const scrollTop = ref(0);
	const lists = ref([]);
	// 没做分页，列表太长会卡页面，先这样处理
	const changeTab = (value) => {
		tabidx.value = value;
		lists.value = [];
		uni.showLoading({
			mask: true,
		});
		nextTick(() => {
			lists.value = value === 0 ? contactsStore.myCreateGrpList : contactsStore.myJoinGrpList;

			uni.hideLoading();
		});
	}

	onMounted(() => {
		changeTab(tabidx.value);
	});

	const startGrpChat = () => {
		appNavigator.toCreateGrp();
	};

	const clickCell = (item) => {
		appNavigator.toChat(item);
	}
</script>

<style lang="scss" scoped>
	.my-grp {
		padding-top: calc(44px + var(--status-bar-height) + 48px + 44px);
	}

	.tabs {
		position: fixed;
		left: 0;
		top: calc(44px + var(--status-bar-height) + 48px);
		height: 44px;
		width: 100%;
		display: flex;
		align-items: center;
		background-color: #fff;
		z-index: 1;

		&-label {
			flex: 1;
			padding: 0 16px;
			box-sizing: border-box;
			font-size: 12px;
			line-height: 28px;

			&-text {
				text-align: center;

				&.active {
					color: $u-primary;
					border-radius: 14px;
					background-color: rgba(0, 0, 0, 0.06);
				}
			}
		}

		&-panel-content {
			overflow-y: auto;
		}
	}
</style>