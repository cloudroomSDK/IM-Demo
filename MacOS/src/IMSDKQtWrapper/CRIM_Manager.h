#ifndef CRIM_MANAGER_QT_H
#define CRIM_MANAGER_QT_H

#include "CRIM_Defines.h"
#include "CRIM_Defines_Ex.h"

#define DECLARED_SIGNALS(MSGNAME, ...)	\
	void s_##MSGNAME##Success(__VA_ARGS__, const QString &cookie = QString()); \
	void s_##MSGNAME##Failed(int code, const QString &error, const QString &cookie = QString());

#define CALLRSP_MSG(MSGNAME, ...)	\
	void m_##MSGNAME##Success(__VA_ARGS__, const QString &cookie = QString()); \
	void m_##MSGNAME##Failed(int code, const QString &error, const QString &cookie = QString());

#define CALLRSP_MSGSLOT(MSGNAME, ...) \
	void slot_##MSGNAME##Success(__VA_ARGS__, const QString &cookie = QString()); \
	void slot_##MSGNAME##Failed(int code, const QString &error, const QString &cookie = QString());

#define NOTIFY_MSG(FUNC, ...) void m_##FUNC(__VA_ARGS__);
#define NOTIFY_MSGSLOT(FUNC, ...) void slot_##FUNC(__VA_ARGS__);

extern CRIMLogLevel g_imLogLevel;

namespace CRIMSDK { class CRIMManager; }
class CRIM_WebAPI;
class CRIMCallbacker_Qt;
class CRIMManager_Qt : public QObject
{
	Q_OBJECT
	friend class CRIMCallbacker_Qt;
public:
	static CRIMManager_Qt *instance();
	/*
	* 获取版本号
	*/
	static QString getSDKVersion();

public://IM SDK接口（与im sdk接口一一对应，基本是异步接口）
	/**
	* 初始化sdk
	* 注：在创建图片，语音，视频，文件等需要路径参数的消息体时，
	* 如果选择的是非全路径方法如：createSoundMessage（全路径方法为：createSoundMessageFromFullPath）,
	* 需要将文件自行拷贝到dbPath目录下，如果此时文件路径为 apath+"/sound/a.mp3"，则参数path的值为：/sound/a.mp3。
	* 如果选择的全路径方法，路径为你文件的实际路径不需要再拷贝。
	*
	* @param apiUrl        SDK的API接口地址。
	* @param skipVerifyCert 是否校验https证书（https协义时此参数才有效）
	* @param storageDir    数据存储目录路径
	* @param logDir        日志存储目录路径
	* @param logLevel      日志等级
	* @return bool   true成功; false失败
	*/
	bool initSDK(const QString &apiUrl, bool skipVerifyCert, const QString &storageDir, const QString &logDir, CRIMLogLevel logLevel = CRIM_DEBUGLEVEL);

	void unInitSDK();

	/*
	* APP ID 登录
	*
	* @param userID       用户ID
	*                  userID来自于自身业务服务器
	* @param appId     APP ID
	*                  可在管理后台，【项目管理】获取。
	* @param appSecret APP Secret
	*                  可在管理后台，【项目管理】获取，MD5后的值。
	*/

	void loginWithAppID(const QString &userID, const QString &appId, const QString &appSecret, const QString &cookie = "");

	/*
	* token 登录
	*
	* @param userID   用户ID
	*              userID来自于自身业务服务器
	* @param token token
	*              token生成器返回的token。
	*/
	void loginWithToken(const QString &userID, const QString &token, const QString &cookie = "");

	/**
	* 登出
	*/
	void logout(const QString &cookie = QString());

	//获取当前服务器时间，单位ms（登录成功后才有效）
	int64_t getCurrentSvrTime();

	/**
	* 查询登录用户
	*/
	const QString& getUserID() const { return m_userID; }
	/**
	* 查询登录平台ID
	*/
	CRIMPlatform getPlatformID() { return m_platform; }

	/************消息******************/
	/*
	* 上传文件到服务器
	*
	* @param filePath 带路径的文件名
	* @param cause 文件分类
	*/
	void uploadFile(const QString &filePath, const QString &cause = QString(), const QString &cookie = QString());

	/**
	* 创建文本消息
	*
	* @param text 内容
	* @return {@link Message}
	*/
	CRIMMessage createTextMsg(const QString &text);

	/**
	* 创建@文本消息
	*
	* @param text           内容
	* @param atUserIDList   用户id列表
	* @param atUserInfoList 被@的用户id跟昵称映射
	* @param quoteMessage   @消息带引用消息
	* @return {@link Message}
	*/
	CRIMMessage createTextAtMsg(const QString &text, const QStringList &atUserIDList, const QList<CRIMAtInfo> &atUserInfoList, const CRIMMessage &quoteMessage = CRIMMessage());

	/**
	* 创建位置消息
	*
	* @param longitude   经度
	* @param latitude    纬度
	* @param description 描述消息
	* @return {@link Message}
	*/
	CRIMMessage createLocationMsg(const QString &description, double longitude, double latitude);

	/**
	* 创建自定义消息
	*
	* @param data        json String
	* @param extension   json String
	* @param description 描述
	* @return {@link Message}
	*/
	CRIMMessage createCustomMsg(const QString &data, const QString &extension, const QString &description);

	/**
	* 创建引用消息
	*
	* @param text    内容
	* @param message 被引用的消息体
	* @return {@link Message}
	*/
	CRIMMessage createQuoteMsg(const QString &text, const CRIMMessage &message);

	/**
	* 创建名片消息
	*
	* @param card CRIMCardElem
	* @return {@link Message}
	*/
	CRIMMessage createCardMsg(const CRIMCardElem &card);

	/**
	* 创建图片消息
	*
	* @param imagePath 绝对路径
	* @return {@link Message}
	*/
	CRIMMessage createImageMsgFromFullPath(const QString &imagePath);

	/**
	* 创建图片消息
	* 例如：自行上传的文件，然后使用返回的url发送消息
	*
	*/
	CRIMMessage createImageMsgByURL(const QString &sourcePath, const CRIMPictureInfo &sourcePicture, const CRIMPictureInfo &bigPicture, const CRIMPictureInfo &snapshotPicture);

