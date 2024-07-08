import appStorage from "@/common/storage";

const platform = uni.getSystemInfoSync().platform.toLowerCase() === "ios" ? 1 : 2;

const post = (url, data, setting = {}) => uni.$u?.http.post(url, JSON.stringify(data), setting);

export default class api {

	static login(params) {
		return post("/account/login", Object.assign(params, {
			password: "",
			platform
		}));
	};

	static getVerifyCode(params) {
		return post("/account/code/verify", params);
	};

	static sendSms(areaCode, phoneNumber) {
		return post("/account/code/send", {
			areaCode,
			phoneNumber,
			platform,
			usedFor: 3
		}, {
			header: {
				token: appStorage.getApiToken(),
			},
		});
	};

	static setSelfUserInfo(params) {
		return post("/user/update", params, {
			header: {
				token: appStorage.getApiToken(),
			},
		});
	}

	static getUserInfo(userID) {
		return post("/user/find/full", {
			userIDs: [userID],
		}, {
			header: {
				token: appStorage.getApiToken(),
			},
		});
	}


	// static searchFriend(keyword) {
	// 	return post("/friend/search", {
	// 		keyword,
	// 		pagination: {
	// 			pageNumber: 1,
	// 			showNumber: 10,
	// 		},
	// 	}, {
	// 		header: {
	// 			token: appStorage.getApiToken(),
	// 		},
	// 	});
	// }

	static searchUserInfo(keyword) {
		return post("/user/search/full", {
			keyword,
			pagination: {
				pageNumber: 1,
				showNumber: 10,
			},
		}, {
			header: {
				token: appStorage.getApiToken(),
			},
		});
	}

}