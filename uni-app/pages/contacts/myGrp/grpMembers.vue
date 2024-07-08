<template>
	<view class="page-view-search">
		<u-navbar :title="$t('pages.members')" autoBack></u-navbar>
		<view class="search-container">
			<u-search v-model="keyword" shape="square" :placeholder="$t('members.placeholder')" :showAction="false"
				@search="search()" @clear="getMembers()"></u-search>
		</view>

		<view class="member-list">
			<im-cell v-for="item in list" :key="item.userID" :title="item.nickname" :avatar="item.faceURL"
				@click="clickCell(item)">
				<template #value>
					<u-tag class="role-level" v-if="item.roleLevel !== GroupMemberRole.Nomal" shape="circle"
						:text="item.roleLevel == GroupMemberRole.Owner ? $t('members.owner') : $t('members.admin')"
						size="mini" type="warning"></u-tag>
				</template>
			</im-cell>

			<im-empty v-if="isEmpty" :text="$t('noUserData')" />
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
	import crimsdk, {
		GroupMemberRole,
		GroupMemberFilter
	} from "@cloudroom/crimsdk";
	import {
		isMySelf
	} from "@/utils/imUtils";
	import {
		appNavigator
	} from "@/utils";

	const keyword = ref("");
	const list = ref([]);
	const isEmpty = ref(false);

	let gid = "";
	onLoad(({
		groupID
	}) => {
		gid = groupID;
		getMembers();
	});

	const getMembers = async () => {
		const {
			data
		} = await crimsdk.getGrpMemberList({
			groupID: gid,
			filter: GroupMemberFilter.All,
			offset: 0,
			count: 1000,
		});
		list.value = data;
	}

	const search = async () => {
		const {
			data
		} = await crimsdk.searchGrpMembers({
			groupID: gid,
			keywordList: [keyword.value],
			isSearchUserID: true,
			isSearchMemberNickname: true,
			offset: 0,
			count: 1000,
		});
		list.value = data.filter(d => !isMySelf(d.userID));
	};

	const clickCell = ({
		userID
	}) => {
		appNavigator.toGrpMemberCard({
			groupID: gid,
			userID
		});
	}
</script>

<style lang="scss" scoped>
	.member-list {
		flex: 1;
		overflow-y: auto;
	}

	.role-level {
		font-size: 12px;
		transform: scale(.8);
	}
</style>