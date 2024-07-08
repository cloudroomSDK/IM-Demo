<template>
	<view class="page-view-nonav">
		<view class="search-container flex">
			<u-search v-model="keyword" shape="square" :placeholder="$t('search')" :actionText="$t('cancel')"
				@custom="appNavigator.back" @search="search"></u-search>
		</view>
		<!-- <im-empty v-if="isEmpty" :text="isGrp ? $t('noGrpData') : $t('noUserData')" /> -->
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
	import crimsdk from "@cloudroom/crimsdk";

	const isEmpty = ref(false);
	const list = ref([]);
	onLoad(({
		conversationID
	}) => {});

	const keyword = ref("");
	const search = async () => {
		// isEmpty.value = list.value.length === 0;
		uni.showToast({
			title: "还没做"
		})
	}
</script>

<style lang="scss" scoped>
</style>