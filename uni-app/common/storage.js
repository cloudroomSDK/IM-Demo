import config from "@/common/config";

const getObjectStorage = (storageKey) => {
	const value = uni.getStorageSync(storageKey);
	if (value) {
		try {
			return JSON.parse(value);
		} catch (e) {}
	}
	return value;
};

const setObStorage = (storageKey, value) => {
	uni.setStorageSync(storageKey, JSON.stringify(value));
};

const sdkServer = config.sdkServer;
const businessServer = config.businessServer;
const appID = config.appID;
const appSecret = config.appSecret;
const pubSDKServerHost = "sdk.cloudroom.com";

// process.env.NODE_ENV === "development"
const configModel = {
	sdkServer,
	businessServer,
	authType: 0,
	appID,
	appSecret,
	token: "",
};

const configModelKey = "configModel";
const loginResultKey = "loginResult";

class AppStorage {
	// configModel
	getConfigModel() {
		const model = getObjectStorage(configModelKey) || Object.assign({}, configModel);
		try {
			if (model.authType === 1) {
				const express = JSON.parse(atob(model.token.split(".")[1]));
				model.appID = express.appID;
			}
		} catch (e) {}
		return model;
	}
	setConfigModel(nModel) {
		// 如果保存的是token登录，不要改appID
		if (nModel.authType === 1) {
			const model = getObjectStorage(configModelKey) || Object.assign({}, configModel);
			nModel.appID = model.appID;
		}
		setObStorage(configModelKey, nModel);
	}
	resetConfigModel() {
		setObStorage(configModelKey, Object.assign(
			configModel, {
				sdkServer: sdkServer,
				businessServer: businessServer,
				appID: appID,
				appSecret: appSecret,
			}
		));
		return Object.assign({}, configModel);
	}
	// 是否私有云
	isPrivateCloud() {
		const model = getObjectStorage(configModelKey);
		return !model.sdkServer.includes(pubSDKServerHost);
	}

	// loginResultKey
	getApiToken() {
		const loginResult = getObjectStorage(loginResultKey);
		if (loginResult) {
			return loginResult.chatToken;
		}
		return "";
	}
	getLoginResult() {
		return getObjectStorage(loginResultKey);
	}
	setLoginResult(value) {
		setObStorage(loginResultKey, value);
	}
	rmLoginResult() {
		return uni.removeStorage({
			key: loginResultKey,
		});
	}
}

export default new AppStorage();