	/**
	* 创建视频消息
	*
	* @param videoPath    绝对路径
	* @param videoType    mine type
	* @param duration     时长
	* @param snapshotPath 缩略图绝对路径
	* @return {@link Message}
	*/
	CRIMMessage createVideoMsgFromFullPath(const QString &videoPath, const QString &videoType, qint64 duration, const QString &snapshotPath);

	/*
	* 创建视频消息
	* 例如：自行上传的文件，然后使用返回的url发送消息
	*
	*/
	CRIMMessage createVideoMsgByURL(const CRIMVideoElem &videoElem);

	/**
	* 创建声音消息
	*
	* @param soundPath 绝对路径
	* @param duration  时长
	* @return {@link Message}
	*/
	CRIMMessage createSoundMsgFromFullPath(const QString &soundPath, qint64 duration);

	/*
	* 创建音频消息
	* 例如：自行上传的文件，然后使用返回的url发送消息
	*
	*/
	CRIMMessage createSoundMsgByURL(const CRIMSoundElem &soundElem);

	/**
	* 创建文件消息
	* initSDK时传入了数据缓存路径，如路径：A，这时需要你将声音文件复制到A路径下后，如 A/file/a.txt路径，soundPath的值：“/file/.txt”
	*
	* @param filePath 绝对路径
	* @param fileName 文件名
	* @return {@link Message}
	*/
	CRIMMessage createFileMsgFromFullPath(const QString &filePath, const QString &fileName);

	/*
	* 创建文件消息
	* 例如：自行上传的文件，然后使用返回的url发送消息
	*
	*/
	CRIMMessage createFileMsgByURL(const CRIMFileElem &fileElem);

	/**
	* 创建合并消息
	*
	* @param title       标题
	* @param summaryList 摘要
	* @param messageList 消息列表
	* @return {@link Message}
	*/
	CRIMMessage createMergerMsg(const QList<CRIMMessage> &messageList, const QString &title, const QStringList &summaryList);

	/**
	* 创建自定义表情
	*
	* @param index 位置表情，表情的位置
	* @param data  url表情，json const QString &: {"url":"","width":0,"height":0}
	* @return {@link Message}
	*/
	CRIMMessage createFaceMsg(int index, const QString &data);

	/**
	* 创建转发消息
	*
	* @param message 消息体
	* @return {@link Message}
	*/
	CRIMMessage createForwardMsg(const CRIMMessage &message);

	/**
	* 发送消息
	*
	* @param message         消息体{@link Message}
	* @param recvUid         接受者用户id
	* @param recvGid         群id
	* @param offlinePushInfo 离线推送内容
	*                        onProgress:消息发送进度，如图片，文件，视频等消息
	*/
	void sendMsg(const CRIMMessage &message, const QString &recvUid, const QString &recvGid, const CRIMOfflinePushInfo &offlinePushInfo, const QString &cookie = QString());

	/**
	* 撤回消息
	*
	* @param conversationID 会话ID
	* @param clientMsgID    msg client ID
	*/
	void revokeMsg(const QString &conversationID, const QString &clientMsgID, const QString &cookie = QString());

	/**
	* 标记消息已读
	*
	* @param conversationID 会话id
	* @param clientMsgIDs   消息ids
	* @param callBack
	*/
	void markMsgAsReadByMsgID(const QString &conversationID, const QStringList &clientMsgIDs, const QString &cookie = QString());

	/**
	* 提示对方我正在输入
	*
	* @param userID 用户ID
	* @param msgTip 提示信息
	*/
	void typingStatusUpdate(const QString &userID, const QString &msgTip, const QString &cookie = QString());

	/**
	* 获取某条消息之前的历史消息
	*
	* @param conversationID 会话id
	* @param startClientMsgID 从startClientMsgID开始拉取消息（不包含本条消息）
	*                 第一次拉取20条记录 startClientMsgID="" && count=20 得到 list；
	*                 下一次拉取消息记录参数：startClientMsgID=list.get(0).startClientMsgID && count =20；（startClientMsgID始终为上次拉取的list中的第一条记录的值（时间最旧））。
	* @param count    一次拉取count条
	*/
	void getHistoryMsgList(const QString &conversationID, const QString &startClientMsgID, int count, const QString &cookie = QString());

	/**
	* 获取某条消息之后的历史消息
	* 
	* @param conversationID 会话id
	* @param startClientMsgID 从startClientMsgID开始拉取消息（不包含本条消息）
	*                 第一次拉取20条记录 startClientMsgID="" && count=20 得到 list；
	*                 下一次拉取消息记录参数：startMsg=list.last.startClientMsgID && count =20；（startClientMsgID始终为上次拉取的list中的第后一条记录的值（时间最新））。
	* @param count    一次拉取count条
	*/
	void getHistoryMsgListReverse(const QString &conversationID, const QString &startClientMsgID, int count, const QString &cookie = QString());

	/**
	* 删除一条消息  本地 & 服务器
	*
	* @param conversationID 会话id
	* @param clientMsgID    消息id
	*/
	void deleteMsg(const QString &conversationID, const QString &clientMsgID, const QString &cookie = QString());

	/**
	* 删除一条消息  本地 
	*
	* @param conversationID 会话id
	* @param clientMsgID    消息id
	*/
	void deleteMsgFromLocal(const QString &conversationID, const QString &clientMsgID, const QString &cookie = QString());

	/**
	*  删除所有消息  本地 & 服务器
	*/
	void deleteAllMsg(const QString &cookie = QString());

	/**
	* 删除所有消息  本地
	*/
	void deleteAllMsgFromLocal(const QString &cookie = QString());

	/**
	* 插入单条消息到本地
	*
	* @param message  {@link Message}
	* @param receiver 接收者
	* @param sender   发送者
	*/
	void insertSingleMsgToLocal(const CRIMMessage &message, const QString &receiver, const QString &sender, const QString &cookie = QString());

	/*
	* 插入一条群聊消息到本地
	*
	* @param groupID   群ID
	* @param sendID    发送者ID
	*/
	void insertGrpMsgToLocal(const CRIMMessage &message, const QString &groupID, const QString &sendID, const QString &cookie = QString());
	
