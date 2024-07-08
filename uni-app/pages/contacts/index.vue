<template>
	<view class="contacts bottom-tabbar">
		<u-navbar height="64" :title="$t('pages.contacts')" leftIcon="">
			<template #right>
				<im-menus />
			</template>
		</u-navbar>
		<view class="search-container">
			<u-search shape="square" :placeholder="$t('search')" :showAction="false" disabled
				@click="appNavigator.toSearch"></u-search>
		</view>

		<im-cell :title="$t('pages.newFri')" :avatar="images.newFri" isLink @click="appNavigator.toNewFri">
			<template #value>
				<u-badge max="99" :value="contactsStore.friendReqCount"></u-badge>
			</template>
		</im-cell>
		<im-cell :title="$t('pages.myFri')" :avatar="images.myFri" isLink @click="appNavigator.toMyFri"></im-cell>
		<im-cell :title="$t('pages.myGrp')" :avatar="images.myGrp" isLink @click="appNavigator.toMyGrp"></im-cell>
		<im-tabbar />
	</view>
</template>

<script setup>
	import {
		ref,
		reactive,
		computed
	} from "vue";
	import {
		appNavigator,
		images
	} from "@/utils";
	import useContacts from "@/store/contacts";

	const contactsStore = useContacts();
</script>

<style lang="scss" scoped>
	.contacts {
		// padding-top: calc(var(--status-bar-height) + 68px);
		@include pageView(64px, 48px);

		.search {
			padding: 4px 16px 18px !important;

			:deep(.uni-input-input) {
				pointer-events: none;
			}
		}
	}
</style>