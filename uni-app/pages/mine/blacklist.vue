<template>
	<view class="page-view">
		<u-navbar :title="$t('pages.blacklist')" autoBack border>
		</u-navbar>

		<view class="blacklist">
			<u-list v-if="blackList.length > 0">
				<u-swipe-action>
					<u-list-item v-for="cell in blackList" :key="cell.userID">
						<u-swipe-action-item :options="options" @click="action" :threshold="60" :name="cell.userID">
							<im-cell :title="cell.nickname" :avatar="cell.faceURL">
							</im-cell>
						</u-swipe-action-item>
					</u-list-item>
				</u-swipe-action>
			</u-list>
			<view v-else class="empty">
				<image src="@/static/images/blacklist-empty.png"></image>
				<view class="empty-text">
					<text>{{ $t("blacklist.emptyText") }}</text>
				</view>
			</view>
		</view>
		<!-- 		<u-modal :show="show" showCancelButton asyncClose @confirm="logout" @cancel="show = false">
			<text class="rm-text">{{$t("blacklist.removeHint")}}</text>
		</u-modal> -->
	</view>
</template>

<script setup>
	import {
		ref
	} from "vue";
	import crimsdk from "@cloudroom/crimsdk";
	import {
		storeToRefs
	} from "pinia";
	import useContacts from "@/store/contacts";
	const contactsStore = useContacts();
	const {
		blackList
	} = storeToRefs(contactsStore);

	contactsStore.getBlackList();

	// const show = ref(false);
	const options = [{
		text: uni.$u.$t("remove"),
		style: {
			backgroundColor: '#FF6A6A'
		},
		idx: 0,
	}];

	const action = async ({
		index,
		name
	}) => {
		crimsdk.removeBlack(name);
	};
</script>

<style lang="scss" scoped>
	.blacklist {

		.empty {
			padding-top: 156px;
			text-align: center;

			image {
				width: 87.5px;
				height: 87.5px;
			}

			&-text {
				margin-top: 16px;
				font-size: 14px;
				color: #3D3D3D;
			}
		}


		.rm-text {
			padding: 10px 0;
			font-size: 16px;
			font-weight: 400;
			color: #333;
		}
	}
</style>