	/*
	查找消息列表
	key: conversationID
	value: clientMsgIDList 
	*/
	void findMsgList(const QMap<QString, QStringList>  &findParams, const QString &cookie = QString());

	/**
	* 搜索消息
	*
	* @param conversationID       根据会话查询，如果是全局搜索传null
	* @param keywordList          搜索关键词列表，目前仅支持一个关键词搜索
	* @param keywordListMatchType 关键词匹配模式，1代表与，2代表或，暂时未用
	* @param senderUserIDList     指定消息发送的uid列表 暂时未用
	* @param messageTypeList      搜索的消息类型列表，传空代表所有类型
	* @param searchTimePosition   搜索的起始时间点。默认为0即代表从现在开始搜索。UTC 时间戳，单位：秒
	* @param searchTimePeriod     从起始时间点开始的过去时间范围，单位秒。默认为0即代表不限制时间范围，传24x60x60代表过去一天
	* @param pageIndex            当前页数
	* @param count                每页数量
	*/
	void searchLocalMsgs(const QString &conversationID, const QStringList &keywordList,
		int keywordListMatchType = 2, const QStringList &senderUserIDList = QStringList(), const QList<CRIMMsgType> &messageTypeList = QList<CRIMMsgType>(),
		int searchTimePosition = 0, int searchTimePeriod = 0, int pageIndex = 0, int count = 0xFFFF, const QString &cookie = QString());
	
	/*
	* 修改消息本地 localEx 字段，如：下载文件后设置保存路径等。
	*/
	void setMsgLocalEx(const QString &conversationID, const QString &clientMsgID, const QVariantMap &localEx, const QString &cookie = QString());

	/************会话******************/
	/**
	* 获取会话记录
	*/
	void getAllConversationList(const QString &cookie = QString());

	/**
	* 获取会话记录
	*
	* @param offset 偏移量，每次开始的index值
	* @param count  数量，每次获取的数量总数
	*/
	void getConversationListSplit(int offset, int count, const QString &cookie = QString());
	
	/**
	* 获取会话id；
	* 在从群列表进入聊天窗口后退群，这时候需要根据此方法获取会话id删除会话。
	*
	* @param sessionType: 会话类型
	* @param sourceId:    单聊：UserId；群聊：GroupId
	*/
	QString getConversationIDBySessionType(const QString &sourceId, CRIMConversationType sessionType);

	/**
	* 获取单个会话，如果不存在会自动创建
	*
	* @param sessionType: 会话类型
	* @param sourceId:    单聊：UserId；群聊：GroupId
	*/
	void getOneConversation(const QString &sourceId, CRIMConversationType sessionType, const QString &cookie = QString());

	/**
	* 根据会话id获取多个会话
	*
	* @param conversationIDs 会话ID 集合
	*/
	void getMultipleConversation(const QStringList &conversationIDs, const QString &cookie = QString());

	/*
	* 隐藏会话，不删除聊天记录；
	* 如果有新消息到达，会话会再次出现。
	*/
	void hideConversation(const QString &conversationID, const QString &cookie = QString());

	/*
	* 隐藏会话，不删除聊天记录；
	* 如果有新消息到达，会话会再次出现。
	*/
	void hideAllConversations(const QString &cookie = QString());

	/*
	* 清空一个会话 本地 & 服务器
	* @param conversationID    会话ID
	*/
	void clearConversationAndDeleteAllMsg(const QString &conversationID, const QString &cookie = QString());

	/**
	* 通过会话id删除指定会话
	*
	* @param conversationID 会话ID
	*/
	void deleteConversationAndDeleteAllMsg(const QString &conversationID, const QString &cookie = QString());

	/**
	* 置顶会话
	*
	* @param conversationID 会话ID
	* @param isPinned       true 置顶； false 取消置顶
	**/
	void pinConversation(const QString &conversationID, bool isPinned, const QString &cookie = QString());

	/**
	* 设置草稿
	*
	* @param conversationID 会话ID
	* @param draftText      草稿
	**/
	void setConversationDraft(const QString &conversationID, const QString &draftText, const QString &cookie = QString());

	/**
	* 清除@消息标志位
	*
	* @param conversationID 会话ID
	*/
	void resetConversationGrpAtType(const QString &conversationID, const QString &cookie = QString());

	/**
	* 获取at所有人标识
	*/
	QString getAtAllTag();

	/*
	* 清空未读数
	*
	*/
	void markConversationMsgAsRead(const QString &conversationID, const QString &cookie = QString());

	/**
	* 得到消息未读总数
	*/
	void getTotalUnreadMsgCount(const QString &cookie = QString());

	/***
	* 设置私聊
	* @param conversionID 会话ID
	* @param isPrivate true开启
	* */
	void setConversationPrivateChat(const QString &conversionID, bool isPrivate, int duration, const QString &cookie = QString());

	/**
	* 设置会话免打扰状态
	*
	* @param status 1:屏蔽消息; 2:接收消息但不提示; 0:正常
	*/
	void setConversationRecvMsgOpt(const QString &conversationID, CRIMReceiveMsgOpt status, const QString &cookie = QString());

	/************用户******************/
	/**
	* 修改资料
	*
	* @param nickname    名字
	* @param faceURL     头像
	*/
	void setSelfInfo(const QString &nickname, const QString &faceURL, const QString &ex, const QString &cookie = QString());

	/**
	* 修改我的本地资料, 以便快速触发某此事件，变化相关ui
	*/
	void setSelfLocInfo(const QString &nickname, const QString &faceURL, const QVariantMap &ex);

	/**
	* 获取当前用户信息
	*/
	const CRIMUserInfo& getSelfUserInfo() { return m_selfInfo; }


	/**
	* 根据uid 批量查询用户信息
	* @param uidList 用户id列表
	*/
	void getUsersInfo(const QStringList &uidList, const QString &cookie = QString());

	/**
	* 普通用户获取用户状态
	*/
	void getUserStatus(const QStringList &uidList, const QString &cookie = QString());

	/*
	*  订阅用户的在线状态
	*/
	void subscribeUsersStatus(const QStringList &uidList, const QString &cookie = QString());

	/*
	*  取消订阅用户的在线状态
	*/
	void unsubscribeUsersStatus(const QStringList &uidList, const QString &cookie = QString());

