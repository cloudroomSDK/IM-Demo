<template>
	<view class="page-view-container common-cells">
		<u-navbar :title="$t('pages.createGrp')" autoBack border></u-navbar>

		<u-cell class="create-grp-name" :touch-feed="false" border>
			<template #icon>
				<view class="grp-avatar" @click="selAvatarVisible = true">
					<im-avatar :src="grpFaceURL" :size="52" group></im-avatar>
				</view>
			</template>
			<template #title>
				<u-input class="pl-10" border="none" v-model="grpName" :maxlength="50"
					:placeholder="$t('createGrp.defaultGrpName')"></u-input>
			</template>
		</u-cell>

		<view class="grp-member">
			<view class="grp-member-info flex">
				<view class="flex-1">{{ $t('createGrp.mTitle') }}</view>
				<view class="">
					{{ memberNum }}
				</view>
			</view>
			<view class="members">
				<im-members layout="self,members,add,remove" :members="members" :choose="chooseMember" />
			</view>
		</view>

		<u-button class="create" type="primary" :text="$t('createGrp.create')" @click="createGrp"
			:disabled="disabled"></u-button>

		<u-action-sheet :actions="avatarChoose" closeOnClickOverlay closeOnClickAction :show="selAvatarVisible"
			:round="16" :cancelText="$t('cancel')" @select="selAvatar"
			@close="selAvatarVisible = false"></u-action-sheet>
	</view>
</template>

<script setup>
	import {
		onLoad
	} from "@dcloudio/uni-app";
	import {
		ref,
		computed
	} from "vue";
	import {
		appNavigator,
		images,
		getPurePath
	} from "@/utils";
	import {
		v4 as uuidv4
	} from "uuid";
	import {
		chooseImage,
		chooseVideo,
	} from "@/utils/uniUtils";
	import crimsdk, {
		SessionType,
		GroupVerificationType
	} from "@cloudroom/crimsdk";

	const grpName = ref("");
	const faceURL = ref("");
	const grpFaceURL = computed(() => faceURL.value ? faceURL.value : images.cameraCricle)
	const members = ref([]);
	const memberNum = computed(() => uni.$u.$fprint("createGrp.mNum", [members.value.length + 1]));
	const disabled = computed(() => uni.$u.trim(grpName.value) === "" || members.value.length < 1);

	onLoad((params) => {
		if (params.data) {
			try {
				members.value = JSON.parse(params.data);
			} catch (e) {
				console.log(e);
			}
		}
	});

	const avatarChoose = [{
		name: uni.$u.$t("album"),
		value: "album",
	}, {
		name: uni.$u.$t("photograph"),
		value: "camera",
	}];
	const selAvatarVisible = ref(false);
	const selAvatar = async ({
		value
	}) => {
		try {
			const paths = await chooseImage([value], 1);
			const path = paths[0];
			const fileName = path.slice(path.lastIndexOf("/") + 1);
			const fileType = path.slice(path.lastIndexOf(".") + 1);
			const {
				data: {
					url
				}
			} = await crimsdk.uploadFile({
				name: fileName,
				contentType: fileType,
				uuid: uuidv4(),
				filepath: getPurePath(path),
			});

			faceURL.value = url;
		} catch (e) {
			uni.$u.$itoast("uploadFail");
		}
	};

	const chooseMember = ({
		type,
		data
	}) => {
		for (let i = 0; i < data.length; i++) {
			const item = data[i];
			const idx = members.value.findIndex(mem => mem.userID === item.userID);
			if (type === "add") {
				idx === -1 && members.value.push(item);
			} else {
				idx !== -1 && members.value.splice(idx, 1);
			};
		}
	};

	const createGrp = async () => {
		if (uni.$u.trim(grpName.value) === "") {
			uni.$u.$itoast("createGrp.grpNameIsRequired");
			return;
		}
		if (members.value.length < 1) {
			uni.$u.$itoast("createGrp.chooseMemsIsRequired");
			return;
		}

		const memIds = members.value.reduce((result, mem) => {
			result.push(mem.userID);
			return result;
		}, []);

		try {
			const {
				data
			} = await crimsdk.createGrp({
				groupInfo: {
					groupName: grpName.value,
					faceURL: faceURL.value,
					needVerification: GroupVerificationType.AllNot,
				},
				ownerUserID: crimsdk.getLoginUserID(),
				adminUserIDs: [],
				memberUserIDs: memIds,
			});

			appNavigator.toChat({
				groupID: data.groupID
			}, {
				type: "redirectTo",
				delay: 100,
			});

			uni.$u.$itoast("createGrp.createSucc");
		} catch (e) {
			uni.$u.$itoast("createGrp.createSucc");
		}
	};
</script>

<style lang="scss" scoped>
	.create-grp {
		&-name {
			height: 76px;
			background-color: #fff;

			:deep(.u-cell__body) {
				height: 76px;
			}
		}
	}

	.grp-member {
		margin: 20px 0;
		padding: 0 12px 15px 25px;
		box-sizing: border-box;
		background-color: #fff;

		&-info {
			font-size: 14px;
			line-height: 38px;
		}
	}

	.member-name {
		width: 100%;
		font-size: 12px;
		color: #333;
		margin-top: 8px;
		padding: 0 4px;
		box-sizing: border-box;
		text-align: center;
	}

	.create {
		height: 48px;
	}
</style>