<template>
	<view class="page-view-search">
		<u-navbar :title="$t('pages.myFri')" autoBack></u-navbar>
		<view class="search-container">
			<u-search shape="square" :placeholder="$t('myFri.search')" :showAction="false" disabled
				@click="appNavigator.toFriOrGrpSearch(0)"></u-search>
		</view>

		<u-index-list :index-list="friendIndexList.anchors">
			<template v-for="(item, index) in friendIndexList.list">
				<u-index-item>
					<u-index-anchor :text="friendIndexList.anchors[index]" bgColor="#ffffff"></u-index-anchor>
					<im-cell v-for="cell in item" :key="cell.userID" :title="cell.nickname" :avatar="cell.faceURL"
						@click="clickCell(cell)"></im-cell>
				</u-index-item>
			</template>
		</u-index-list>
	</view>
</template>

<script setup>
	import {
		appNavigator
	} from "@/utils";
	import useContacts from "@/store/contacts";
	import {
		storeToRefs
	} from "pinia";

	const store = useContacts();
	const {
		friendIndexList
	} = storeToRefs(store);

	// store.getFriendList();

	const clickCell = (item) => {
		appNavigator.toFriCard(item);
	}
</script>

<style lang="scss" scoped>
	// :deep(.u-index-anchor) {
	// 	align-items: flex-end;
	// 	padding-bottom: 5px;
	// }
</style>