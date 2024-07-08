<template>
	<view class="page-view-search">
		<u-navbar :title="title" autoBack></u-navbar>
		<view class="search-container">
			<u-search v-model="keyword" shape="square" :placeholder="$t('search')" :showAction="false"
				@focus="showSearchPanel" @blur="hideSearchPanel" @clear="hideSearchPanel" @search="search"></u-search>
		</view>

		<view class="choose-view">
			<u-checkbox-group class="checkbox-group" v-model="members">
				<u-index-list class="index-list" :index-list="lists.anchors">
					<template v-for="(items, index) in lists.list">
						<u-index-item>
							<u-index-anchor :text="lists.anchors[index]" bgColor="#ffffff"></u-index-anchor>
							<view class="flex pl-15" v-for="cell in items" :key="cell.userID"
								@click.capture="clickCell(cell)">
								<u-checkbox v-if="multipleChoice" class="checkbox" shape="circle" :name="cell.userID"
									:disabled="cell.disabled"></u-checkbox>
								<im-cell class="flex-1" :title="cell.nickname" :avatar="cell.faceURL"></im-cell>
							</view>
						</u-index-item>
					</template>
				</u-index-list>
			</u-checkbox-group>
		</view>
		<!-- 搜索弹层 -->
		<u-popup class="search-users-panel" :show="showPanel" mode="bottom" :overlay="false">
			<view class="users-panel">
				<u-checkbox-group v-model="members">
					<view class="users-panel-item" v-for="cell in searchItems" :key="cell.userID">
						<view class="flex pl-15" @click.stop="clickCell(cell)">
							<u-checkbox v-if="multipleChoice" class="checkbox" shape="circle" :name="cell.userID"
								:disabled="cell.disabled"></u-checkbox>
							<im-cell class="flex-1" :title="cell.nickname" :avatar="cell.faceURL"></im-cell>
						</view>
					</view>
				</u-checkbox-group>
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
							<u-icon v-show="!member.disabled" color="#999999" name="close-circle-fill"
								@click="removeMem(index)"></u-icon>
						</template>
					</im-cell>
				</view>
			</view>
		</view>

		<im-send-usercard ref="sendUsercardRef" @done="done" />
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
		formatChooseData
	} from "@/utils";
	import {
		storeToRefs
	} from "pinia";
	import useContacts from "@/store/contacts";

	const title = ref("");
	const pageQuery = {};
	onLoad((params) => {
		if (params.redirect === "createGrp") {
			title.value = uni.$u.$t("chooseUser.createGrp");
		} else if (params.type === "add") {
			title.value = uni.$u.$t("chooseUser.addMem");
		} else if (params.type === "remove") {
			title.value = uni.$u.$t("chooseUser.removeMem");
		}
		Object.assign(pageQuery, params);
		if ("multipleChoice" in params) {
			multipleChoice.value = params.multipleChoice !== "false";
		}
		getFriendList(params);
	});

	const contactsStore = useContacts();
	const lists = ref({
		list: [],
		anchors: []
	});
	const getFriendList = (params) => {
		let items = [...contactsStore.friendInfoList].map(fri => Object.assign({
			disabled: false
		}, fri));
		// 排除指定的用户
		const exincludes = pageQuery.exincludes ? pageQuery.exincludes.split(",") : [];
		if (exincludes.length > 0) {
			items = items.filter(fri => !(exincludes.includes(fri.userID)));
		}
		console.log("type", params.type, items);
		const mems = params.members ? params.members.split(",") : [];
		if (params.type === "remove") {
			lists.value = formatChooseData(items.filter((item) => mems.includes(item.userID)));
		} else {
			// add 不传是createGrp
			lists.value = formatChooseData(items);
			if (params.members) {
				items.forEach(item => {
					if (mems.includes(item.userID)) {
						params.type === "add" && (item.disabled = true); // 群添加成员，已有的成员不给删
						members.push(item.userID);
						// membersInfo.push(item);
					}
				});
			}
		}
	}

	const multipleChoice = ref(true);
	const members = reactive([]); // 已勾选的用户id
	const membersInfo = reactive([]); // 已选中的用户信息
	const sendUsercardRef = ref(null); // 发送用户名片
	const clickCell = (memInfo) => {
		if (memInfo.disabled) return;
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
			if (pageQuery.type === "sendUsercard") {
				sendUsercardRef.value?.open({
					...memInfo,
					targetUserID: pageQuery.targetUserID, // 发送给谁
					targetNickname: pageQuery.targetNickname,
					targetFaceURL: pageQuery.targetFaceURL,
				});
			} else {
				done(memInfo);
			}
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
		contactsStore.friendInfoList.forEach(item => {
			if (item.nickname.includes(kw) || item.remark.includes(kw)) {
				list.push(item);
			}
		});
		searchItems.value = list;
	};

	// 已选列表面板
	const showDrawerBody = ref(false);
	const drawerTitle = computed(() => uni.$u.$fprint("chooseUser.sel", [membersInfo.length]));
	const drawerComfirm = computed(() => uni.$u.$fprint("chooseUser.comfirm")); // , [members.length, maxChooseCount]
	const removeMem = (idx) => {
		members.splice(idx, 1);
		membersInfo.splice(idx, 1);
	}

	const instance = getCurrentInstance().proxy;
	const eventChannel = instance.getOpenerEventChannel();
	const comfirm = () => {
		const membersRaw = toRaw(membersInfo);
		const data = pageQuery.type === "add" ? membersRaw.filter(mem => !mem.disabled) : membersRaw;
		if ("redirect" in pageQuery) {
			switch (pageQuery.redirect) {
				case "createGrp":
					appNavigator.toCreateGrp({
						data: JSON.stringify(data)
					}, {
						type: "redirectTo"
					});
					break;
				default:
					break;
			}
		} else {
			done(data);
		}
	}

	const done = (data) => {
		eventChannel.emit('choose', {
			data
		});
		appNavigator.back();
	}
</script>

<style lang="scss" scoped>
	.choose-view {
		flex: 1;
		// padding-bottom: 80px;
		padding-bottom: 60px;
		overflow: hidden;
	}

	.checkbox-group,
	.index-list,
	:deep(.index-list uni-scroll-view) {
		width: 100%;
		height: 100%;
	}

	.checkbox {
		pointer-events: none;
	}

	.search-users-panel {
		flex: none;
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
		z-index: 999;

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
				height: calc(100vh - $barNavHeight - $searchBoxHeight);
			}
		}
	}
</style>