	/*
	*  获取所有已订阅用户的在线状态
	*/
	void getSubscribeUsersStatus(const QString &cookie = QString());

	/************好友******************/

	/**
	* 发送好友申请
	*
	* @param uid        对方userID
	* @param reqMessage 请求消息
	*/
	void addFriend(const QString &uid, const QString &reqMessage, const QString &cookie = QString());
	
	/**
	* 删除好友
	*
	* @param uid  用户ID
	*/
	void deleteFriend(const QString &uid, const QString &cookie = QString());

	/**
	* 修改好友备注
	*
	* @param uid    用户id
	* @param remark 备注名
	*/
	void setFriendRemark(const QString &uid, const QString &remark, const QString &cookie = QString());

	/**
	* 搜索好友
	*/
	QList<CRIMFriendInfo*> searchFriends(const QString &keyword);

	/**
	* 发出好友申请列表
	*/
	void getFriendReqListAsApplicant(int offset, int count, const QString &cookie = QString());

	/**
	* 收到好友申请列表
	*/
	void getFriendReqListAsRecipient(int offset, int count, const QString &cookie = QString());

	/**
	* 接受好友请求
	*
	* @param uid       用户ID
	* @param handleMsg 处理信息
	*/
	void acceptFriendReq(const QString &uid, const QString &handleMsg, const QString &cookie = QString());

	/**
	* 拒绝好友申请
	*
	* @param uid       用户ID
	* @param handleMsg 处理信息
	*/
	void refuseFriendReq(const QString &uid, const QString &handleMsg, const QString &cookie = QString());
	
	/**
	* 黑名单
	*/
	void getBlackList(const QString &cookie = QString());

	/**
	* 加入黑名单
	*
	* @param uid  用户ID
	*/
	void addToBlackList(const QString &uid, const QString &cookie = QString());

	/**
	* 从黑名单删除
	*
	* @param uid  用户ID
	*/
	void removeFromBlackList(const QString &uid, const QString &cookie = QString());

	/************群组******************/
	/**
	* 创建群
	*
	* @param groupName    群名称
	* @param faceURL      群icon
	* @param notification 群公告setGroupInfo
	* @param introduction 群简介
	* @param groupType
	* @param ex           其他信息
	* @param member		  群成员
	*/
	void createGrp(const CRIMGrpCreateInfo &info, const QString &cookie = QString());

	/**
	* 申请加入群组
	*
	* @param gid    群组ID
	* @param reason 请求原因
	* @param joinSource 进群方式
	*/
	void joinGrp(const QString &gid, const QString &reason, CRIMJoinType joinSource, const QString &cookie = QString());

	/**
	* 退群
	*
	* @param gid  群组ID
	*/
	void quitGrp(const QString &gid, const QString &cookie = QString());

	/**
	* 解散群
	*/
	void dismissGrp(const QString &gid, const QString &cookie = QString());

	/**
	* 设置或更新群资料
	*
	* @param groupID      群ID
	* @param groupName    群名称
	* @param faceURL      群icon
	* @param notification 群公告
	* @param introduction 群简介
	* @param ex           其他信息
	*/
	void setGrpInfo(const CRIMGrpInfo &info, const QString &cookie = QString());

	/**
	* 开启群禁言
	*
	* @param gid  群ID
	* @param mute true开启
	*/
	void changeGrpMute(const QString &gid, bool mute, const QString &cookie = QString());

	/**
	* 禁言群成员
	*
	* @param gid     群ID
	* @param uid     群成员userID
	* @param seconds 禁言时间s
	*/
	void changeGrpMemberMute(const QString &gid, const QString &uid, qint64 seconds, const QString &cookie = QString());

	/**
	* 设置群管理员
	*
	* @param groupID   组ID号
	* @param userID    用户ID号
	* @param roleLevel 角色 {@link enums.GroupMemberRole}
	*/
	void setGrpMemberRoleLevel(const QString &groupID, const QString &userID, CRIMGroupMemberRole roleLevel, const QString &cookie = QString());

	/**
	* 群成员之间是否可添加好友
	*
	* @param groupID 群ID
	* @param rule 0: 默认可行 1: 不可行
	*/
	void setGrpLookMemberInfo(const QString &groupID, int rule, const QString &cookie = QString());

	/*
	* 设置群成员资料
	*/
	void setGrpMemberInfo(const CRIMGrpMemberInfo &info, const QString &cookie = QString());

	/**
	* 修改所在群的昵称
	*
	* @param gid           群ID
	* @param uid           群成员userID
	* @param groupNickname 群内显示名称
	*/
	void setGrpMemberNickname(const QString &gid, const QString &uid, const QString &groupNickname, const QString &cookie = QString());

	/*
	获取管理者和拥有者
	@param groupID 群ID
	*/
	void getGrpMemberOwnerAndAdmin(const QString &gid, const QString &cookie = QString());

	/**
	* 设置进群验证（暂不支持）
	*
	* @param groupID          组ID号
	* @param needVerification {@link io.crim.android.sdk.enums.GroupVerification}
	*/
	void setGrpVerification(const QString &groupID, CRIMGroupVerificationType needVerification, const QString &cookie = QString());

	/**
	* 邀请进群
	*
	* @param groupId 群组ID
	* @param uidList 被邀请的用户id列表
	* @param reason  邀请说明
	*/
	void inviteUserToGrp(const QString &groupId, const QStringList &uidList, const QString &reason, const QString &cookie = QString());

	/**
	* 踢出群
	*
	* @param groupId 群组ID
	* @param uidList 被踢出群的用户id列表
	* @param reason  说明
	*/
	void kickGrpMember(const QString &groupId, const QStringList &uidList, const QString &reason, const QString &cookie = QString());

	/**
	* 转让群主
	*
	* @param gid  群组ID
	* @param uid  新拥有者（群主）id
	*/
	void transferGrpOwner(const QString &gid, const QString &uid, const QString &cookie = QString());

	/**
	* 收到群申请列表
	*/
	void getGrpReqListAsRecipient(QStringList groupIDs, int offset, int count, const QString &cookie = QString());

	/**
	* 发出群申请列表
	*/
	void getGrpReqListAsApplicant(QStringList groupIDs, int offset, int count, const QString &cookie = QString());

