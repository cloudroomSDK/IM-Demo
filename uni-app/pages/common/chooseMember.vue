<template>
	<view class="page-view-search" :class="{'multi-selection': multipleChoice}">
		<u-navbar :title="title" autoBack></u-navbar>
		<view class="search-container">
			<u-search v-model="keyword" shape="square" :placeholder="$t('search')" :showAction="false"
				@focus="showSearchPanel" @blur="hideSearchPanel" @clear="hideSearchPanel" @search="search"></u-search>
		</view>

		<view class="choose-view">
			<template v-if="multipleChoice">
				<u-checkbox-group class="checkbox-group" v-model="members">
					<view class="flex w-full pl-15" v-for="cell in grpMembersList" :key="cell.userID"
						@click.stop="clickCell(cell)">
						<u-checkbox class="checkbox" shape="circle" :name="cell.userID"></u-checkbox>
						<im-cell class="flex-1" :title="cell.nickname" :avatar="cell.faceURL"></im-cell>
					</view>
				</u-checkbox-group>
			</template>
			<template v-else>
				<im-cell v-for="cell in grpMembersList" :key="cell.userID" :title="cell.nickname" :avatar="cell.faceURL"
					@click="clickCell(cell)"></im-cell>
			</template>
		</view>
		<!-- 搜索弹层 -->
		<u-popup class="search-users-panel" :show="showPanel" mode="bottom" :overlay="false">
			<view class="users-panel">
				<template v-if="multipleChoice">
					<u-checkbox-group v-model="members">
						<view class="users-panel-item" v-for="cell in searchItems" :key="cell.userID">
							<view class="flex pl-15" @click.stop="clickCell(cell)">
								<u-checkbox class="checkbox" shape="circle" :name="cell.userID"
									v-if="multipleChoice"></u-checkbox>
								<im-cell class="flex-1" :title="cell.nickname" :avatar="cell.faceURL"></im-cell>
							</view>
						</view>
					</u-checkbox-group>
				</template>
				<template v-else>
					<im-cell v-for="cell in searchItems" :key="cell.userID" :title="cell.nickname"
						:avatar="cell.faceURL" @click="clickCell(cell)"></im-cell>
				</template>
			</view>
		</u-popup>
		<!--  -->
		<view class="im-drawer" v-if="multipleChoice">
			<view class="im-drawer-title">
				<view @click="showDrawerBody = !showDrawerBody">{{ drawerTitle }} <u-icon class="arrow"
						:name="showDrawerBody ? 'arrow-up': 'arrow-down'"></u-icon></view>
				<view>
					<u-button class="im-drawer-comfirm" type="primary" :text="drawerComfirm"
						@click="comfirm"></u-button>
				</view>
			</view>
			<!-- <u-transition></u-transition> -->
			<view class="im-drawer-body" :class="{'show': showDrawerBody}">
				<view class="member-list">
					<im-cell v-for="(member, index) in membersInfo" :key="member.userID" class=""
						:title="member.nickname" :avatar="member.faceURL">
						<template #value>
							<u-icon color="#999999" name="close-circle-fill" @click="removeMem(index)"></u-icon>
						</template>
					</im-cell>
				</view>
			</view>
		</view>
	</view>
</template>

