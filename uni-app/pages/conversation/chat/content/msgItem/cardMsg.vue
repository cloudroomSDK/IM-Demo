<template>
	<view class="msg-card-con" @click="toUserCard">
		<view class="card-icon">
			<im-avatar :src="msgItem.cardElem.faceURL" />
		</view>
		<view class="card-info">
			<view class="truncate">{{ msgItem.cardElem.nickname }}</view>
		</view>
	</view>
</template>

<script setup>
	import {
		ref
	} from "vue";
	import {
		appNavigator
	} from "@/utils";

	const props = defineProps({
		msgItem: {
			type: Object,
			default: {}
		}
	});

	const toUserCard = () => {
		appNavigator.toFriCard({
			userID: props.msgItem.cardElem.userID
		});
	}
</script>

<style lang="scss" scoped>
	.msg-card-con {
		width: 45vw;
		display: flex;
		align-items: center;
		padding: 8px;
		border-radius: 6px;
		background-color: #f0f0f0;

		.card-info {
			flex: 1;
			display: flex;
			flex-direction: column;
			justify-content: space-between;
			overflow: hidden;
		}

		.card-icon {
			width: 40px;
			height: 40px;
			margin-right: 10px;

			image {
				width: 100%;
				height: 100%;
			}
		}
	}
</style>