<template>
	<view class="chat-action-bar">
		<u-row class="action-row">
			<u-col v-for="item in actionList" :key="item.idx" @click="actionClick(item)" span="3">
				<view class="action-item">
					<view class="action-item-icon">
						<image :src="item.icon" alt="" srcset="" />
					</view>
					<text class="action-item-title">{{ item.title }}</text>
				</view>
			</u-col>
		</u-row>
	</view>
</template>

<script setup>
	import {
		images
	} from "@/utils";

	const actionList = [{
			idx: 0,
			type: "Album",
			title: "相册",
			icon: images.msgToolbarAlbum,
		},
		{
			idx: 1,
			type: "Camera",
			title: "拍摄",
			icon: images.msgToolbarCamera,
		},
		// {
		// 	idx: 2,
		// 	type: "File",
		// 	title: "文件",
		// 	icon: images.msgToolbarFile,
		// },
		{
			idx: 3,
			type: "Location",
			title: "位置",
			icon: images.msgToolbarLoc,
		},
		{
			idx: 4,
			type: "Card",
			title: "名片",
			icon: images.msgToolbarCard,
		},
	];
	const emits = defineEmits(["actionClick"]);
	const actionClick = async (action) => {
		emits("actionClick", action.type);
	}
</script>

<style lang="scss" scoped>
	.chat-action-bar {
		position: relative;
		background: #f0f2f6;
		padding: 12px 18px;

		.action-row {
			flex-wrap: wrap;
			margin-bottom: 12px;
		}

		.action-item {
			display: flex;
			justify-content: center;
			align-items: center;
			flex-direction: column;
			margin-top: 12px;

			image {
				width: 30px;
				height: 30px;
			}

			&-icon {
				display: flex;
				justify-content: center;
				align-items: center;
				width: 56px;
				height: 56px;
				background-color: #fff;
				border-radius: 8px;
			}

			&-title {
				font-size: 12px;
				color: #999;
				margin-top: 6px;
			}
		}
	}
</style>