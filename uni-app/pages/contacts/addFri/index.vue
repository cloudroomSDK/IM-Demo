<template>
	<view class="add-fri">
		<u-search class="im-search" shape="square" focus :placeholder="$t('search')" :actionText="$t('cancel')"
			@custom="appNavigator.back" v-model="keyword" @search="search"></u-search>

		<view v-if="keyword === ''">
			<im-cell :title="$t('pages.myQrcode')" :label="$t('addFri.inviteScanQrcode')" :avatar="images.qrcodeActive"
				shape="square" isLink @click="appNavigator.toMyQrcode">
			</im-cell>
			<im-cell :title="$t('addFri.scan')" :label="$t('addFri.scanQrcode')" :avatar="images.scanActive"
				shape="square" isLink border @click="scanQrCodeResult">
			</im-cell>
		</view>

		<im-cell v-for="item in list" :key="item.userID" :title="item.nickname" :avatar="item.faceURL"
			@click="clickCell(item)">
			<template #value>
				<u-button v-if="!item.isFriendShip" class="add" type="primary" color="#5AD439" :text="$t('addFri.add')"
					@click="toSendAddReq(item)"></u-button>
			</template>
		</im-cell>

		<im-empty v-if="isEmpty && keyword !== ''" :text="$t('noUserData')" />
	</view>
</template>

<script setup>
	import {
		ref,
		computed
	} from "vue";
	import {
		appNavigator,
		images,
		scanQrCodeResult,
	} from "@/utils";

	import api from "@/api";
	import crimsdk from "@cloudroom/crimsdk";

	const isEmpty = ref(false);
	const list = ref([]);
	const keyword = ref("");
	const search = async () => {
		try {
			const {
				users,
				total
			} = await api.searchUserInfo(keyword.value);
			// const friends = users;
			const friends = users.filter(({
				allowAddFriend
			}) => allowAddFriend == 1);
			const ids = friends.map(({
				userID
			}) => userID);
			const newList = [];
			if (ids.length > 0) {
				const {
					data: userInfos
				} = await crimsdk.getUsersInfo(ids);
				for (let i = 0; i < userInfos.length; i++) {
					const uInfo = userInfos[i];
					const isFriendShip = uInfo.friendInfo !== null; // uInfo.blackInfo === null
					const nObj = Object.assign({
							isFriendShip
						},
						friends[i],
						uInfo.friendInfo
					);
					newList.push(nObj);
				}
			}
			list.value = newList;
			isEmpty.value = list.value.length === 0;
		} catch (e) {
			isEmpty.value = true;
		}
	}

	const clickCell = (item) => {
		if (item.isFriendShip) {
			appNavigator.toFriCard({
				userID: item.userID
			});
		}
	};

	const toSendAddReq = (item) => {
		appNavigator.toSendAddReq({
			toUserID: item.userID
		});
	};
</script>

<style lang="scss" scoped>
	.add-fri {
		.add {
			height: 30px;
			width: 78px;
		}
	}
</style>