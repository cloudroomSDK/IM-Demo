#ifndef CRIM_DEFINES_QT_H
#define CRIM_DEFINES_QT_H

#include "CRIMDefine.h"

#if defined(MAC)
const CRIMPlatform CURRENT_CRIMPlatform = CRIM_XOS;
#elif defined(LINUX)
const CRIMPlatform CURRENT_CRIMPlatform = CRIM_LINUX;
#elif defined(WIN32)
const CRIMPlatform CURRENT_CRIMPlatform = CRIM_WIN;
#endif

/**
* 错误码定义
*/
enum CRIM_ERR_DEF
{
	CRIM_OK = 0,

	CRIM_ParseToken = 200,

	CRIM_ThirdPartyCredential = 400,

	CRIM_InBlackList = 600,
	CRIM_NotFriend,

	CRIM_TokenExpired = 701,
	CRIM_TokenInvalid,
	CRIM_TokenMalformed,
	CRIM_TokenNotValidYet,
	CRIM_TokenUnknown,
	CRIM_TokenKicked,

	CRIM_Access = 801,
	CRIM_DB,
	CRIM_Args,
	CRIM_Api,
	CRIM_Data,
	CRIM_Login,
	CRIM_Config,
	CRIM_ThirdParty,

	CRIM_WsRecvConnDiff = 901,
	CRIM_WsRecvConnSame,
	CRIM_WsRecvCode,
	CRIM_WsSendTimeout,
	CRIM_ResourceLoadNotComplete,
	CRIM_NotSupportFunction,

	CRIM_HttpErr_Start = 10000,
	CRIM_HttpTimeout = CRIM_HttpErr_Start,
	CRIM_HttpRspDatErr,	//http响应数据不正常
	CRIM_HttpErr,		//http请求失败
	CRIM_HttpSSLErr,
	CRIM_HttpCmdFailed,	//http命令返回失败
	CRIM_HttpErr_End,

};
//获取错误类型说明（CRIM）
QString getErrDsc(CRIM_ERR_DEF err);

//自定义消息类型
enum CRIM_CustomMsgType
{
	CRCUSTOMMSG_Unknow = 0,

	CRCUSTOMMSG_NewInvitation = 300,                /// 新邀请
	CRCUSTOMMSG_InviteeAccept = 301,                /// 邀请接受
	CRCUSTOMMSG_InviteeReject = 302,                /// 邀请拒绝
	CRCUSTOMMSG_InvitationCancel = 303,             /// 邀请取消
	CRCUSTOMMSG_InvitationHangUp = 304,             /// 邀请挂断
};
//呼叫操作类型
enum CRInvitationOpType
{
	CRIVT_OP_Undef = 0,		//无特殊定义
	CRIVT_OP_Timeout,		//被叫超时无应答
	CRIVT_OP_Busy,			//被叫忙
	CRIVT_OP_Reject,		//被叫拒接

	CRIVT_OP_BUTT
};
const int CRIM_MAX_MSGLEN = 1024 * 8;

/**
* 用户状态
*/
enum USER_STATUS
{
	US_OFFLINE = 0,		// 不在线
	US_ONLINE,			// 在线
	US_BUSY,			// 忙
	US_INMEETING,		// 会议中

	USER_STATUS_BUTT,
};

struct CRIMMessage;
typedef QSharedPointer<CRIMMessage> CRIMMessagePtr;
struct CRIMPictureInfo
{
	/**
	* 唯一ID
	*/
	QString uuid;
	/**
	* 图片类型
	*/
	QString type;
	/**
	* 图片大小
	*/
	qint64 size;
	/**
	* 图片宽度
	*/
	int width;
	/**
	* 图片高度
	*/
	int height;
	/**
	* 图片url地址
	*/
	QString url;
};

struct CRIMPictureElem 
{
	/**
	* 本地资源地址
	*/
	QString sourcePath;
	/**
	* 本地图片详情
	*/
	CRIMPictureInfo sourcePicture;
	/**
	* 大图详情
	*/
	CRIMPictureInfo bigPicture;
	/**
	* 缩略图详情
	*/
	CRIMPictureInfo snapshotPicture;
};

