<template>
	<!-- 	<u-overlay show opacity="1">
	</u-overlay> -->
	<view class="preview-video-box">
		<view class="video-wrapper" v-html="vhtml"></view>
		<!-- <view class="video-wrapper"><video class="video-player" id="videoPlayer" :src="videoUrl" :poster="snapshotUrl" autoplay controls
					@error="videoErrorCallback"></video></view> -->
		<view class="play-action-bar">
			<u-icon @click="back" size="24" color="#fff" name="close-circle"></u-icon>
			<u-icon @click="download" size="24" color="#fff" name="download"></u-icon>
		</view>
		<u-overlay :show="downloading">
			<view class="progress-box">
				<u-line-progress :percentage="percentage" :height="15" activeColor="#3981fc"></u-line-progress>
			</view>
		</u-overlay>
	</view>

</template>

<script setup>
	import {
		onLoad
	} from "@dcloudio/uni-app";
	import {
		onMounted,
		ref,
		nextTick,
		computed
	} from "vue";
	import {
		downloadFile,
		saveImageToPhotosAlbum
	} from "@/utils/uniUtils";

	const videoUrl = ref("");
	const snapshotUrl = ref("");

	onLoad((params) => {
		videoUrl.value = params.videoUrl;
		snapshotUrl.value = params.snapshotUrl;
	});

	const vhtml = computed(() =>
		`<video class="video-player" id="videoPlayer" src="${videoUrl.value}" poster="${snapshotUrl.value}" autoplay controls></video>`
	);

	const downloading = ref(false);
	const percentage = ref(0);
	const back = () => {
		uni.navigateBack();
	};
	const download = async () => {
		if (downloading.value) return;
		downloading.value = true;
		await downloadFile(videoUrl.value, {
			onProgressUpdate: ({
				progress
			}) => {
				percentage.value = progress;
			}
		}).then(res => {
			saveImageToPhotosAlbum(res.tempFilePath);
			setTimeout(() => {
				downloading.value = false;
				percentage.value = 0;
			}, 300);
		});;
	};
</script>

<style lang="scss">
	.preview-video-box {
		display: flex;
		flex-direction: column;
		align-items: center;
		justify-content: center;
		height: 100vh;
		background-color: #000;

		.video-wrapper {
			// display: flex;
			width: 100%;
			// height: 100%;
			// align-items: center;
		}

		.video-player {
			height: 100%;
			width: 100%;
		}

		.play-action-bar {
			width: 100%;
			padding: 0 24px;
			box-sizing: border-box;
			margin-top: 12px;
			display: flex;
			justify-content: space-between;
		}
	}

	.progress-box {
		display: flex;
		align-items: center;
		height: 100%;
		padding: 20%;
	}
</style>