<template>
	<view class="global-search">
		<view class="global-search-container">
			<u-search shape="square" :placeholder="$t('search')" :actionText="$t('cancel')" @custom="appNavigator.back"
				v-model="keyword" @search="search"></u-search>

			<view class="tags">
				<u-tag class="tag" :text="tag.text" shape="circle" :color="type === tag.value ? '#3981fc' : '#333333'"
					bgColor="#F0F0F0" borderColor="#F0F0F0" v-for="tag in tags" :key="tag.value"
					@click="chooseTag(tag)"></u-tag>
			</view>
		</view>

		<view class="search-result">
			<template v-if="type === 1">
				<im-cell v-for="item in users" :key="item.userID" :title="item.nickname" :avatar="item.faceURL"
					@click="clickCell(item)"></im-cell>
			</template>
			<template v-else-if="type === 3">
				<im-cell v-for="item in groups" :key="item.groupID" :title="item.groupName" :avatar="item.faceURL"
					groupAvatar @click="clickCell(item)"></im-cell>
			</template>
			<im-empty v-if="isEmpty" :text="emptyText" :icon="images.friEmpty" :size="100" />
		</view>

	</view>
</template>

<script setup>
	import {
		ref,
		reactive,
		computed
	} from "vue";
	import {
		appNavigator
	} from "@/utils";
	import crimsdk from "@cloudroom/crimsdk";

	const type = ref(1);
	const tags = reactive([
		// 	{
		// 	text: uni.$u.$t("globalSearch.composite"),
		// 	value: 0,
		// }, 
		{
			text: uni.$u.$t("globalSearch.contacts"),
			value: 1,
		},
		// {
		// 	text: uni.$u.$t("globalSearch.chatHistory"),
		// 	value: 2,
		// }, 
		{
			text: uni.$u.$t("globalSearch.group"),
			value: 3,
		},
		// {
		// 	text: uni.$u.$t("globalSearch.files"),
		// 	value: 4,
		// }
	]);
	const chooseTag = ({
		value
	}) => {
		type.value = value;

		if (keyword.value !== "") {
			search();
		}
	};
	const isEmpty = ref(false);
	const emptyText = computed(() => {
		if (type.value === 1) {
			return uni.$u.$t('noUserData');
		} else if (type.value === 3) {
			return uni.$u.$t('noGrpData');
		}
		return "";
	});
	const users = ref([]);
	const groups = ref([]);
	const keyword = ref("");
	const search = async () => {
		uni.showLoading({
			mask: true,
		});
		switch (type.value) {
			case 0:
				break;
			case 1:
				await getUsers();
				break;
			case 2:
				break;
			case 3:
				await getGroups();
				break;
			case 4:
				break;
		}
		uni.hideLoading();
	}

	const getUsers = async () => {
		const {
			data
		} = await crimsdk.searchFriends({
			keywordList: [keyword.value],
			isSearchNickname: true,
			isSearchUserID: true,
			isSearchRemark: true,
		});

		users.value = data;
		isEmpty.value = data.length === 0;
	}

	const getGroups = async () => {
		const {
			data
		} = await crimsdk.searchGrps({
			keywordList: [keyword.value],
			isSearchGroupID: true,
			isSearchGroupName: true,
		});

		groups.value = data;
		isEmpty.value = data.length === 0;
	}

	const clickCell = (item) => {
		switch (type.value) {
			case 0:
				break;
			case 1:
				appNavigator.toFriCard(item);
				break;
			case 2:
				break;
			case 3:
				appNavigator.toGrpCard(item);
				break;
			case 4:
				break;
		}
	};
</script>

<style lang="scss" scoped>
	.global-search {
		@include pageView(0px, $searchBoxHeight + 44px);

		&-container {
			position: fixed;
			left: 0;
			top: var(--status-bar-height);
			height: 92px;
			width: 100%;
			padding: 0px 16px !important;
			box-sizing: border-box;
			background-color: #fff;
			z-index: 9;
			display: flex;
			flex-direction: column;
		}

		.tags {
			height: 36px;
			box-sizing: border-box;
		}

		.tag {

			+.tag {
				margin-left: 3px;
			}

			:deep(.u-tag__text--medium) {
				font-size: 12px !important;
				line-height: 28px !important;
			}
		}

		.search-result {
			flex: 1;
			overflow-y: auto;
		}
	}
</style>