struct CRIMSoundElem 
{
	/**
	* 唯一ID
	*/
	QString uuid;
	/**
	* 本地资源地址
	*/
	QString soundPath;
	/**
	* oss地址
	*/
	QString sourceUrl;
	/**
	* 音频大小
	*/
	qint64 dataSize;
	/**
	* 音频时长 s
	*/
	qint64 duration;
};

struct CRIMVideoElem 
{
	/**
	* 视频本地资源地址
	*/
	QString videoPath;
	/**
	* 视频唯一ID
	*/
	QString videoUUID;
	/**
	* 视频oss地址
	*/
	QString videoUrl;
	/**
	* 视频类型
	*/
	QString videoType;
	/**
	* 视频大小
	*/
	qint64 videoSize;
	/**
	* 视频时长
	*/
	qint64 duration;
	/**
	* 视频快照本地地址
	*/
	QString snapshotPath;
	/**
	* 视频快照唯一ID
	*/
	QString snapshotUUID;
	/**
	* 视频快照大小
	*/
	qint64 snapshotSize;
	/**
	* 视频快照oss地址
	*/
	QString snapshotUrl;
	/**
	* 视频快照宽度
	*/
	int snapshotWidth;
	/**
	* 视频快照高度
	*/
	int snapshotHeight;
};

struct CRIMFileElem 
{
	/**
	* 文件本地资源地址
	*/
	QString filePath;
	/**
	* id
	*/
	QString uuid;
	/**
	* oss地址
	*/
	QString sourceUrl;
	/**
	* 文件名称
	*/
	QString fileName;
	/**
	* 文件大小
	*/
	qint64 fileSize;
};

struct CRIMAtInfo
{
	/*
	* at 成员的id
	*/
	QString atUserID;
	/*
	* at 成员的昵称/群名片
	*/
	QString groupNickname;
};

struct CRIMAtElem 
{
	/**
	* at 消息内容
	*/
	QString text;
	/**
	* 被@的用户id集合
	*/
	QStringList atUserList;
	/**
	* 被@的用户信息集合
	*/
	QList<CRIMAtInfo> atUsersInfo;
	/*
	* at 引用消息
	*/
	CRIMMessagePtr quoteMessage;
	/**
	* 自己是否被@了
	*/
	bool isAtSelf() const;
	/*
	* 是否@全体成员
	*/
	bool isAtAll() const;
};

struct CRIMFaceElem
{
	/**
	* app内嵌表情包，根据index 匹配显示表情图
	* */
	int index;
	/**
	* 其他表情，如url表情，直接加载url即可
	* */
	QString data;
};

struct CRIMLocationElem
{
	/**
	* 位置描述
	*/
	QString description;
	/**
	* 经度
	*/
	double longitude;
	/**
	* 纬度
	*/
	double latitude;
};

struct CRIMInvitationInfo
{
	/**
	* 邀请的唯一标识，拒绝/接受/取消/挂断时应填写对应的邀请标识
	*/
	QString invitationMsgId;
	/**
	* 邀请的初始发起者
	*/
	QString inviterUserID;
	/**
	* 被邀请的群id，单聊时为空
	*/
	QString groupID;
	/**
	* 被邀请者UserID列表, 如果目标是群且为全部成员，列表可为空
	*/
	QStringList inviteeUserIDList;
	/**
	* 房间ID，必须唯一，可以不设置。
	*/
	QString roomID;
	/**
	* video 或者audio
	*/
	QString mediaType;
	bool isVideo(){ return mediaType == "video"; }
	/**
	* 发起时间（秒）
	*/
	qint64 initiateTime{ 0 };
	/**
	* 邀请超时时间（秒）
	*/
	int timeout{ 30 };
	/**
	* 接受时间（秒），用于计算通话时长
	*/
	qint64 acceptTime{ 0 };

	/**
	* 操作类型、可选描述
	*/
	CRInvitationOpType opType{ CRIVT_OP_Undef };
	QString opDesc;

	/**
	* 通话时长（秒）
	*/
	int duration{ 0 };
	void makeDuration();
};


typedef QSharedPointer<CRIMInvitationInfo> CRIMInvitationInfoPtr;
struct CRIMCustomElem 
{
	//自定义内容
	QString data;
	//描述
	QString description;
	//扩展信息
	QVariantMap extension;

public:
	/*以下本地扩展数据*/
	//自定义消息类型
	CRIM_CustomMsgType customType{ CRCUSTOMMSG_Unknow };

