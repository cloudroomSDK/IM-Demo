<template>
	<view class="page-view-container">
		<u-navbar title="" autoBack border></u-navbar>

		<view class="common-cells" v-if="!isEmpty">
			<view class="block-container">
				<view class="block-container-box">
					<u-cell class="grp-name" :border="false">
						<template #icon>
							<view class="grp-avatar">
								<im-avatar :src="infomation.faceURL ?? images.head" :size="52"></im-avatar>
							</view>
						</template>
						<template #title>
							<view class="grp-name-title">{{ infomation.groupName }}</view>
							<view class="grp-name-sub">{{ infomation.groupID }}</view>
						</template>
					</u-cell>
				</view>
			</view>

			<view class="block-container">
				<view class="block-container-box">
					<u-cell :title="$t('joinGrp.grpID')" :value="infomation.groupID" isLink @click="toImCopyId">
					</u-cell>
				</view>
			</view>

			<view class="join">
				<u-button class="join-btn" type="primary" :text="$t('joinGrp.joinGrpBtn')" @click="joinGrp"></u-button>
			</view>
		</view>
		<im-empty :text="$t('noGrpData')" v-else />
	</view>
</template>

<script setup>
	import {
		onLoad
	} from "@dcloudio/uni-app";
	import {
		ref
	} from "vue";
	import {
		appNavigator,
		images
	} from "@/utils";
	import crimsdk, {
		GroupJoinSource
	} from "@cloudroom/crimsdk";
	import useContacts from "@/store/contacts";

	const contactsStore = useContacts();
	const isEmpty = ref(false);
	const infomation = ref({});
	let joinSource;
	onLoad((params) => {
		if (!contactsStore.getSpecifiedGrpMemberInfo(params.groupID)) {
			joinSource = params.joinSource;
			infomation.value = {
				groupID: params.groupID
			};
			getGrpInfo(params.groupID);
		} else {
			appNavigator.toGrpCard({
				groupID: params.groupID
			}, {
				type: "redirectTo"
			});
		}
	});

	const getGrpInfo = async (groupID) => {
		try {
			const {
				data
			} = await crimsdk.getSpecifiedGrpsInfo([groupID]);
			if (data.length > 0) {
				infomation.value = data[0]
			} else {
				isEmpty.value = true;
			}
		} catch (e) {
			console.log(e);
		}
	};

	const toImCopyId = () => {
		appNavigator.toImCopyId({
			groupID: infomation.value.groupID
		});
	};

	const joinGrp = async () => {
		appNavigator.toSendJoinGrpReq({
			groupID: infomation.value.groupID,
			joinSource,
		});
	}
</script>

<style lang="scss" scoped>
	.grp-name {
		height: 76px;
		padding: 0 20px;
		justify-content: center;
		background-color: #fff;

		&-title {
			font-size: 16px;
			color: #000;
			padding-left: 10px;
		}

		&-sub {
			font-size: 14px;
			color: #666;
			padding-left: 10px;
		}

		:deep(.u-cell__body) {
			padding: 0;
		}
	}

	.chat-setting {
		padding-bottom: 80px;
		overflow-y: scroll;
	}

	.grp-avatar {
		position: relative;
	}

	.join {
		position: absolute;
		bottom: 0;
		left: 0;
		width: 100%;
		height: 70px;
		padding: 15px;
		box-sizing: border-box;
		background-color: #ffffff;

		&-btn {}
	}
</style>