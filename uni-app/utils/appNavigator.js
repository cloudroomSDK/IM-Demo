class UniRouter {
	constructor() {
		// 原始属性定义
		this.config = {
			type: 'navigateTo',
			url: '',
			delta: 1, // navigateBack页面后退时,回退的层数
			params: {}, // 传递的参数
			animationType: 'pop-in', // 窗口动画,只在APP有效
			animationDuration: 300, // 窗口动画持续时间,单位毫秒,只在APP有效
			intercept: false // 是否需要拦截
		}
		// 因为route方法是需要对外赋值给另外的对象使用，同时route内部有使用this，会导致route失去上下文
		// 这里在构造函数中进行this绑定
		this.route = this.route.bind(this)
	}

	// 判断url前面是否有"/"，如果没有则加上，否则无法跳转
	addRootPath(url) {
		return url[0] === '/' ? url : `/${url}`
	}

	// 整合路由参数
	mixinParam(url, params) {
		url = url && this.addRootPath(url)

		// 使用正则匹配，主要依据是判断是否有"/","?","="等，如“/page/index/index?name=mary"
		// 如果有url中有get参数，转换后无需带上"?"
		let query = ''
		if (/.*\/.*\?.*=.*/.test(url)) {
			// object对象转为get类型的参数
			query = uni.$u.queryParams(params, false)
			// 因为已有get参数,所以后面拼接的参数需要带上"&"隔开
			return url += `&${query}`
		}
		// 直接拼接参数，因为此处url中没有后面的query参数，也就没有"?/&"之类的符号
		query = uni.$u.queryParams(params)
		return url += query
	}

	// 对外的方法名称
	async route(options = {}, params = {}) {
		// 合并用户的配置和内部的默认配置
		let mergeConfig = {}

		if (typeof options === 'string') {
			// 如果options为字符串，则为route(url, params)的形式
			mergeConfig.url = this.mixinParam(options, params)
			mergeConfig.type = 'navigateTo'
		} else {
			mergeConfig = uni.$u.deepMerge(this.config, options)
			// 否则正常使用mergeConfig中的url和params进行拼接
			mergeConfig.url = this.mixinParam(options.url, options.params)
		}

		// 如果本次跳转的路径和本页面路径一致，不执行跳转，防止用户快速点击跳转按钮，造成多次跳转同一个页面的问题
		if (mergeConfig.url === uni.$u.page()) return

		if (params.intercept) {
			this.config.intercept = params.intercept
		}
		// params参数也带给拦截器
		mergeConfig.params = params
		// 合并内外部参数
		mergeConfig = uni.$u.deepMerge(this.config, mergeConfig)
		// 判断用户是否定义了拦截器
		if (typeof uni.$u.routeIntercept === 'function') {
			// 定一个promise，根据用户执行resolve(true)或者resolve(false)来决定是否进行路由跳转
			const isNext = await new Promise((resolve, reject) => {
				uni.$u.routeIntercept(mergeConfig, resolve)
			})
			// 如果isNext为true，则执行路由跳转
			isNext && this.openPage(mergeConfig)
		} else {
			if (mergeConfig.delay && mergeConfig.delay > 0) {
				setTimeout(() => {
					this.openPage(mergeConfig)
				}, mergeConfig.delay);
			} else {
				this.openPage(mergeConfig)
			}
		}
	}

	// 执行路由跳转
	openPage(config) {
		console.log(`route: ${config.url}`, config);
		// 解构参数
		const {
			url,
			type,
			delta,
			animationType,
			animationDuration
		} = config
		if (config.type == 'navigateTo' || config.type == 'to') {
			if (config.preload) {
				const pages = getCurrentPages().reverse();
				if (pages.length > 0) {
					if (pages[0].route === url) return;
					const idx = pages.findIndex(page => `/${page.route}` === url);
					if (idx > -1) {
						uni.navigateBack({
							delta: idx
						});
						return;
					}
				}
			}

			const opt = {
				url,
				animationType,
				animationDuration
			}
			if (config.events) {
				opt.events = config.events;
			}
			uni.navigateTo(opt)
		} else if (config.type == 'redirectTo' || config.type == 'redirect') {
			uni.redirectTo({
				url
			})
		} else if (config.type == 'switchTab' || config.type == 'tab') {
			uni.switchTab({
				url
			})
		} else if (config.type == 'reLaunch' || config.type == 'launch') {
			uni.reLaunch({
				url
			})
		} else if (config.type == 'navigateBack' || config.type == 'back') {
			uni.navigateBack({
				delta
			})
		}
	}
}