	//邀请类型
	bool isInviteType() const;
	CRIMInvitationInfoPtr inviteInfo;
};

struct CRIMMergeElem
{
	/**
	* 标题
	*/
	QString title;
	/**
	* 摘要
	*/
	QStringList abstractList;
	/**
	* 具体选择合并的消息列表
	*/
	QList<CRIMMessagePtr> multiMessage;
};

struct CRIMQuoteElem
{
	/**
	* 回复内容
	*/
	QString text;
	/**
	* 被回复的消息
	*/
	CRIMMessagePtr quoteMessage;
};

struct CRIMGrpInfo;
struct CRIMGrpMemberInfo;
typedef QSharedPointer<CRIMGrpInfo> CRIMGrpInfoPtr;
typedef QSharedPointer<CRIMGrpMemberInfo> CRIMGrpMemberInfoPtr;

struct CRIMBurnAfterReading{
	QString recvID;
	QString sendID;
	QString conversationID;
	bool isPrivate;
};
typedef QSharedPointer<CRIMBurnAfterReading> CRIMBurnAfterReadingPtr;
struct CRIMNotificationElem 
{
	/**
	* 通知内容详情
	*/
	QString detail;
	/**
	* 默认提示内容
	*/
	QString defaultTips;

	/// 以下字段是从detail里面decode出来的
	CRIMGrpMemberInfoPtr opUser;
	CRIMGrpMemberInfoPtr quitUser;
	CRIMGrpMemberInfoPtr entrantUser;
	CRIMGrpMemberInfoPtr groupNewOwner;
	CRIMGrpInfoPtr group;
	QList<CRIMGrpMemberInfoPtr> kickedUserList;
	QList<CRIMGrpMemberInfoPtr> invitedUserList;
	//阅后即焚字段
	CRIMBurnAfterReadingPtr burnAfterReading;
};

struct CRIMGroupHasReadInfo
{
	/**
	* 已读的用户id列表
	*/
	QStringList hasReadUserIDList;
	/**
	* 已读总数
	*/
	int hasReadCount;
	/**
	* 发送此条消息时的群人数
	*/
	int groupMemberCount;
};

#define DEFUALT_BURNDURATION 30

struct CRIMAttachedInfoElem
{
	/**
	* 组信息已读详细
	*/
	CRIMGroupHasReadInfo groupHasReadInfo;
	/**
	* 是否是私聊消息（阅后即焚消息）
	*/
	bool isPrivateChat;
	/**
	* 阅读时长 s
	* 即从hasReadTime时间算起，超过了burnDuration秒触发销毁
	*/
	qint64 burnDuration;
	/**
	* 消息已读时间，单位ms
	*/
	qint64 hasReadTime;

	bool notSenderNotificationPush;
	bool isEncryption;
	bool inEncryptStatus;

	//CRIMUploadProgress uploadProgress;
	CRIMAttachedInfoElem()
	{
		isPrivateChat = false;
		burnDuration = 0;
		hasReadTime = 0;
	}
};

struct CRIMOfflinePushInfo
{
	QString title;
	QString desc;
	QString ex;
	QString iOSPushSound;
	bool iOSBadgeCount;
};

struct CRIMTextElem
{
	QString content;
};

struct CRIMCardElem
{
	QString userID; 
	QString nickname;
	QString faceURL;
	QString ex;
};

struct CRIMTypingElem
{
	QString msgTips;
};

