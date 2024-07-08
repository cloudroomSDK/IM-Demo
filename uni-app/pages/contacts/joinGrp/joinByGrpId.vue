<template>
	<view class="join-fri">
		<u-search class="im-search" shape="square" focus :placeholder="$t('joinGrp.search')" :actionText="$t('cancel')"
			@custom="appNavigator.back" v-model="keyword" @search="search"></u-search>

		<im-cell v-for="item in list" :key="item.groupID" :title="item.groupName" :avatar="item.faceURL"
			@click="clickCell(item)">
		</im-cell>

		<im-empty v-if="isEmpty && keyword !== ''" :text="$t('noGrpData')" />
	</view>
</template>

<script setup>
	import {
		ref
	} from "vue";
	import {
		appNavigator
	} from "@/utils";
	import crimsdk, {
		GroupJoinSource
	} from "@cloudroom/crimsdk";

	const isEmpty = ref(false);
	const list = ref([]);
	const keyword = ref();
	const search = async () => {
		try {
			const {
				data
			} = await crimsdk.getSpecifiedGrpsInfo([keyword.value]);
			list.value = data;
			isEmpty.value = list.value.length === 0;
		} catch (e) {
			isEmpty.value = true;
		}
	}

	const clickCell = (item) => {
		appNavigator.toGrpCard({
			groupID: item.groupID,
			joinSource: GroupJoinSource.Search,
		});
	};
</script>

<style lang="scss" scoped>
</style>