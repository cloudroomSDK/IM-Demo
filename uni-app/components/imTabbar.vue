<template>
	<view class="im-tabbar">
		<view class="im-tabbar--border"></view>
		<view class="im-tabbar-item" v-for="(item, index) in data" :key="item.name" @click="change(item)">
			<view class="im-tabbar-item--icon">
				<image class="im-tabbar-icon" :src="item.name == active ? item.activeIcon : item.inactiveIcon">
				</image>
				<u-badge class="im-tabbar-item--badge" :value="item.badge"></u-badge>
			</view>
			<view class="im-tabbar-item--text" :class="{active: item.name == active}">
				{{ item.text }}
			</view>
		</view>
	</view>
</template>

<script setup name="imTabbar">
	import {
		onLoad,
		onShow
	} from "@dcloudio/uni-app";
	import {
		ref,
		onMounted
	} from "vue";
	import {
		appNavigator,
		images
	} from "@/utils";
	import useConversation from "@/store/conversation";
	import useContacts from "@/store/contacts";
	import {
		storeToRefs
	} from "pinia";

	const conversationStore = useConversation();
	const contactsStore = useContacts();


	const {
		unreadMsgCount
	} = storeToRefs(conversationStore);
	const {
		unreadCount
	} = storeToRefs(contactsStore);

	// const props = defineProps({
	// 	name: {
	// 		type: String,
	// 		default: "conversation"
	// 	}
	// })
	const data = ref([{
		text: "消息",
		name: "conversation",
		route: "/pages/conversation/index",
		activeIcon: images.conversation_active,
		inactiveIcon: images.conversation,
		badge: unreadMsgCount,
	}, {
		text: "通讯录",
		name: "contacts",
		route: "/pages/contacts/index",
		activeIcon: images.contacts_active,
		inactiveIcon: images.contacts,
		badge: unreadCount,
	}, {
		text: "我的",
		name: "mine",
		route: "/pages/mine/index",
		activeIcon: images.mine_active,
		inactiveIcon: images.mine,
		badge: 0,
	}]);
	const active = ref("mine");

	onLoad(() => {
		const pages = getCurrentPages();
		const route = pages[pages.length - 1].route;
		const item = data.value.find(d => d.route === `/${route}`);
		item && (active.value = item.name);
	});

	const change = ({
		name
	}) => {
		switch (name) {
			case "conversation":
				appNavigator.toConversation();
				break;
			case "contacts":
				appNavigator.toContacts();
				break;
			case "mine":
				appNavigator.toMine();
				break;
		}
	};
</script>

<style lang="scss" scoped>
	.im-tabbar {
		display: flex;
		height: $tabbarHeight;
		width: 100%;
		position: fixed;
		bottom: 0;
		left: 0;
		background-color: rgb(255, 255, 255);
		backdrop-filter: none;

		&--border {
			position: absolute;
			left: 0;
			top: 0;
			width: 100%;
			height: 0.0625rem;
			transform: scaleY(0.5);
			background-color: rgba(0, 0, 0, 0.33);
		}

		&-item {
			display: flex;
			flex-direction: column;
			align-items: center;
			justify-content: center;
			flex: 1;
			width: 100%;
			height: 100%;

			&--icon {
				position: relative;
			}

			&--text {
				font-size: 9px;
				font-weight: 400;
				margin-top: 3px;
				color: rgb(142, 154, 176);

				&.active {
					color: $u-primary;
				}
			}

			&--badge {
				position: absolute;
				top: -3px;
				left: 16px;
			}
		}

		&-icon {
			width: 24px;
			height: 24px;
		}
	}
</style>