struct CRIMMessage
{
	/**
	* 客户端消息唯一ID， 客户端推荐使用
	*/
	QString clientMsgID;
	/**
	* 消息服务器ID，内部使用
	*/
	QString serverMsgID;
	/**
	* 消息创建时间，单位ms
	*/
	qint64 createTime;
	/**
	* 消息发送时间，单位ms
	*/
	qint64 sendTime;
	/**
	* enum CRIMConversationType
	* 会话类型 
	*/
	CRIMConversationType sessionType;
	/**
	* 发送者ID
	*/
	QString sendID;
	/**
	* 接收者ID
	*/
	QString recvID;
	/**
	* enum CRIMMsgLevel
	* 标识消息是用户级别还是系统级别 100:用户 200:系统
	*/
	CRIMMsgLevel msgFrom;
	/**
	* {enum CRIMMsgType}
	* 消息类型：<br/>
	* 101:文本消息<br/>
	* 102:图片消息<br/>
	* 103:语音消息<br/>
	* 104:视频消息<br/>
	* 105:文件消息<br/>
	* 106:@消息<br/>
	* 107:合并消息<br/>
	* 108:转发消息<br/>
	* 109:位置消息<br/>
	* 110:自定义消息<br/>
	* 111:撤回消息回执<br/>
	* 112:C2C已读回执<br/>
	* 113:正在输入状态
	*/
	CRIMMsgType contentType;
	/**
	* {enums CRIMPlatform}
	* 平台类型 1:ios 2:android 3:windows 4:osx 5:web 6:mini 7:linux
	*/
	CRIMPlatform senderPlatformID;
	/**
	* 发送者昵称
	*/
	QString senderNickname;
	/**
	* 发送者头像
	*/
	QString senderFaceUrl;
	/**
	* 群聊ID
	*/
	QString groupID;
	/**
	* 消息唯一序列号
	*/
	int seq;
	/**
	* 是否已读
	*/
	bool isRead;
	/**
	* {enum CRIMMsgStatus}
	* 消息状态 1:发送中 2:发送成功 3:发送失败 4:已删除 5:已撤回
	*/
	CRIMMsgStatus status;

	/**
	* 离线消息推送内容
	*/
	CRIMOfflinePushInfo offlinePush;
	/**
	* 附加信息
	*/
	QString attachedInfo;
	/**
	* 自定义扩展字段
	*/
	QString ext;
	/**
	* 扩展字段
	*/
	QVariantMap ex;
	/**
	* 本地扩展字段
	*/
	QVariantMap localEx;
	/**
	* 图片信息
	*/
	CRIMPictureElem pictureElem;
	/**
	* 语音信息
	*/
	CRIMSoundElem soundElem;
	/**
	* 视频信息
	*/
	CRIMVideoElem videoElem;
	/**
	* 文件信息
	*/
	CRIMFileElem fileElem;
	/**
	* _@信息
	*/
	CRIMAtElem atTextElem;
	/**
	* 位置信息
	*/
	CRIMLocationElem locationElem;
	/**
	* 自定义信息
	*/
	CRIMCustomElem customElem;
	/**
	* 引用消息
	*/
	CRIMQuoteElem quoteElem;
	/**
	* 合并信息
	*/
	CRIMMergeElem mergeElem;
	/**
	* 通知
	*/
	CRIMNotificationElem notificationElem;

	/**
	* 自定义表情
	*/
	CRIMFaceElem faceElem;

	/**
	* 附加信息 如：群消息已读
	*/
	CRIMAttachedInfoElem attachedInfoElem;

	/**
	* 文本
	*/
	CRIMTextElem textElem;

	/**
	* 个人名片
	*/
	CRIMCardElem cardElem;

	/**
	* 正在输入状态
	*/
	CRIMTypingElem typingElem;
};

//已读信息
struct CRIMReadReceiptInfo 
{
	/**
	* 用户id
	*/
	QString userID;
	/**
	* 组id
	*/
	QString groupID;
	/**
	* 已读消息id
	*/
	QStringList msgIDList;
	/**
	* 阅读时间
	*/
	qint64 readTime;
	/**
	* 标识消息是用户级别还是系统级别 100:用户 200:系统
	*/
	CRIMMsgLevel msgFrom;
	/**
	* {@link enums MessageType}
	* 消息类型：
	* 101:文本消息
	* 102:图片消息
	* 103:语音消息
	* 104:视频消息
	* 105:文件消息
	* 106:@消息
	* 107:合并消息
	* 108:转发消息
	* 109:位置消息
	* 110:自定义消息
	* 111:撤回消息回执
	* 112:C2C已读回执
	* 113:正在输入状态
	*/
	CRIMMsgType contentType;
	//会话类型
	CRIMConversationType sessionType;
};

/// 用户公开信息，主要是基本信息，不包括手机等其它隐私字段
struct CRIMPublicUserInfo
{
	/**
	* 用户id
	*/
	QString userID;
	/**
	* 昵称
	*/
	QString nickname;
	/**
	* 头像
	*/
	QString faceURL;
	/**
	* 扩展字段
	*/
	QVariantMap ex;

	virtual QString getDisplayName() const
	{
		return nickname;
	}

};

