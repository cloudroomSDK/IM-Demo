#ifndef CHATPUBLIC_H
#define CHATPUBLIC_H

const QString g_atTextFormat = QString(" @%1 ");
namespace ChatPublic{
enum UIFileState
{
	UFS_NULL = -1,			//空状态
	UFS_NORMAL = 0,		//正常状态，即本地有文件
	UFS_SENDING,		//发送中
	UFS_SEND_PAUSED,	//发送暂停（暂不提供）		
	UFS_SEND_FAILED,	//发送失败	

	UFS_NORECV,			//未接收，即消息已接收，但本地没有文件
	UFS_RECVING,		//接收中
	UFS_RECV_PAUSED,	//接收暂停（暂不提供）		
	UFS_RECV_FAILED,	//接收失败
};
struct UIMsgInfo : public CRIMMessage
{
	//空消息类型的默认高度，只对MSGTYPE_NULL生效
	int defHeightForNull;
	//表情资源，方便内部区分是否为表情
	QList<QUrl> faceUrls;
	//文件状态、进度（文件，视频等）
	UIFileState fileState;
	float fileProgress;
	//高亮文本
	QString highlighterText;
	//倒计时（适用于阅后即焚），-1时不显示
	int countDown;

	UIMsgInfo()
	{
		contentType = CRIM_MsgType_NULL;
		defHeightForNull = 0;
		fileState = UFS_NORMAL;
		fileProgress = 0.0;
		countDown = -1;
	}
	UIMsgInfo(const CRIMMessage &info) : UIMsgInfo()
	{
		fromCRIMMsgInfo(info);
	}
	UIMsgInfo& operator =(const CRIMMessage &other)
	{
		fromCRIMMsgInfo(other);
		return *this;
	}
	//更改消息类型
	void updateMsgType(CRIMMsgType type);
	const QString &getMsgID() const;
	const QString &getSenderID() const;
	const QString &getSenderName() const;
	//绘制内容，转换后的内容
	void setDisplayContent(const QString &displayContent);
	const QString &getDisplayContent() const;
	//原始内容，转换前的内容
	const QString &getOrgContent() const;
	QPixmap getSenderFace(const QSize &sz);

	//消息状态
	CRIMMsgStatus getMsgState() const;
	//是否为自己发送消息
	bool isMyMsg(bool includeOtherPlatform = true) const;
	//是否为正常消息
	static bool isNormalMsg(CRIMMsgType type);
	bool isNormalMsg() const{ return isNormalMsg(contentType); }
	//是否为系统消息
	static bool isSystemMsg(CRIMMsgType type);
	bool isSystemMsg() const{ return isSystemMsg(contentType); }
	//是否为正常消息
	static bool isNotificationMsg(CRIMMsgType type);
	bool isNotificationMsg() const{ return isNotificationMsg(contentType); }
	//创建时间
	QDateTime getCreateTime() const;
	//发送时间
	QDateTime getSendTime() const;
	//是否已读
	bool hasRead() const;
	//是否at自己
	bool isAtMe() const;
	//获取存储目录
	QString getFileStorageDir() const;
	//获取服务器资源路径（图片，语音，视频，文件）
	const QString &getDownloadUrl() const;
	//设置本地资源路径（文件）
	void setLocFilePath(const QString &fileName, const QString &conversationID);
	//获取本地资源路径（图片，语音，视频，文件）
	QString getLocFilePath() const;
	//获取预览图资源路径（图片，视频）
	QString getSnapshotFilePath() const;
	//获取文件名
	QString getFileName() const;
	//获取文件大小
	qint64 getFileSize() const;
	//获取预览文本
	QString getMsgPreviewText() const;
	//获取自定义文本
	QString getCustomMsgText() const;

	//json转换
	QString toJson() const;
	void fromJson(const QString &json);
	

private:
	void fromCRIMMsgInfo(const CRIMMessage &info);
	CRIMMessage toCRIMMsgInfo() const;
	//用于绘制的文本（可以是特殊格式）
	QString displayContent;
	//扩展参数
	QVariantMap exParams;
};

bool isSupportMsgType(CRIMMsgType t);
QString getPicSavePath();
QString makeANewPicName();
QString save(const QImage& image);
QString getFinalPicPath(const QString& picName);
QString parseFullHtml(const QString& html, QStringList &picUrls);
QString getInviteOpTypeString(const CRIMMessage &msg);
};

Q_DECLARE_METATYPE(ChatPublic::UIMsgInfo);
#endif // CHATPUBLIC_H
