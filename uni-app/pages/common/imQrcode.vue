<template>
	<view class="page-view">
		<u-navbar :title="title" autoBack border>
		</u-navbar>
		<view class="im-qrcode">
			<view class="info flex">
				<view class="avatar">
					<im-avatar :src="faceURL"></im-avatar>
				</view>
				<view class="nickname flex-1 flex-self-center truncate">
					{{ showname }}
				</view>
			</view>

			<view class="qrcode">
				<uqrcode canvas-id="imqrcode" :value="qrcodeVal" :size="200" :options="{ margin: 10 }">
				</uqrcode>
			</view>

			<view class="desc">
				<text>{{ desc }}</text>
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
		reactive
	} from "vue";
	import {
		getQrCodeLink
	} from "@/utils";

	const title = ref("");
	const faceURL = ref("")
	const showname = ref("")
	const desc = ref("");
	const qrcodeVal = ref("");
	onLoad((params) => {
		if (params.groupID) {
			title.value = uni.$u.$t("grp.qrcode");
			desc.value = uni.$u.$t("grp.scanQrcode");
		} else {
			title.value = uni.$u.$t("pages.myQrcode");
			desc.value = uni.$u.$t("myQrcode.desc");
		}
		showname.value = params.showname;
		faceURL.value = params.faceURL;
		qrcodeVal.value = getQrCodeLink(params);
	});
</script>

<style lang="scss" scoped>
	.im-qrcode {
		padding: 64px 40px 0;

		.avatar {
			image {
				height: 80px;
				width: 80px;
			}
		}

		.nickname {
			padding: 0 16px;
			font-size: 24px;
			font-weight: 500;
		}

		.qrcode {
			width: 200px;
			height: 200px;
			margin: 40px auto 55px;
		}

		.desc {
			font-size: 16px;
			font-weight: 400;
			color: #666666;
			text-align: center;
		}
	}
</style>