	/**
	* 接受入群申请
	*
	* @param gid       群ID
	* @param uid       申请入群的用户ID
	* @param handleMsg 说明
	*/
	void acceptGrpReq(const QString &gid, const QString &uid, const QString &handleMsg, const QString &cookie = QString());

	/**
	* 拒绝入群申请
	*
	* @param gid       群ID
	* @param uid       申请入群的用户ID
	* @param handleMsg 说明
	*/
	void refuseGrpReq(const QString &gid, const QString &uid, const QString &handleMsg, const QString &cookie = QString());

	/*
	检测自已是否为目标群成员
	*/
	void isJoinGrp(const QString &gid, const QString &cookie = QString());

	/*
	搜索群成员
	*/
	void searchGrpMembers(const QString &groupID, const QStringList &keywordList, bool isSearchUserID,
		bool isSearchMemberNickname, int offset, int count, const QString &cookie);

	/************邀请******************/
	/**
	* 邀请个人加入音视频
	*/
	void call(const CRIMInvitationInfo &info, const QByteArray &cookie = QByteArray());

	/**
	* 同意某人音视频邀请
	*/
	void acceptCall(const CRIMInvitationInfo &info, const QByteArray &cookie = QByteArray());

	/**
	* 拒绝某人音视频邀请
	*/
	void rejectCall(const CRIMInvitationInfo &info, const QByteArray &cookie = QByteArray());

	/**
	* 邀请者取消音视频通话
	*/
	void cancelCall(const CRIMInvitationInfo &info, const QByteArray &cookie = QByteArray());

	/**
	* 挂断
	*/
	void hangUpCall(const CRIMInvitationInfo &info, const QByteArray &cookie = QByteArray());

public://自定义封装接口（有本地数据缓存）

	/**
	* 获取已加入的群信息列表
	*/
	const QMap<QString, CRIMGrpInfo_Ex>& getJoinedGrpList();

	/**
	* 获取某个群信息
	*/
	const CRIMGrpInfo_Ex* getJoinedGrpInfo(const QString &groupId);
	/**
	* 获取已退出的群信息
	*/
	const CRIMGrpInfo* getQuittedGrpInfo(const QString &groupId);
	/**
	* 获取群成员列表
	*
	* @param groupId 群组ID
	**/
	const QMap<QString, CRIMGrpMemberInfo>& getGrpMembersInfo(const QString &groupId);

	/**
	* 获取群中某个成员信息
	*
	* @param groupId 群组ID
	**/
	const CRIMGrpMemberInfo* getGrpMemberInfo(const QString &groupId, const QString &userId);
	/**
	* 获取群中某个成员名称
	*
	* @param groupId 群组ID
	**/
	QString getGrpMemberDisplayName(const QString &groupId, const QString &userId);


	/**
	* 在所有群中随机获取一个成员信息
	*
	* @param groupId 群组ID
	**/
	const CRIMGrpMemberInfo* getGrpMemberInfoFromAll(const QString &userId);

	/**
	* 根据关键词搜索群组
	*/
	QList<CRIMGrpInfo_Ex*> searchGrps(const QString &keyword);

	/**
	* 好友列表
	*/
	const QHash<QString, CRIMFriendInfo>& getFriendList();
	const CRIMFriendInfo* getFriendInfo(const QString &userID);
	bool isFriend(const QString &userID){ return getFriendInfo(userID) != NULL; }

protected:
	/**
	* 同步群信息，只同步退出的群
	*
	* @param gidList 群ID集合
	*/
	void refreshGrpsInfoFromSDK(const QStringList &gidList, bool bOnlyQuitted = false);
	/**
	* 批量自身信息
	**/
	void getSelfInfoFromSDK(const QString &cookie);

	/**
	* 批量获取群成员信息
	*
	* @param groupId 群组ID
	* @param uidList 群成员ID集合
	**/
	void getGrpMembersInfoFromSDK(const QString &groupId, const QStringList &uidList, const QString &cookie = QString());

	/**
	* 获取群成员
	*
	* @param groupId 群组ID
	* @param filter  过滤成员 1普通成员, 2群主，3管理员，0所有
	* @param offset  偏移量
	* @param count   每页数量
	*/
	void getGrpMemberListFromSDK(const QString &groupId, CRIMGroupMemberFilter filter, int offset, int count, const QVariantMap &cookie = QVariantMap());

	/**
	* 批量获取群资料
	*
	* @param gidList 群ID集合
	*/
	void getSpecifiedGrpsInfoFromSDK(const QStringList &gidList, const QString &cookie = QString());
	/**
	* 获取已加入的群列表
	*/
	void getJoinedGrpListFromSDK(const QString &cookie);

