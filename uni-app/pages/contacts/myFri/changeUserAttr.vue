<template>
	<view class="page-view-container">
		<u-navbar :title="title" autoBack border>
			<template #right>
				<text class="main-color" @click="save">{{ $t("save") }}</text>
			</template>
		</u-navbar>
		<view class="bg-witle">
			<view class="change-remark">
				<u-input v-model="remark" border="none" clearable :maxlength="50">
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
		getCurrentInstance
	} from "vue";
	import {
		appNavigator
	} from "@/utils";

	import useContacts from "@/store/contacts";

	const contactsStore = useContacts();
	const title = ref("");
	let userID;
	const remark = ref("");

	onLoad((params) => {
		userID = params.userID;
		if (params.handle === "changeRemark") {
			title.value = uni.$u.$t("userCard.changeRemark");
			remark.value = params.remark;
		}
	});

	const instance = getCurrentInstance().proxy;
	const eventChannel = instance.getOpenerEventChannel();
	const save = async () => {
		await contactsStore.setFriendRemark(userID, remark.value);
		eventChannel.emit('changeAttr', {
			data: {
				type: "changeRemark",
				remark: remark.value
			}
		});
		// appNavigator.toFriCard({
		// 	userID
		// });
		appNavigator.back();
	};
</script>

<style lang="scss" scoped>
	.change-remark {
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