<script setup>
	import {
		onLaunch,
		onShow,
		onHide
	} from "@dcloudio/uni-app";
	import crimsdk, {
		IMEvents,
		LogLevel,
		LoginStatus,
		MessageReceiveOptType,
		MessageType,
		SessionType,
	} from "@cloudroom/crimsdk";
	import md5 from "md5";
	import appStorage from "@/common/storage";
	import {
		appNavigator,
		getDbDir
	} from '@/utils';
	import {
		initListener,
		initData,
		initListData
	} from "@/store";
	import useConversation from "@/store/conversation";
	const conversationStore = useConversation();

	const configModel = appStorage.getConfigModel();

	onLaunch(() => {
		//#ifdef APP-PLUS
		plus.screen.lockOrientation('portrait-primary');
		//#endif
		setSDKListener();
		initSDK();
	});
	onShow(() => {
		crimsdk.setAppBackgroundStatus(false);
	});
	onHide(() => {
		crimsdk.setAppBackgroundStatus(true);
	});

	const initSDK = async () => {
		const path = await getDbDir();
		const isInit = crimsdk.initSDK({
			sdkServer: configModel.sdkServer,
			dataDir: path, // 数据存储路径
			logLevel: LogLevel.Debug,
			logFilePath: path,
			isLogStandardOutput: true,
			isExternalExtensions: false,
		});
		if (!isInit) {
			//#ifdef APP-PLUS
			plus.navigator.closeSplashscreen();
			//#endif
			uni.$u.toast("初始化失败！");
			return;
		}

		await isLogined();
	};

	const setSDKListener = () => {
		crimsdk.setListenerCallBack({
			onCallNativeRequest(method, arg) {
				console.log(`imsdk request ${method}: `, arg);
			},
			onCallNativeResponse(method, data) {
				console.log(`imsdk response ${method}: `, data);
			},
			onNativeNotifiy(method, data) {
				console.log(`imsdk listener ${method}: `, data);
			}
		});
		initListener();
		const events = {
			// ConnListener
			[IMEvents.OnConnecting]: (data) => {
				conversationStore.connectStatus = 0;
			},
			[IMEvents.OnConnectSuccess]: (data) => {
				conversationStore.connectStatus = 1;
			},
			[IMEvents.OnConnectFailed]: (data) => {
				conversationStore.connectStatus = -1;
			},
			[IMEvents.OnSyncServerStart]: (data) => {
				uni.showLoading({
					title: uni.$u.$t("syncData"),
					mask: true,
				});
				conversationStore.syncServer = true;
			},
			[IMEvents.OnSyncServerFinish]: (data) => {
				uni.hideLoading();
				conversationStore.syncServer = false;
				initListData();
			},
			[IMEvents.OnSyncServerFailed]: (data) => {
				uni.hideLoading();
				uni.$u.$itoast("syncFailed");
				conversationStore.syncServer = false;
			},
			[IMEvents.OnKickedOffline]: (data) => {
				uni.$u.$itoast("kickedOffline");
				appStorage.rmLoginResult();
				appNavigator.toLogin();
			},
			[IMEvents.OnUserTokenExpired]: (data) => {
				uni.$u.$itoast("userTokenExpired");
				appStorage.rmLoginResult();
				appNavigator.toLogin();
			},
		}
		for (const [evt, cb] of Object.entries(events)) {
			crimsdk.on(evt, cb);
		}
	};

	const isLogined = async () => {
		const status = crimsdk.getLoginStatus();
		if (status === LoginStatus.Logged) {
			// 已登录、初始化数据
			initData();
			appNavigator.toConversation();
			return;
		}

		const loginResult = appStorage.getLoginResult();
		if (loginResult.userID && loginResult.imToken) {
			try {
				await crimsdk.login({
					userID: loginResult.userID,
					token: "", // loginResult.imToken,
					appID: configModel.appID,
					appSecret: md5(configModel.appSecret),
				});
				initData();
				appNavigator.toConversation();
			} catch (e) {
				console.log(e);
				appStorage.rmLoginResult();
				plus.navigator.closeSplashscreen();
			}
		} else {
			appNavigator.toLogin();
		}
	};
</script>

<style lang="scss">
	/*每个页面公共css */
	@import '@/style/main.scss';
	@import "@/uni_modules/uview-plus/index.scss";
</style>