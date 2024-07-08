<template>
	<view class="user-card">
		<u-modal :show="show" showCancelButton asyncClose @confirm="confirm" @cancel="show = false"
			:confirmText="$t('send')">
			<view class="tit">发送给：</view>
			<view class="target-info">
				<im-avatar class="target-info-avatar" :src="data.targetFaceURL"></im-avatar>
				<view class="truncate">{{ data.targetNickname }}</view>
			</view>
			<view class="user-info truncate">
				[个人名片] {{ data.nickname }}
			</view>
			<view class="leave-msg">
				<u-input v-model="msg" placeholder="给朋友留言" border="none" clearable>
				</u-input>
			</view>
		</u-modal>
	</view>
</template>

<script setup name="imSendUserCard">
	import {
		ref,
		reactive
	} from "vue";
	import crimsdk, {
		MessageType,
	} from "@cloudroom/crimsdk";
	import useMsgStore from "@/store/msg";
	const msgStore = useMsgStore();

	const data = reactive({
		userID: "",
		nickname: "",
		faceURL: "",
		targetUserID: "",
		targetNickname: "",
		targetFaceURL: "",
	});

	const show = ref(false);
	const open = ({
		userID,
		nickname,
		faceURL,
		targetUserID,
		targetNickname,
		targetFaceURL,
	}) => {
		data.userID = userID;
		data.nickname = nickname;
		data.faceURL = faceURL;
		data.targetUserID = targetUserID;
		data.targetNickname = targetNickname;
		data.targetFaceURL = targetFaceURL;
		show.value = true;
	};
	const close = () => show.value = false;
	const emits = defineEmits(['done']);

	const msg = ref("");
	const confirm = async () => {
		// 把data.userID的名片发送给data.targetUserID
		const {
			userID,
			nickname,
			faceURL,
			targetUserID
		} = data;
		const message = crimsdk.createCardMsg({
			userID,
			nickname,
			faceURL,
			ex: "",
		});
		await msgStore.sendNewMessage(message, {
			userID: targetUserID,
		});
		// 留言
		if (uni.$u.trim(msg.value) !== '') {
			const leaveMsg = crimsdk.createTextMsg(msg.value);
			await msgStore.sendNewMessage(leaveMsg, {
				userID: targetUserID,
			});
		}
		show.value = false;
		emits("done");
	};

	defineExpose({
		open,
		close
	});
</script>

<style lang="scss">
	.user-card {
		:deep(.u-modal__content) {
			display: block;
			// justify-content: start;
		}

		.tit {
			text-align: left;
		}

		.target-info {
			display: flex;
			align-items: center;
			margin: 20px 0;

			&-avatar {
				width: 30px;
				height: 30px;
				margin-right: 10px;
			}
		}

		.user-info {
			font-size: 14px;
			color: #666;
		}

		.leave-msg {
			margin-top: 20px;
			padding: 5px 10px;
			box-sizing: border-box;
			background-color: #f2f2f2;
		}
	}
</style>