	/**
	* 好友列表
	* 返回的好友里包含了已拉入黑名单的好友
	* 需要根据字段isInBlackList做筛选，isInBlackList==1 已拉入黑名单
	*/
	void getFriendListFromSDK(bool filterBlack, const QString &cookie);

signals://调用接口返回结果（外部）
	DECLARED_SIGNALS(login, const QString &);
	DECLARED_SIGNALS(logout, const QString &);
	void s_uploadFileSuccess(const QString &filePath, const QString &url, const QString &cookie = QString());
	void s_uploadFileFailed(const QString &filePath, int code, const QString &error, const QString &cookie = QString());
	void s_uploadFileProgress(const QString &filePath, qint64 saveBytes, qint64 currentBytes, qint64 totalBytes, const QString &cookie = QString());
	void s_sendMsgSuccess(const CRIMMessage &msg, const QString &cookie = QString());
	void s_sendMsgFailed(const QString &msgID, int code, const QString &error, const QString &cookie = QString());
	void s_sendMsgProgress(const QString &msgID, int progress, const QString &cookie = QString());
	DECLARED_SIGNALS(revokeMsg, const QString &convId, const QString &msgId);
	DECLARED_SIGNALS(markMsgAsReadByMsgID, const QString &convId, const QStringList &msgIds);
	DECLARED_SIGNALS(typingStatusUpdate, const QString &);
	DECLARED_SIGNALS(getHistoryMsgList, const QList<CRIMMessage> &hisMsg, bool isEnd);
	DECLARED_SIGNALS(getHistoryMsgListReverse, const QList<CRIMMessage> &hisMsg, bool isEnd);
	DECLARED_SIGNALS(deleteMsg, const QString &);
	DECLARED_SIGNALS(deleteMsgFromLocal, const QString &);
	DECLARED_SIGNALS(deleteAllMsg, const QString &);
	DECLARED_SIGNALS(deleteAllMsgFromLocal, const QString &);
	DECLARED_SIGNALS(insertSingleMsgToLocal, const QString &);
	DECLARED_SIGNALS(insertGrpMsgToLocal, const QString &);
	DECLARED_SIGNALS(findMsgList, const CRIMSearchResultInfo &);
	DECLARED_SIGNALS(searchLocalMsgs, const CRIMSearchResultInfo &);
	DECLARED_SIGNALS(setMsgLocalEx, const QString &);
	DECLARED_SIGNALS(getAllConversationList, const QList<CRIMConversationInfo> &);
	DECLARED_SIGNALS(getConversationListSplit, const QList<CRIMConversationInfo> &);
	DECLARED_SIGNALS(getOneConversation, const CRIMConversationInfo &);
	DECLARED_SIGNALS(getMultipleConversation, const QList<CRIMConversationInfo> &);
	DECLARED_SIGNALS(hideConversation, const QString &);
	DECLARED_SIGNALS(hideAllConversations, const QString &);
	DECLARED_SIGNALS(clearConversationAndDeleteAllMsg, const QString &conversationID);
	DECLARED_SIGNALS(deleteConversationAndDeleteAllMsg, const QString &conversationID);
	DECLARED_SIGNALS(pinConversation, const QString &);
	DECLARED_SIGNALS(setConversationDraft, const QString &);
	DECLARED_SIGNALS(resetConversationGrpAtType, const QString &);
	DECLARED_SIGNALS(markConversationMsgAsRead, const QString &);
	DECLARED_SIGNALS(getTotalUnreadMsgCount, int count);
	DECLARED_SIGNALS(setConversationPrivateChat, const QString &);
	DECLARED_SIGNALS(setConversationBurnDuration, const QString &);
	DECLARED_SIGNALS(setConversationRecvMsgOpt, const QString &);
	DECLARED_SIGNALS(setSelfInfo, const QString &);
	DECLARED_SIGNALS(getUsersInfo, const QList<CRIMUserInfo> &);
	DECLARED_SIGNALS(getUserStatus, const QList<CRIMUserStatusInfo> &);
	DECLARED_SIGNALS(subscribeUsersStatus, const QList<CRIMUserStatusInfo> &);
	DECLARED_SIGNALS(unsubscribeUsersStatus, const QString &);
	DECLARED_SIGNALS(getSubscribeUsersStatus, const QList<CRIMUserStatusInfo> &);
	DECLARED_SIGNALS(addFriend, const QString &);
	DECLARED_SIGNALS(deleteFriend, const QString &);
	DECLARED_SIGNALS(setFriendRemark, const QString &);
	DECLARED_SIGNALS(getFriendReqListAsApplicant, const QList<CRIMFriendReqInfo> &);
	DECLARED_SIGNALS(getFriendReqListAsRecipient, const QList<CRIMFriendReqInfo> &);
	DECLARED_SIGNALS(acceptFriendReq, const QString &);
	DECLARED_SIGNALS(refuseFriendReq, const QString &);
	DECLARED_SIGNALS(getBlackList, const QList<CRIMBlackListInfo> &);
	DECLARED_SIGNALS(addToBlackList, const QString &);
	DECLARED_SIGNALS(removeFromBlackList, const QString &);
	DECLARED_SIGNALS(createGrp, const CRIMGrpInfo &);
	DECLARED_SIGNALS(joinGrp, const QString &);
	DECLARED_SIGNALS(quitGrp, const QString &);
	DECLARED_SIGNALS(dismissGrp, const QString &);
	DECLARED_SIGNALS(setGrpInfo, const QString &);
	DECLARED_SIGNALS(changeGrpMute, const QString &);
	DECLARED_SIGNALS(changeGrpMemberMute, const QString &);
	DECLARED_SIGNALS(setGrpMemberRoleLevel, const QString &);
	DECLARED_SIGNALS(setGrpLookMemberInfo, const QString &);
	DECLARED_SIGNALS(setGrpApplyMemberFriend, const QString &);
	DECLARED_SIGNALS(setGrpMemberInfo, const QString &);
	DECLARED_SIGNALS(setGrpMemberNickname, const QString &);
	DECLARED_SIGNALS(getGrpMemberOwnerAndAdmin, const QString &);
	DECLARED_SIGNALS(setGrpVerification, const QString &);
	DECLARED_SIGNALS(inviteUserToGrp, const QList<CRIMSimpleResultInfo> &);
	DECLARED_SIGNALS(kickGrpMember, const QList<CRIMSimpleResultInfo> &);
	DECLARED_SIGNALS(transferGrpOwner, const QString &);
	DECLARED_SIGNALS(getGrpReqListAsRecipient, const QList<CRIMGrpReqInfo> &);
	DECLARED_SIGNALS(getGrpReqListAsApplicant, const QList<CRIMGrpReqInfo> &);
	DECLARED_SIGNALS(acceptGrpReq, const QString &);
	DECLARED_SIGNALS(refuseGrpReq, const QString &);
	DECLARED_SIGNALS(isJoinGrp, const QString &);
	DECLARED_SIGNALS(searchGrpMembers, const QString &);
	DECLARED_SIGNALS(call, const CRIMInvitationResultInfo &);
	DECLARED_SIGNALS(acceptCall, const CRIMInvitationResultInfo &);
	DECLARED_SIGNALS(rejectCall, const CRIMInvitationResultInfo &);
	DECLARED_SIGNALS(cancelCall, const CRIMInvitationResultInfo &);
	DECLARED_SIGNALS(hangUpCall, const CRIMInvitationResultInfo &);

signals://服务器通知消息
	/*********初始化*********/
	// 正在连接到服务器，适合在 UI 上展示“正在连接”状态。
	void s_connecting();
	// 已经成功连接到服务器
	void s_connectSuccess();
	// 连接服务器失败，可以提示用户当前网络连接不可用
	void s_connectFailed(int errCode);