const uniRoute = (new UniRouter()).route;

export const AppRoutes = {
	Login: "/pages/login/index",
	SetConfig: "/pages/setConfig/index",
	Conversation: "/pages/conversation/index",
	Chat: "/pages/conversation/chat/index",
	GrpChatSetting: "/pages/conversation/chat/grpChatSetting",
	SingleChatSetting: "/pages/conversation/chat/singleChatSetting",
	SearchChatHistory: "/pages/conversation/chat/searchChatHistory",
	SetChatBackground: "/pages/conversation/chat/setChatBackground",
	ChangeGrpAttr: "/pages/conversation/chat/changeGrpAttr",
	Contacts: "/pages/contacts/index",
	NewFri: "/pages/contacts/newFri/index",
	Recipient: "/pages/contacts/newFri/recipient",
	MyFri: "/pages/contacts/myFri/index",
	FriCard: "/pages/contacts/myFri/userCard",
	ChangeUserAttr: "/pages/contacts/myFri/changeUserAttr",
	MyGrp: "/pages/contacts/myGrp/index",
	FriOrGrpSearch: "/pages/contacts/search/index",
	GrpCard: "/pages/contacts/myGrp/grpCard",
	GrpMembers: "/pages/contacts/myGrp/grpMembers",
	GrpMemberCard: "/pages/contacts/myGrp/grpMemberCard",
	AddFri: "/pages/contacts/addFri/index",
	SendAddReq: "/pages/contacts/addFri/sendAddReq",
	JoinGrp: "/pages/contacts/joinGrp/index",
	JoinGrpCard: "/pages/contacts/joinGrp/joinGrpCard",
	SendJoinGrpReq: "/pages/contacts/joinGrp/sendJoinGrpReq",
	JoinGrpById: "/pages/contacts/joinGrp/joinByGrpId",
	CreateGrp: "/pages/contacts/createGrp/index",
	Mine: "/pages/mine/index",
	MyInfo: "/pages/mine/myInfo",
	MyInfoDetail: "/pages/mine/myInfoDetail",
	MineSetting: "/pages/mine/setting",
	About: "/pages/mine/about",
	MyQrcode: "/pages/mine/myQrcode",
	Blacklist: "/pages/mine/blacklist",
	Search: "/pages/common/search",
	ChooseUser: "/pages/common/chooseUser",
	ChooseMember: "/pages/common/chooseMember",
	ImCopyId: "/pages/common/imCopyId",
	ImQrcode: "/pages/common/imQrcode",
	PreviewVideo: "/pages/common/previewVideo",
	PreviewMap: "/pages/common/previewMap",
}

class AppNavigator {

	getCurrentPage() {
		const pages = getCurrentPages();
		const page = pages[pages.length - 1];
		return page ?? null;
	}

	isPage(url) {
		const page = this.getCurrentPage();
		return page ? `/${page.route}` === url : false;
	}

	back(setting = {}) {
		uniRoute({
			type: "navigateBack",
			...setting
		});
	}

	toLogin() {
		uniRoute({
			url: AppRoutes.Login
		});
	}

	toSetConfig() {
		uniRoute({
			url: AppRoutes.SetConfig
		});
	};

	toConversation() {
		uniRoute({
			// type: "switchTab",
			animationDuration: 0,
			animationType: "none",
			url: AppRoutes.Conversation,
			preload: true
		});
	};

	// 聊天页面
	toChat(params, setting = {}) {
		uniRoute({
			...setting,
			url: AppRoutes.Chat,
			params: {
				conversationID: params.conversationID,
				userID: params.userID,
				groupID: params.groupID,
				showName: params.showName || params.nickname || params.groupName,
				faceURL: params.faceURL,
				recommend: params.recommend, // { userID, nickname } // 可选
			}
		});
	};

	toChatSetting({
		userID,
		groupID
	}) {
		if (groupID) {
			uniRoute(AppRoutes.GrpChatSetting, {
				groupID
			});
		} else {
			uniRoute(AppRoutes.SingleChatSetting, {
				userID
			});
		}
	}

	toSearchChatHistory(params) {
		uniRoute(AppRoutes.SearchChatHistory, params);
	}

	toSetChatBackground(params) {
		uniRoute(AppRoutes.SetChatBackground, params);
	}

	toChangeGrpAttr(params, setting = {}) {
		uniRoute({
			url: AppRoutes.ChangeGrpAttr,
			params,
			...setting
		});
	}