/**
* 黑名单信息，黑名单用户基本信息，注意黑名单是双向关系。
*/
struct CRIMBlackListInfo : public CRIMPublicUserInfo
{
	/**
	* 创建时间
	*/
	qint64 createTime;
	/**
	* 添加方式
	*/
	int addSource;
	/**
	* 操作者
	*/
	QString operatorUserID;
	/**
	* 附加信息
	*/
	QString attachedInfo;
};

/**
* 好友信息
*/
struct CRIMFriendInfo : public CRIMPublicUserInfo
{
	/**
	* 好友备注名
	*/
	QString remark;
	/**
	* 创建时间
	*/
	qint64 createTime;
	/**
	* 添加方式
	*/
	int addSource;
	/**
	* 操作者id
	*/
	QString operatorUserID;
	/**
	* 附加信息
	*/
	QString attachedInfo;

	QString getDisplayName() const override
	{
		return remark.isEmpty()? nickname : remark;
	}
	bool operator ==(const CRIMFriendInfo &s2) const
	{
		return (userID == s2.userID
			&& nickname == s2.nickname
			&& faceURL == s2.faceURL
			&& ex == s2.ex
			&& remark == s2.remark
			);
	}
};

///个人信息
struct CRIMUserInfo : public CRIMPublicUserInfo
{
	/// 创建时间
	qint64 createTime;

	/// 附加信息
	QString attachedInfo;

	/// 全局免打扰
	CRIMReceiveMsgOpt globalRecvMsgOpt;
};

///个人状态信息
struct CRIMUserStatusInfo
{
	/// 用户id
	QString userID;

	/// 用户在线的平台ID列表
	QList<CRIMPlatform> platformIDs;
	
	//状态：0 表示离线，1 表示在线
	int status;
};

///好友请求信息
struct CRIMFriendReqInfo
{
	/**
	* 请求用户id
	*/
	QString fromUserID;
	/**
	* 请求用户昵称
	*/
	QString fromNickname;
	/**
	* 请求用户头像
	*/
	QString fromFaceURL;
	/**
	* 接受用户id
	*/
	QString toUserID;
	/**
	* 接受用户昵称
	*/
	QString toNickname;
	/**
	* 接受用户头像
	*/
	QString toFaceURL;
	/**
	* 处理结果
	*/
	CRIMReqStatus handleResult;
	/**
	* 请求备注
	*/
	QString reqMsg;
	/**
	* 创建时间
	*/
	qint64 createTime;
	/**
	* 处理者id
	*/
	QString handlerUserID;
	/**
	* 处理备注
	*/
	QString handleMsg;
	/**
	* 处理时间
	*/
	qint64 handleTime;
	/**
	* 扩展字段
	*/
	QVariantMap ex;
};

///群组基础信息
struct CRIMGrpBaseInfo
{
	/**
     * 群类型
     */
	CRIMGroupType groupType;
	/**
	* 群名
	*/
	QString groupName;
	/**
	* 群公告
	*/
	QString notification;
	/**
	* 群简介
	*/
	QString introduction;
	/**
	* 群头像
	*/
	QString faceURL;
	/**
	* 扩展字段
	*/
	QVariantMap ex;

};

///群组创建信息
struct CRIMGrpCreateInfo 
{
	/**
	* 群信息
	*/
	CRIMGrpBaseInfo groupInfo;
	/**
	* 群成员
	*/
	QStringList memberUserIDs;
	/**
	* 群管理员
	*/
	QStringList adminUserIDs;
	/**
	* 群主
	*/
	QString ownerUserID;
};

///群组信息
struct CRIMGrpInfo : public CRIMGrpBaseInfo
{
	/**
	* 组ID
	*/
	QString groupID;
	/**
	* 群主id
	*/
	QString ownerUserID;
	/**
	* 创建时间
	*/
	qint64 createTime;
	/**
	* 创建者id
	*/
	QString creatorUserID;
	/**
	* 群成员数量
	*/
	int memberCount;
	/**
	* 群状态：0正常，1被封，2解散，3禁言
	*/
	CRIMGroupStatus status;
	/**
	* 进群验证方式 （不支持）
	*/
	CRIMGroupVerificationType needVerification;
	/**
	* 不允许通过群获取成员资料 0:允许，1：不允许
	*/
	int lookMemberInfo;
	/**
	* 不允许通过群添加好友 0:允许，1：不允许
	*/
	int applyMemberFriend;
	/**
	* 通知更新时间
	*/
	qint64 notificationUpdateTime;
	/**
	* 通知更新人
	*/
	QString notificationUserID;
};


