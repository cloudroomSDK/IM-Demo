<template>
	<view class="page-view-container">
		<u-navbar :title="title" autoBack border></u-navbar>
		<view class="bg-witle">
			<u-cell :title="id">
				<template #value>
					<u-button class="copy-btn" type="primary" :text="$t('copy')" @click="copyID"></u-button>
				</template>
			</u-cell>
		</view>
	</view>
</template>

<script setup>
	import {
		onLoad
	} from "@dcloudio/uni-app";
	import {
		ref,
		computed,
	} from "vue";
	import {
		appNavigator
	} from "@/utils";

	const title = ref("");
	const id = ref("");
	onLoad((params) => {
		if (params.groupID) {
			title.value = uni.$u.$t("grp.idNum");
			id.value = params.groupID;
		} else {
			title.value = uni.$u.$t("user.idNum");
			id.value = params.userID;
		}
	});

	const copyID = () => {
		uni.setClipboardData({
			showToast: false,
			data: id.value,
			success: function() {
				uni.$u.$itoast("copySucc");
			},
		});
	};
</script>

<style lang="scss" scoped>
	.copy-btn {
		width: 64px;
		height: 30px;
		line-height: 30px;
	}

	:deep(.u-cell) {
		.u-cell__body {
			padding: 10px 15px 10px 36px;
		}

		.u-cell__title-text {
			font-size: 16px;
			font-weight: 400;
			color: #000;
		}
	}
</style>