	/*********会话*********/
	void s_userTokenInvalid(const char *data);
	//同步会话服务开始
	void s_syncServerStart();
	//同步会话服务完成
	void s_syncServerFinish();
	//同步会话服务失败
	void s_syncServerFailed(int err);
	//同步会话服务进度
	void s_syncServerProgress(int progress);
	// 新增会话, 可刷新会话列表
	void s_newConversation(const QList<CRIMConversationInfo> &list);
	// 会话列表有数据更新, 可刷新会话列表
	void s_conversationChanged(const QList<CRIMConversationInfo> &list);
	//未读消息总数改变
	void s_totalUnreadMsgCountChanged(int count);

	/*********用户信息*********/
	// 当前用户被踢下线，此时可以 UI 提示用户“您已经在其他端登录了当前账号，是否重新登录？”
	void s_kickedOffline();
	// 登录票据已经过期，请使用新签发的 UserSig 进行登录。
	void s_userTokenExpired();
	// 当前登录用户资料变更回调
	void s_selfInfoUpdated(const CRIMUserInfo &oldInfo, const CRIMUserInfo &newInfo);
	// 当前登录用户状态变更回调
	void s_userStatusChanged(const CRIMUserStatusInfo &info);

	/*********消息*********/
	// 收到新消息，界面添加新消息
	void s_recvNewMsgs(const QList<CRIMMessage> &msgs);
	//  对方撤回了消息,需要将消息类型更改为MessageType.REVOKE，然后刷新界面
	void s_recvMsgRevoked(const CRIMRevokedInfo &info);
	//消息已删除
	void s_msgDeleted(const CRIMMessage &msg);
	// 对方已阅读消息回执，需更新界面已读状态
	void s_recv1v1ReadReceipt(const QList<CRIMReadReceiptInfo> &list);
	
	/*********好友关系*********/
	// 好友被添加
	void s_friendAdded(const CRIMFriendInfo &info);
	// 好友被删除
	void s_friendDeleted(const CRIMFriendInfo &info);
	// 朋友的资料发生变化
	void s_friendInfoChanged(const CRIMFriendInfo &info);
	void s_friendInfoChanged2(const CRIMFriendInfo &oldInfo, const CRIMFriendInfo &newInfo);
	// 发出或收到的好友申请被添加
	void s_friendReqAdded(const CRIMFriendReqInfo &info);
	// 发出或收到的好友申请被删除
	void s_friendReqDeleted(const CRIMFriendReqInfo &info);
	// 发出或收到的好友申请已同意
	void s_friendReqAccepted(const CRIMFriendReqInfo &info);
	// 发出或收到的好友申请被拒绝
	void s_friendReqRejected(const CRIMFriendReqInfo &info);
	//拉入黑名单
	void s_blackListAdded(const CRIMBlackListInfo &info);
	//从黑名单删除
	void s_blackListDeleted(const CRIMBlackListInfo &info);

	/*********群组关系*********/
	// 创建群：初始成员收到；邀请进群：被邀请者收到
	void s_joinedGrpAdded(const CRIMGrpInfo &info);
	// 退出群：退出者收到；踢出群：被踢者收到
	void s_joinedGrpDeleted(const CRIMGrpInfo &info);
	// 解散群
	void s_joinedGrpDismissed(const CRIMGrpInfo &info);
	// 组资料变更
	void s_grpInfoChanged(const CRIMGrpInfo &info);
	void s_grpInfoChanged2(const CRIMGrpInfo &oldInfo, const CRIMGrpInfo &newInfo);
	// 组成员信息发生变化
	void s_grpMemberInfoChanged(const CRIMGrpMemberInfo &info);
	void s_grpMemberInfoChanged2(const CRIMGrpMemberInfo &oldInfo, const CRIMGrpMemberInfo &newInfo);
	// 组成员进入
	void s_grpMemberAdded(const CRIMGrpMemberInfo &info);
	// 组成员退出
	void s_grpMemberDeleted(const CRIMGrpMemberInfo &info);
	// 发出或收到的组申请被接受
	void s_grpReqAccepted(const CRIMGrpReqInfo &info);
	// 发出或收到的组申请被拒绝
	void s_grpReqRejected(const CRIMGrpReqInfo &info);
	// 发出或收到的组申请有新增
	void s_grpReqAdded(const CRIMGrpReqInfo &info);
	// 发出或收到的组申请被删除
	void s_grpReqDeleted(const CRIMGrpReqInfo &info);

	/*********邀请*********/
	//被邀请者收到：音视频通话邀请
	void s_notifyCallIn(const CRIMInvitationInfo &info);
	//邀请者收到：被邀请者同意音视频通话
	void s_notifyCallAccepted(const CRIMInvitationInfo &info);
	//邀请者收到：被邀请者拒绝音视频通话
	void s_notifyCallRejected(const CRIMInvitationInfo &info);
	//被邀请者收到：邀请者取消音视频通话
	void s_notifyCallCancelled(const CRIMInvitationInfo &info);
	// 被邀请者（其他端）收到：比如被邀请者在手机拒接，在pc上会收到此回调
	void s_notifyCallAcceptedByOtherDevice(const CRIMInvitationInfo &info);
	// 被邀请者（其他端）收到：比如被邀请者在手机同意，在pc上会收到此回调
	void s_notifyCallRejectedByOtherDevice(const CRIMInvitationInfo &info);
	//被挂断
	void s_notifyCallHungUp(const CRIMInvitationInfo &info);

signals://内部使用消息
	CALLRSP_MSG(login, const QString &);
	NOTIFY_MSG(connectFailed, int errCode);
	NOTIFY_MSG(kickedOffline, int errCode);
	NOTIFY_MSG(syncServerFinish);
	CALLRSP_MSG(getSelfInfo, const CRIMUserInfo &);
	CALLRSP_MSG(getHistoryMsgList, const CRIMAdvancedMsg &hisMsg);
	CALLRSP_MSG(getHistoryMsgListReverse, const CRIMAdvancedMsg &hisMsg);
	//会话
	CALLRSP_MSG(getAllConversationList, const QList<CRIMConversationInfo> &);
	NOTIFY_MSG(newConversation, const QList<CRIMConversationInfo> &list);
	//群
	CALLRSP_MSG(getJoinedGrpList, const QList<CRIMGrpInfo> &);
	CALLRSP_MSG(getGrpMemberList, const QList<CRIMGrpMemberInfo> &);
	CALLRSP_MSG(getSpecifiedGrpsInfo, const QList<CRIMGrpInfo> &infos);
	NOTIFY_MSG(joinedGrpAdded, const CRIMGrpInfo &info);
	NOTIFY_MSG(joinedGrpDeleted, const CRIMGrpInfo &info);
	NOTIFY_MSG(joinedGrpDismissed, const CRIMGrpInfo &info);
	NOTIFY_MSG(grpInfoChanged, const CRIMGrpInfo &info);
	NOTIFY_MSG(grpMemberInfoChanged, const CRIMGrpMemberInfo &info);
	NOTIFY_MSG(grpMemberAdded, const CRIMGrpMemberInfo &info);
	NOTIFY_MSG(grpMemberDeleted, const CRIMGrpMemberInfo &info);