/// 群成员信息
struct CRIMGrpMemberInfo : public CRIMPublicUserInfo
{
	/**
	* 群id
	*/
	QString groupID;
	/**
	* 入群时间
	*/
	qint64 joinTime;
	/**
	* 加入方式,2- 通过邀请,3- 通过搜索,4- 通过二维码
	*/
	CRIMJoinType joinSource;
	/**
	* 操作者id
	*/
	QString operatorUserID;
	/**
	* 禁言结束时间
	*/
	qint64 muteEndTime;
	/**
	* 邀请人id
	*/
	QString inviterUserID;
	/**
	* 群角色
	*/
	CRIMGroupMemberRole roleLevel;
	/**
	* 是否为群主
	*/
	bool isOwner() const;
	/**
	* 是否为管理员
	*/
	bool isManager() const;

	bool operator ==(const CRIMGrpMemberInfo &s2) const
	{
		return (userID == s2.userID
			&& roleLevel == s2.roleLevel
			&& groupID == s2.groupID
			&& nickname == s2.nickname
			&& faceURL == s2.faceURL
			&& ex == s2.ex);
	}
};

///申请进群信息
struct CRIMGrpReqInfo
{
	/**
	* 群id
	*/
	QString groupID;
	/**
	* 群名
	*/
	QString groupName;
	/**
	* 群头像
	*/
	QString groupFaceURL;
	/**
	* 申请者的id
	*/
	QString userID;
	/**
	* 申请者的昵称
	*/
	QString nickname;
	/**
	* 申请者的头像
	*/
	QString userFaceURL;
	/**
	* -1：拒绝，1：同意
	*/
	CRIMReqStatus handleResult;
	/**
	* 申请原因
	*/
	QString reqMsg;
	/**
	* 申请时间
	*/
	qint64 reqTime;
	/**
	* 处理结果描述
	*/
	QString handledMsg;
	/**
	* 处理者的id
	*/
	QString handleUserID;
	/**
	* 处理时间
	*/
	qint64 handledTime;
	/**
	* 扩展字段
	*/
	QVariantMap ex;
	/**
	* 加入方式,2- 通过邀请,3- 通过搜索,4- 通过二维码
	*/
	CRIMJoinType joinSource;
	/**
	* 邀请进群用户ID
	*/
	QString inviterUserID;
};

///通用结果信息
struct CRIMSimpleResultInfo
{
	QString userID;
	/*
	* checkFriend: result为1表示好友（并且不是黑名单）
	*/
	/*
	* inviteUserToGrp、kickGrpMember: result为1表示成功
	*/
	int result;
};

struct CRIMNotDisturbInfo
{
	/**
	* 会话id
	* */
	QString conversationId;
	/**
	* 免打扰状态
	* 1:屏蔽消息; 2:接收消息但不提示; 0:正常
	* */
	int result;
};

/// 会话信息
struct CRIMConversationInfo
{
	/**
	* 会话id
	*/
	QString conversationID;
	/**
	* 会话类型 1:单聊 2:群聊
	*/
	CRIMConversationType conversationType;
	/**
	* 会话对象用户ID
	*/
	QString userID;
	/**
	* 会话群聊ID
	*/
	QString groupID;
	/**
	* 会话显示名称
	*/
	QString showName() const;
	/**
	* 会话原始名称
	*/
	QString orgName;

