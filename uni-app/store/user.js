import {
	defineStore
} from "pinia";
import crimsdk, {
	IMEvents
} from "@cloudroom/crimsdk";
import api from "@/api";

let isInitListener = false;
const useUserStore = defineStore("User", {
	state: () => {
		return {
			userInfo: {}
		}
	},
	getters: {
		userID({
			userInfo
		}) {
			return userInfo.userID;
		},
		nickname({
			userInfo
		}) {
			return userInfo.nickname;
		},
		faceURL({
			userInfo
		}) {
			return userInfo.faceURL;
		}
	},
	actions: {
		initStoreData() {
			this.userInfo = {
				"userID": "",
				"nickname": "",
				"faceURL": "",
				"createTime": 0,
				"ex": "",
				"attachedInfo": "",
				"globalRecvMsgOpt": 0
			};
		},
		initListener() {
			if (isInitListener) {
				return;
			}
			isInitListener = true;
			crimsdk.on(IMEvents.OnSelfInfoUpdated, ({
				data
			}) => {
				this.userInfo = Object.assign({}, this.userInfo, data);
			});

			crimsdk.on(IMEvents.onUserStatusChanged, ({
				data
			}) => {});
		},
		async getSelfUserInfo() {
			const {
				data
			} = await crimsdk.getSelfUserInfo();
			const {
				users
			} = await api.getUserInfo(data.userID);
			this.userInfo = Object.assign({}, data, users[0]);
		},
		async setSelfUserInfo(data) {
			await api.setSelfUserInfo(Object.assign(data, {
				userID: this.userID,
			}));
			// const { users } = await api.getUserInfo(data.userID);
			this.userInfo = Object.assign({}, this.userInfo, data);
		}
	}
})

export default useUserStore;