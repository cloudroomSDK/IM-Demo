<template>
	<view class="media-box" @click="clickItem">
		<view v-if="msgTypes.isVoice">voice 暂不支持</view>
		<u-image v-else :showLoading="true" :width="imgWidth" :height="imgHeight" mode="widthFix" :src="imgUrl"
			@click="clickItem">
			<template v-slot:loading>
				<u-loading-icon color="#3981fc"></u-loading-icon>
			</template>
		</u-image>
		<image v-if="msgTypes.isVideo" class="play-icon" :src="images.play" alt="" srcset="" />
		<text v-if="msgTypes.isVideo" class="video-duration">{{ secFormat(msgItem.videoElem.duration) }}</text>
	</view>
</template>

<script setup>
	import {
		ref,
		reactive,
		computed
	} from "vue";
	import {
		MessageType,
		MessageStatus
	} from "@cloudroom/crimsdk";
	import {
		appNavigator,
		images
	} from "@/utils";
	import {
		secFormat
	} from "@/utils/imUtils";
	import {
		previewImage
	} from "@/utils/uniUtils";
	import useMsgStore from "@/store/msg";

	const msgStore = useMsgStore();
	const props = defineProps({
		msgItem: {
			type: Object,
			default: {}
		}
	});

	const msgTypes = reactive({
		isPicture: props.msgItem.contentType === MessageType.PictureMessage,
		isVoice: props.msgItem.contentType === MessageType.VoiceMessage,
		isVideo: props.msgItem.contentType === MessageType.VideoMessage,
	});
	const imgUrl = computed(() => {
		const {
			msgItem
		} = props;
		return msgTypes.isVideo ? msgItem.videoElem.snapshotUrl : msgItem.pictureElem.snapshotPicture?.url ??
			msgItem.pictureElem.sourcePath;
	});
	const imgWidth = 120;
	const imgHeight = computed(() => {
		const {
			msgItem
		} = props;
		try {
			if (msgTypes.isPicture) {
				const {
					width,
					height
				} = msgItem.pictureElem.snapshotPicture;
				return height * (imgWidth / width);
			} else if (msgTypes.isVideo) {
				const {
					snapshotWidth,
					snapshotHeight
				} = msgItem.videoElem;
				return snapshotHeight * (imgWidth / snapshotWidth);
			}
		} catch (e) {}
		return "auto";
	});
	const clickItem = () => {
		const {
			msgItem
		} = props;
		if (msgTypes.isPicture) {
			const idx = msgStore.previewImageList.findIndex(url => msgItem.pictureElem.sourcePicture.url);
			previewImage(msgStore.previewImageList, idx);
		} else if (msgTypes.isVideo) {
			appNavigator.previewVideo({
				videoUrl: msgItem.videoElem.videoUrl,
				snapshotUrl: msgItem.videoElem.snapshotUrl
			});
		} else {
			// {
			// 	uuid: string;
			// 	soundPath: string;
			// 	sourceUrl: string;
			// 	dataSize: number;
			// 	duration: number;
			// }
			console.log(`播音频`);
		}
	};
</script>

<style lang="scss" scoped>
	.media-box {
		position: relative;
		border-radius: 16rpx;
		overflow: hidden;

		.play-icon {
			width: 48px;
			height: 48px;
			position: absolute;
			top: 50%;
			left: 50%;
			transform: translate(-50%, -50%);
		}

		.video-duration {
			position: absolute;
			bottom: 12rpx;
			right: 24rpx;
			color: #fff;
		}
	}
</style>