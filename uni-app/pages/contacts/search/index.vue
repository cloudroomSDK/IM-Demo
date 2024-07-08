<template>
	<view class="my-fri-search">
		<u-search class="im-search" shape="square" focus :placeholder="$t('search')" :actionText="$t('cancel')"
			@custom="appNavigator.back" v-model="keyword" @search="search"></u-search>

		<template v-if="isGrp">
			<im-cell v-for="item in list" :key="item.groupID" :title="item.groupName" :avatar="item.faceURL" groupAvatar
				@click="clickCell(item)"></im-cell>
		</template>
		<template v-else>
			<im-cell v-for="item in list" :key="item.userID" :title="item.nickname" :avatar="item.faceURL"
				@click="clickCell(item)"></im-cell>
		</template>

		<im-empty v-if="isEmpty" :text="isGrp ? $t('noGrpData') : $t('noUserData')" :icon="images.friEmpty"
			:size="100" />
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
		images,
		appNavigator
	} from "@/utils";
	import crimsdk from "@cloudroom/crimsdk";

	const isEmpty = ref(false);
	const isGrp = ref(false);
	const list = ref([]);
	onLoad(({
		type
	}) => {
		isGrp.value = type == 1;
	});

	const keyword = ref("");
	const search = async () => {
		const newlist = [];
		if (isGrp.value) {
			const {
				data
			} = await crimsdk.searchGrps({
				keywordList: [keyword.value],
				isSearchGroupID: true,
				isSearchGroupName: true,
			});

			list.value = data;
		} else {
			const {
				data
			} = await crimsdk.searchFriends({
				keywordList: [keyword.value],
				isSearchNickname: true,
				isSearchUserID: true,
				isSearchRemark: true,
			});

			list.value = data;
		}

		isEmpty.value = list.value.length === 0;
	}

	const clickCell = (item) => {
		if (isGrp.value) {
			appNavigator.toGrpCard(item);
		} else {
			appNavigator.toFriCard(item);
		}
	};
</script>

<style lang="scss" scoped>
	.my-fri-search {}
</style>