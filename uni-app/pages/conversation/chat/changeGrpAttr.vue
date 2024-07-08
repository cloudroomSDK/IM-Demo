<template>
	<view class="page-view-container">
		<u-navbar :title="$t('chatSetting.changeGrpName')" autoBack border>
			<template #right>
				<text class="main-color" @click="save">{{ $t("save") }}</text>
			</template>
		</u-navbar>
		<view class="bg-witle">
			<view class="change-name">
				<u-input v-model="grpName" border="none" clearable :maxlength="50">
				</u-input>
			</view>
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
		getCurrentInstance,
	} from "vue";
	import api from "@/api";
	import crimsdk from "@cloudroom/crimsdk";
	import {
		appNavigator
	} from "@/utils";

	const groupID = ref("");
	const grpName = ref("");

	onLoad((params) => {
		groupID.value = params.groupID;
		grpName.value = params.grpName;
	});

	const instance = getCurrentInstance().proxy;
	const eventChannel = instance.getOpenerEventChannel();
	const save = async () => {
		try {
			await crimsdk.setGrpInfo({
				groupID: groupID.value,
				groupName: grpName.value,
			});
			uni.$u.$itoast("saveSucc");
			eventChannel.emit('changeAttr', {
				data: {
					type: "changeGrpName",
					grpName: grpName.value
				}
			});
			appNavigator.back();
		} catch (e) {
			uni.$u.$itoast("saveSucc");
		}
	};
</script>

<style lang="scss" scoped>
	.change-name {
		display: flex;
		height: 60px;
		padding: 0 15px;
		border-bottom: 1px solid $uni-border-color;
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