<script setup>
	import {
		onLoad
	} from "@dcloudio/uni-app";
	import {
		onMounted,
		getCurrentInstance,
		ref,
		computed,
		reactive,
		toRaw
	} from "vue";
	import {
		appNavigator,
		GlobalEvents,
	} from "@/utils";
	import {
		isMySelf
	} from "@/utils/imUtils";
	import {
		storeToRefs
	} from "pinia";
	import crimsdk, {
		GroupMemberFilter
	} from "@cloudroom/crimsdk";
	// import {
	// 	testMembers
	// } from "@/testJson/group"

	onLoad((params) => {
		params.multipleChoice && (multipleChoice.value = params.multipleChoice !== "false");
		params.groupID && getMembers(params.groupID);
	});

	const title = ref("");
	const multipleChoice = ref(true);
	const grpMembersList = ref([]);

	const getMembers = async (groupID) => {
		const {
			data
		} = await crimsdk.getGrpMemberList({
			groupID,
			filter: GroupMemberFilter.All,
			offset: 0,
			count: 1000,
		});
		grpMembersList.value = data.filter(d => !isMySelf(d.userID));
	}

	// const maxChooseCount = 99; // 最大可选人数
	const members = reactive([]); // 已选的用户id
	const membersInfo = reactive([]); // 已选中的用户信息

	const instance = getCurrentInstance().proxy;
	const eventChannel = instance.getOpenerEventChannel();
	const comfirm = () => {
		eventChannel.emit('choose', {
			data: toRaw(membersInfo)
		});
		appNavigator.back();
	}

	const clickCell = (memInfo) => {
		const uid = memInfo.userID;
		if (multipleChoice.value) {
			// 多选
			if (members.includes(uid)) {
				const idx = members.findIndex(mem => mem === uid);
				members.splice(idx, 1);
				membersInfo.splice(idx, 1);
			} else {
				members.push(uid);
				membersInfo.push(memInfo);
			}
		} else {
			// 单选
			membersInfo.push(memInfo);
			comfirm();
		}
	};

	// 搜索面板
	const keyword = ref("");
	const searchItems = ref([]);
	const showPanel = ref(false);
	const showSearchPanel = () => showPanel.value = true;
	const hideSearchPanel = () => {
		if (uni.$u.trim(keyword.value) === "") {
			showPanel.value = false;
			searchItems.value = [];
		}
	};
	const search = () => {
		const list = [];
		const kw = keyword.value;
		grpMembersList.value.forEach(item => {
			if (item.userID === kw || item.nickname.includes(kw) || item.remark.includes(kw)) {
				list.push(item);
			}
		});
		searchItems.value = list;
	};

	// 已选列表面板
	const showDrawerBody = ref(false);
	const drawerTitle = computed(() => uni.$u.$fprint("chooseUser.sel", [members.length]));
	const drawerComfirm = uni.$u.$t(
		"chooseUser.comfirm"); // computed(() => uni.$u.$fprint("chooseUser.comfirm", [members.length, maxChooseCount]));
	const removeMem = (idx) => {
		members.splice(idx, 1);
		membersInfo.splice(idx, 1);
	}
</script>

<style lang="scss" scoped>
	.page-view-search {}

	.multi-selection {
		.choose-view {
			padding-bottom: 60px;
		}
	}

	.choose-view {
		flex: 1;
		overflow: hidden;
	}

	.checkbox-group,
	.index-list,
	:deep(.index-list uni-scroll-view) {
		height: 100%;
	}

	.checkbox {
		pointer-events: none;
	}

	.search-users-panel {
		flex: none;
		background-color: blueviolet;
	}

	.users-panel {
		height: calc(100vh - $barNavHeight - $searchBoxHeight);
		overflow-y: auto;

		&-item {
			width: 100%;
		}
	}

	.im-drawer {
		position: fixed;
		width: 100%;
		left: 0;
		bottom: 0;
		background-color: #fff;

		&-title {
			display: flex;
			align-items: center;
			justify-content: space-between;
			height: 60px;
			padding: 0 16px;
			font-size: 16px;
			color: $uni-color-primary;

			.arrow {
				display: inline-block !important;

				:deep(.u-iconfont) {
					color: $uni-color-primary !important;
				}
			}
		}

		&-comfirm {
			width: 110px;
			height: 35px;

			:deep(.u-button__text) {
				font-size: 16px !important;
			}
		}

		&-body {
			height: 0;
			overflow-y: auto;
			transition: all 0.1s ease-in-out;

			&.show {
				height: 130px;
			}
		}

	}
</style>