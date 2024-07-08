<template>
	<view class="msg-file-con" @click="openFile">
		<view class="file-info">
			<view class="truncate">{{ msgItem.fileElem.fileName }}</view>
			<view class="file-info-sub truncate">{{ fileInfoSub }}</view>
		</view>
		<view class="file-icon">
			<image :src="fileIcon" mode=""></image>
		</view>
	</view>
</template>

<script setup>
	import {
		ref,
		computed
	} from "vue";
	import crimsdk, {
		MessageStatus,
	} from "@cloudroom/crimsdk";
	import {
		images,
		formatFileSize
	} from "@/utils";
	import {
		isMySelf
	} from "@/utils/imUtils";
	import {
		previewImage
	} from "@/utils/uniUtils";
	const props = defineProps({
		msgItem: {
			type: Object,
			default: {}
		}
	});
	const fileInfoSub = computed(() => {
		const {
			sendID,
			fileElem: {
				fileSize
			},
			status
		} = props.msgItem;
		const statuss = ["chat.sending", "chat.sendSucc", "chat.sendFail"];
		const fileSizeUnit = formatFileSize(fileSize);
		return isMySelf(sendID) ? `${fileSizeUnit} / ${uni.$u.$t(statuss[status])}` : fileSizeUnit;
	});

	const fileIcon = computed(() => {
		const {
			fileElem: {
				fileName
			}
		} = props.msgItem;
		let icon = images.chatUnknown;
		try {
			const ext = /\.(\w+)$/.exec(fileName)[1].toLocaleLowerCase();
			switch (ext) {
				case "excel":
					icon = images.chatExcel;
					break;
				case "ppt":
					icon = images.chatPpt;
					break;
				case "png":
				case "jpg":
				case "jpeg":
				case "gif":
					icon = images.chatImage;
					break;
				case "pdf":
					icon = images.chatPdf;
					break;
				case "word":
					icon = images.chatWord;
					break;
				case "zip":
					icon = images.chatZip;
					break;
			}
		} catch (e) {}
		return icon;
	});

	const docExts = ["doc", "xls", "ppt", "pdf", "docx", "xlsx", "pptx"];
	const imgExts = ["png", "jpg", "jpeg", "gif"];

	const openFile = () => {
		const {
			filePath,
			fileName,
		} = props.msgItem.fileElem;
		let ext = "";
		try {
			ext = fileName.split(".")[1];
		} catch (e) {}
		if (docExts.includes(ext)) {
			uni.openDocument({
				filePath,
				complete(e) {
					console.log(`${e}`)
				}
			});
		} else if (imgExts.includes(ext)) {
			previewImage([filePath], 0);
		} else {
			plus.io.resolveLocalFileSystemURL(filePath, function(entry) {
				plus.runtime.openFile(entry.toLocalURL());
			}, function(e) {
				// uni.$u.toast("打开文件失败");
				uni.$u.toast("暂不支持打开该文件类型");
			});
		}
	}

	// console.log("msgItem", props.msgItem);
</script>

<style lang="scss" scoped>
	.msg-file-con {
		width: 45vw;
		display: flex;
		align-items: center;
		padding: 8px;
		border-radius: 6px;
		background-color: #f0f0f0;

		.file-info {
			flex: 1;
			display: flex;
			flex-direction: column;
			justify-content: space-between;
			overflow: hidden;
		}

		.file-info-sub {
			margin-top: 5px;
			font-size: 12px;
			color: #666;
		}

		.file-icon {
			width: 35px;
			height: 40px;
			margin-left: 10px;

			image {
				width: 100%;
				height: 100%;
			}
		}
	}
</style>