	/**
	* 用户头像或群聊头像
	*/
	QString faceURL;
	/**
	* 接收消息选项：<br/>
	* 0:在线正常接收消息，离线时进行推送 <br/>
	* 1:不会接收到消息<br/>
	* 2:在线正常接收消息，离线不会有推送
	*/
	CRIMReceiveMsgOpt recvMsgOpt;
	/**
	* 未读消息数量
	*/
	int unreadCount;
	/**
	* 强提示 如at消息，公告 {@link enums.GroupAtType}
	*/
	CRIMGroupAtType groupAtType;
	/**
	* 最后一条消息 消息对象json字符串
	*/
	CRIMMessagePtr latestMsg;
	/**
	* 最后一条消息发送时间(ms)
	*/
	qint64 latestMsgSendTime;
	/**
	* 会话草稿
	*/
	QString draftText;
	/**
	* 会话草稿设置时间
	*/
	qint64 draftTextTime;
	/**
	* 是否置顶
	*/
	bool isPinned;
	/**
	* 是否开启私聊
	*/
	bool isPrivateChat;
	/**
	* 是否还在组内，如果退群返回true
	*/
	bool isNotInGroup;
	/**
	* 阅读时长 s，即超过了burnDuration秒触发销毁
	*/
	qint64 burnDuration;
	/**
	* 附加信息
	*/
	QString attachedInfo;
	/**
	* 扩展预留字段
	*/
	QVariantMap ex;
	CRIMConversationInfo()
	{
		latestMsgSendTime = 0;
		isPrivateChat = false;
		burnDuration = 0;
	}
};

struct CRIMSearchResultItem
{
	/**
	* 会话ID
	*/
	QString conversationID;

	/**
	* 这个会话下的消息数量
	*/
	int messageCount;
	/**
	* 显示名
	*/
	QString showName;

	/**
	* 头像
	*/
	QString faceURL;
	/**
	* Message的列表
	*/
	QList<CRIMMessagePtr> messageList;
};
	
struct CRIMSearchResultInfo
{
	/**
	* 获取到的总的消息数量
	**/
	int totalCount;
	QList<CRIMSearchResultItem> items; //findResultItems、searchResultItems

	CRIMSearchResultInfo() { totalCount = 0; }
};

struct CRIMAdvancedMsg 
{
	int errCode;
	QString errMsg;
	int lastMinSeq;
	bool isEnd;
	QList<CRIMMessage> messageList;
};

struct CRIMRevokedInfo
{
	/**
	* 撤回者ID
	*/
	QString revokerID;

	/**
	* 撤回者群角色
	*/
	CRIMGroupMemberRole revokerRole;

	/**
	* 撤回者昵称
	*/
	QString revokerNickname;
	/**
	* 消息id
	*/
	QString clientMsgID;
	/**
	* 撤回时间
	*/
	qint64 revokeTime;
	/**
	* 消息发送时间
	*/
	qint64 sourceMessageSendTime;
	/**
	* 消息发送者
	*/
	QString sourceMessageSendID;
	/**
	* 消息发送者昵称
	*/
	QString sourceMessageSenderNickname;
	/**
	* 会话类型
	*/
	CRIMConversationType sessionType;
};


struct CRIMInvitationResultInfo
{
	/*
	*  token
	*/
	QString token;
	/*
	*  房间ID，必须唯一，可以不设置。
	*/
	QString roomID;
	/*
	*   直播地址
	*/
	QString liveURL;
};

struct CRIMGetFriendReqListAsRecipientReq
{
	QList<int> handleResults;
	int offset;
	int count;
};
struct CRIMGetFriendReqListAsApplicantReq
{
	int offset;
	int count;
};
struct CRIMGetGroupReqListAsRecipientReq
{
	QStringList groupIDs;
	QList<int> handleResults;
	int offset;
	int count;
};
struct CRIMGetGroupReqListAsApplicantReq
{
	QStringList groupIDs;
	QList<int> handleResults;
	int offset;
	int count;
};
struct CRIMUpdateFriendsReq
{
	QString ownerUserID;
	QStringList friendUserIDs;
	QString remark;
	QString ex;
	bool isPinned;
};

extern CRIMLogLevel g_imLogLevel;

#define CRIMMgrLogErr(fmt, ...) if( g_imLogLevel>=CRIM_ERRORLEVEL ) OutPutLog(LOGLEVEL_ERR, "CRIMMgr", fmt, ##__VA_ARGS__)
#define CRIMMgrLogWarn(fmt, ...) if( g_imLogLevel>=CRIM_WARNLEVEL ) OutPutLog(LOGLEVEL_WARN, "CRIMMgr", fmt, ##__VA_ARGS__)
#define CRIMMgrLogDebug(fmt, ...) if( g_imLogLevel>=CRIM_DEBUGLEVEL ) OutPutLog(LOGLEVEL_DEBUG, "CRIMMgr", fmt, ##__VA_ARGS__)

#endif // CRIM_DEFINES_QT_H
