<template>
	<view class="login">
		<!-- flex flex-col flex-justify-space-around -->
		<view class="setting flex flex-justify-end">
			<u-icon @click="appNavigator.toSetConfig" name="setting" color="#333333" size="30"></u-icon>
		</view>

		<view class="logo">
			<image :src="images.logo" alt="" />
		</view>

		<view class="from">
			<view class="title">验证码登录</view>
			<view class="sub-title">未注册的手机号通过验证后自动注册</view>
			<u-form class="base-form" :model="loginModel" :rules="rules" errorType="toast" ref="from">
				<u-form-item prop="phoneNumber">
					<u-input type="number" fontSize="16px" v-model="loginModel.phoneNumber" border="bottom"
						placeholder="请输入手机号码" clearable>
						<template #prefix>
							<view class="flex areacode" @click="showAreaPicker">
								<text class="areacode-content">+{{ loginModel.areaCode }}</text>
							</view>
						</template>
					</u-input>
				</u-form-item>

				<u-form-item prop="verifyCode">
					<u-input type="number" fontSize="16px" v-model="loginModel.verifyCode" border="bottom"
						placeholder="请输入验证码">
						<template #suffix>
							<u-code ref="vCodeRef" @change="codeChange"></u-code>
							<u-text size="16px" @tap="getCode" type="primary" :text="tips"></u-text>
						</template>
					</u-input>
				</u-form-item>
				<p class="private-cloud-tips" v-if="isPrivateCloud">{{ $t("login.privateCloudTip")}}</p>
			</u-form>

			<view class="login-btn">
				<u-button type="primary" text="登录/注册" :loading="logined" loadingText="登录中" @click="imlogin"
					:disabled="!canLogin"></u-button>
			</view>
			<u-picker :itemHeight="44" :show="areaVisible" :defaultIndex="areaDefaultIndex" :columns="columns"
				keyName="label" @cancel="areaVisible = false" @confirm="chooseArea" />
		</view>
	</view>
</template>


<script setup>
	import {
		ref,
		reactive,
		computed
	} from "vue";
	import crimsdk from "@cloudroom/crimsdk";
	import {
		images,
		appNavigator
	} from "@/utils";
	import api from "@/api";
	import md5 from "md5";
	import {
		columns
	} from "@/utils/areaCode";
	import {
		initData
	} from "@/store";
	import appStorage from "@/common/storage";

	const loginModel = reactive(
		process.env.NODE_ENV === "development" ? {
			// phoneNumber: "19900000444",
			phoneNumber: "15800000001",
			// phoneNumber: "15888888888",
			// phoneNumber: "13700000001",
			areaCode: "86",
			verifyCode: "8888",
		} : {
			phoneNumber: "",
			areaCode: "86",
			verifyCode: "",
		});

	const canLogin = computed(() => uni.$u.test.mobile(loginModel.phoneNumber) && !!loginModel.verifyCode);

	const rules = {
		phoneNumber: [{
			type: "string",
			required: true,
			message: "请输入手机号码",
			trigger: ["blur", "change"],
		}, ],
		verifyCode: [{
			type: "string",
			required: true,
			message: "请输入手机验证码",
			trigger: ["blur", "change"],
		}, ]
	};

	// 区号
	const areaVisible = ref(false);
	const areaDefaultIndex = ref([0]);
	const showAreaPicker = () => {
		areaVisible.value = true;
	};
	const chooseArea = ({
		value
	}) => {
		loginModel.areaCode = value[0].value
		areaVisible.value = false;
	};

	// 倒计时
	const vCodeRef = ref(null);
	const tips = ref('');
	const codeChange = (text) => tips.value = text;
	const getCode = async () => {
		const vCodeRefIns = vCodeRef.value;
		if (vCodeRefIns.canGetCode) {
			try {
				uni.showLoading({
					title: '正在获取验证码',
				});
				await api.sendSms(`+${loginModel.areaCode}`, loginModel.phoneNumber);
				uni.hideLoading();
				// 这里此提示会被start()方法中的提示覆盖
				uni.$u.toast('验证码已发送');
				// 通知验证码组件内部开始倒计时
				vCodeRefIns.start();
			} catch (e) {
				uni.$u.toast(`${e.errMsg}: ${e.errCode}`);
			}
		} else {
			uni.$u.toast('倒计时结束后再发送');
		}
	};

	const isPrivateCloud = ref(appStorage.isPrivateCloud());
	const logined = ref(false);
	const imlogin = async () => {
		logined.value = true;
		const configModel = appStorage.getConfigModel();
		try {
			const data = await api.login({
				appID: configModel.appID,
				phoneNumber: loginModel.phoneNumber,
				areaCode: `+${loginModel.areaCode}`,
				verifyCode: loginModel.verifyCode,
			});
			const params = configModel.authType === 0 ? {
				userID: data.userID,
				appID: configModel.appID,
				appSecret: md5(configModel.appSecret),
				token: "",
			} : {
				userID: data.userID,
				appID: configModel.appID,
				appSecret: "",
				token: configModel.token,
			};
			await crimsdk.login(params);
			// 保存登录信息
			appStorage.setLoginResult(data);
			initData();
			logined.value = false;
			appNavigator.toConversation();
		} catch (e) {
			logined.value = false;
			if (e.errCode === 1005) {
				uni.$u.$itoast("errCode.isRegistered");
			} else if (e.errCode === 20006) {
				uni.$u.$itoast("errCode.verifyCodeNotMatch");
			} else {
				uni.$u.toast(`${e.errMsg}: ${e.errCode}`);
			}
		}
	};
</script>

<style lang="scss" scoped>
	.login {
		height: 100vh;
		padding: calc(15px + var(--status-bar-height)) 16px 0;
		box-sizing: border-box;

		.setting {
			height: 64px;
			padding-right: 11px;
		}

		.logo {
			margin-top: 20px;
			margin-bottom: 64px;
			text-align: center;

			image {
				width: 120px;
				height: 120px;
				border-radius: 10px;
				box-shadow: 0px 3px 10px 0px rgba(0, 0, 0, 0.1);
			}
		}

		.from {
			.title {
				font-size: 21px;
				color: #3d3d3d;
				line-height: 30px;
			}

			.sub-title {
				font-size: 15px;
				font-weight: 400;
				color: #666666;
				line-height: 22px;
				margin-bottom: 18px;
			}

			.areacode {
				font-size: 16px;
				border-right: 1px solid #d8d8d8;
				margin-right: 24px;

				&-content {
					font-weight: 400;
					margin-right: 12px;
				}
			}
		}

		.login-btn {
			width: 280px;
			height: 42px;
			margin: 98px auto 0;
			border-radius: 6px;
		}

		.private-cloud-tips {
			font-size: 12px;
			color: #999999;
			padding-top: 5px;
			text-indent: 1em;
		}
	}
</style>