	toContacts() {
		uniRoute({
			// type: "switchTab",
			animationDuration: 0,
			animationType: "none",
			url: AppRoutes.Contacts,
			preload: true
		});
	};

	toNewFri() {
		uniRoute({
			url: AppRoutes.NewFri
		});
	}

	toHandleNewFri({
		fromUserID,
		fromNickname,
		fromFaceURL,
		reqMsg,
	}) {
		uniRoute(AppRoutes.Recipient, {
			fromUserID,
			fromNickname,
			fromFaceURL,
			reqMsg,
		});
	}

	toMyFri() {
		uniRoute({
			url: AppRoutes.MyFri
		});
	}

	// { userID, isScan }
	toFriCard(params, setting = {}) {
		uniRoute({
			url: AppRoutes.FriCard,
			params,
			...setting
		});
	}

	toChangeUserAttr(params, setting = {}) {
		uniRoute({
			url: AppRoutes.ChangeUserAttr,
			params,
			...setting
		});
	}

	toMyGrp() {
		uniRoute({
			url: AppRoutes.MyGrp
		});
	}

	// 0 Fri 1 Grp
	toFriOrGrpSearch(type = 0) {
		uniRoute({
			url: AppRoutes.FriOrGrpSearch,
			params: {
				type
			}
		});
	}

	toGrpCard(params, setting = {}) {
		uniRoute({
			...setting,
			url: AppRoutes.GrpCard,
			params,
		});
	}

	toGrpMembers(params) {
		uniRoute({
			url: AppRoutes.GrpMembers,
			params
		});
	}

	toGrpMemberCard(params) {
		uniRoute({
			url: AppRoutes.GrpMemberCard,
			params
		});
	}

	toAddFri() {
		uniRoute({
			url: AppRoutes.AddFri
		});
	}

	toSendAddReq(params) {
		uniRoute({
			url: AppRoutes.SendAddReq,
			params
		});
	}

	toJoinGrp() {
		uniRoute({
			url: AppRoutes.JoinGrp
		});
	}

	toJoinGrpCard(params) {
		uniRoute({
			url: AppRoutes.JoinGrpCard,
			params
		});
	}

	toSendJoinGrpReq(params, setting = {
		type: "redirectTo"
	}) {
		uniRoute({
			...setting,
			url: AppRoutes.SendJoinGrpReq,
			params,
		});
	}

	toJoinGrpById(params) {
		uniRoute({
			url: AppRoutes.JoinGrpById,
			params
		});
	}


	// params.data: [{ userID, nickname, faceURL }]
	toCreateGrp(params, setting = {}) {
		uniRoute({
			...setting,
			url: AppRoutes.CreateGrp,
			params,
		});
	}

	toMine() {
		uniRoute({
			// type: "switchTab",
			animationDuration: 0,
			animationType: "none",
			url: AppRoutes.Mine,
			preload: true
		});
	};

	toMyInfo() {
		uniRoute({
			url: AppRoutes.MyInfo
		});
	}

	toMyInfoDetail(params = {}) {
		uniRoute({
			url: AppRoutes.MyInfoDetail,
			params
		});
	}

	toSetting() {
		uniRoute({
			url: AppRoutes.MineSetting
		});
	}

	toAbout() {
		uniRoute({
			url: AppRoutes.About
		});
	}

	toMyQrcode() {
		uniRoute({
			url: AppRoutes.MyQrcode
		});
	}

	toBlacklist() {
		uniRoute({
			url: AppRoutes.Blacklist
		});
	}

	// common
	toSearch() {
		uniRoute({
			url: AppRoutes.Search
		});
	}

	toChooseUser(params, setting = {}) {
		uniRoute({
			...setting,
			url: AppRoutes.ChooseUser,
			params
		});
	}

	toChooseMember(params, setting = {}) {
		uniRoute({
			...setting,
			url: AppRoutes.ChooseMember,
			params
		});
	}

	toImCopyId(params) {
		uniRoute({
			url: AppRoutes.ImCopyId,
			params
		});
	}

	toImQrcode(params) {
		uniRoute({
			url: AppRoutes.ImQrcode,
			params
		});
	}

	previewVideo(params) {
		uniRoute({
			url: AppRoutes.PreviewVideo,
			params
		});
	}

	previewMap(params, setting = {}) {
		uniRoute({
			url: AppRoutes.PreviewMap,
			params,
			...setting,
		});
	}
}

export const appNavigator = new AppNavigator();