	//好友
	CALLRSP_MSG(getFriendList, const QList<CRIMFriendInfo> &);
	NOTIFY_MSG(friendAdded, const CRIMFriendInfo &info);
	NOTIFY_MSG(friendDeleted, const CRIMFriendInfo &info);
	NOTIFY_MSG(friendInfoChanged, const CRIMFriendInfo &info);

protected slots:
	CALLRSP_MSGSLOT(login, const QString &);
	NOTIFY_MSGSLOT(connectFailed, int errCode);
	NOTIFY_MSGSLOT(kickedOffline, int errCode);
	NOTIFY_MSGSLOT(syncServerFinish);
	CALLRSP_MSGSLOT(getSelfInfo, const CRIMUserInfo &);   
	//会话
	CALLRSP_MSGSLOT(getAllConversationList, const QList<CRIMConversationInfo> &);
	NOTIFY_MSGSLOT(newConversation, const QList<CRIMConversationInfo> &list);
	//群
	CALLRSP_MSGSLOT(getJoinedGrpList, const QList<CRIMGrpInfo> &);
	CALLRSP_MSGSLOT(getGrpMemberList, const QList<CRIMGrpMemberInfo> &);
	CALLRSP_MSGSLOT(getSpecifiedGrpsInfo, const QList<CRIMGrpInfo> &infos);
	NOTIFY_MSGSLOT(joinedGrpAdded, const CRIMGrpInfo &info);
	NOTIFY_MSGSLOT(joinedGrpDeleted, const CRIMGrpInfo &info);
	NOTIFY_MSGSLOT(joinedGrpDismissed, const CRIMGrpInfo &info);
	NOTIFY_MSGSLOT(grpInfoChanged, const CRIMGrpInfo &info);
	NOTIFY_MSGSLOT(grpMemberInfoChanged, const CRIMGrpMemberInfo &info);
	NOTIFY_MSGSLOT(grpMemberAdded, const CRIMGrpMemberInfo &info);
	NOTIFY_MSGSLOT(grpMemberDeleted, const CRIMGrpMemberInfo &info);
	//好友
	CALLRSP_MSGSLOT(getFriendList, const QList<CRIMFriendInfo> &);
	NOTIFY_MSGSLOT(friendAdded, const CRIMFriendInfo &info);
	NOTIFY_MSGSLOT(friendDeleted, const CRIMFriendInfo &info);
	NOTIFY_MSGSLOT(friendInfoChanged, const CRIMFriendInfo &info);
	void slot_crFriendInfoChanged(const CRIMFriendInfo &info);

protected:
	CRIMManager_Qt();
	~CRIMManager_Qt();
	void getHistorySeqDeal(const QString &conversationID);

	void emitSyncFinishedIfAllDataReady();
	void addGrp(const CRIMGrpInfo& info, const QVariantMap &cookie = QVariantMap());
	bool addGrpMember(const CRIMGrpMemberInfo& info);

	bool isKeywordMatchFriend(const QString &keyword, const CRIMFriendInfo& info);
	CRIMMessage createInviteMsg(const CRIMInvitationInfo &info, CRIM_CustomMsgType customType);
	void syncDataToLocal();
	void getTargetInfo(const CRIMInvitationInfo &info, QString &dstGrpID, QString &dstUsrID);
	CRIMOfflinePushInfo makeOfflinePushInfo(const CRIMInvitationInfo &info, CRIM_CustomMsgType signalingType);

protected slots:
	void slot_checkGrpTimeout();

private:
	CRIMSDK::CRIMManager*	m_imsdk{ nullptr };
	volatile bool			m_bInit{ false };

	enum CRIMMgrLoginST
	{
		CRIMMGR_LOGOUT = 0,
		CRIMMGR_LOGINING,
		CRIMMGR_LOGINSYNCDAT,
		CRIMMGR_LOGINSUCCESS,
	};
	volatile CRIMMgrLoginST	m_loginState{ CRIMMGR_LOGOUT };


	CRIMCallbacker_Qt*		m_callBack{ nullptr };
	QString					m_userID;
	CRIMPlatform			m_platform{ CURRENT_CRIMPlatform };

	QString					m_curLoginSession;
	QString					m_usrLoginCookie;
	CRIMUserInfo			m_selfInfo;

	//群信息
	bool							m_bUiGrpInfosGeted{ false };
	QMap<QString, CRIMGrpInfo_Ex>	m_UiGrpInfos;
	QTimer							m_checkGrpTimer;
	QMap<QString, CRIMGrpInfo>		m_quittedInfos;

	//好友信息
	bool							m_bFriendInfoGeted{ false };
	QHash<QString, CRIMFriendInfo>	m_friendInfos;

	
	QString					m_lastConversationID;
	int						m_lastMinSeq{ 0 };

	//缓存数据状态
	enum CacheDataState{
		CACHE_NULL,				//未缓存
		LOGIN_CACHEING,			//登录后缓存中
		LOGIN_CACHEED,			//登录后缓存完成
	};
	CacheDataState		m_cacheDataState{ CACHE_NULL };
	bool				m_bSyncServerFinish{ false };

};

#endif//CRIM_MANAGER_QT_H
