// 引入配置
import appStorage from "@/common/storage";
import {
	v4
} from "uuid";

// 初始化请求配置
uni.$u.http.setConfig((config) => {
	config.baseURL = appStorage.getConfigModel().businessServer;
	return config;
});

uni.$u.http.interceptors.request.use(
	(config) => {
		config.data = config.data || {};
		config.header = Object.assign(config.header, {
			operationID: v4()
		});
		console.log(`interceptors.request--------------------${config.url}`, config);
		return config;
	},
	(
		config,
	) => Promise.reject(config),
);

uni.$u.http.interceptors.response.use(
	(response) => {
		const data = response.data;
		const {
			errCode
		} = data;
		console.log(`interceptors.response--------------------${response.config.url}`, data);
		if (errCode !== 0) {
			return Promise.reject(data);
		}
		return data.data || {};
	},
	(response) => {
		console.log("interceptors.response--------------------reject", response);
		return Promise.reject(response);
	},
);