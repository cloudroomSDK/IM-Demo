#include "stdafx.h"
#include "ChatPublic.h"
#include "CRIM_DataConv.h"
#include "FaceHelper.h"

namespace ChatPublic{
		QSet<CRIMMsgType> g_supportMsgType {
		CRIM_MsgType_Text,							/// 文本消息
		CRIM_MsgType_Image,							/// 图片消息
//		CRIM_MsgType_Video,							/// 视频消息
		CRIM_MsgType_File,							/// 文件消息
		CRIM_MsgType_At,							/// @消息
		CRIM_MsgType_Revoke,						/// 撤回消息回执
		CRIM_MsgType_Custom,						/// 自定义消息
		//通知类消息类型
		CRIM_MsgType_FriendAppApproved,              /// 同意加好友申请通知
		CRIM_MsgType_FriendAppRejected,              /// 拒绝加好友申请通知
		CRIM_MsgType_FriendReq,                      /// 加好友通知
		CRIM_MsgType_FriendAdded,                    /// 添加好友通知
		CRIM_MsgType_FriendDeleted,                  /// 删除好友通知
		CRIM_MsgType_FriendRemarkSet,                /// 设置好友备注通知
		CRIM_MsgType_BlackAdded,                     /// 加黑名单通知
		CRIM_MsgType_BlackDeleted,                   /// 移除黑名单通知
		CRIM_MsgType_ConversationOptChange,          /// 会话免打扰设置通知
		CRIM_MsgType_UserInfoUpdated,                /// 个人信息变更通知
		CRIM_MsgType_OANotification,                 /// oa 通知
		CRIM_MsgType_GroupCreated,                   /// 群创建通知
		CRIM_MsgType_GroupInfoSet,                   /// 更新群信息通知
		CRIM_MsgType_JoinGroupReq,                   /// 申请加群通知
		CRIM_MsgType_MemberQuit,                     /// 群成员退出通知
		CRIM_MsgType_GroupAppAccepted,               /// 同意加群申请通知
		CRIM_MsgType_GroupAppRejected,               /// 拒绝加群申请通知
		CRIM_MsgType_GroupOwnerTransferred,          /// 群主更换通知
		CRIM_MsgType_MemberKicked,                   /// 群成员被踢通知
		CRIM_MsgType_MemberInvited,                  /// 邀请群成员通知
		CRIM_MsgType_MemberEnter,                    /// 群成员进群通知
		CRIM_MsgType_DismissGroup,                   /// 解散群通知
		CRIM_MsgType_GroupMemberMutedNotification,   /// 群成员禁言通知
		CRIM_MsgType_GroupMemberCancelMutedNotification,  /// 取消群成员禁言通知
		CRIM_MsgType_GroupMutedNotification,              /// 群禁言通知
		CRIM_MsgType_GroupCancelMutedNotification,        /// 取消群禁言通知
		CRIM_MsgType_GroupMemberInfoSetNotification,      /// 设置群成员信息通知
		CRIM_MsgType_GroupMemberSetToAdminNotification,   /// 设置管理员通知
		CRIM_MsgType_GroupMemberSetToOrdinaryUserNotification, /// 设置成普通成员通知
		CRIM_MsgType_GroupAnnouncement,                        /// 群公告
		CRIM_MsgType_GroupSetNameNotification,                 /// 修改群名称
		CRIM_MsgType_SuperGroupUpdateNotification,             /// 群更新通知
		CRIM_MsgType_BurnAfterReadingNotification,             /// 阅后即焚通知
		CRIM_MsgType_OrganizationChangedNotification,          /// 组织变化通知
		CRIM_MsgType_Business,                       /// 业务通知
		CRIM_MsgType_Revoke,                         /// 撤回消息
		CRIM_MsgType_1V1HasReadReceipt,              /// 单聊已读回执
		CRIM_MsgType_GroupHasReadReceipt,            /// 群已读回执
	};
	bool isSupportMsgType(CRIMMsgType t)
	{
		return g_supportMsgType.contains(t);
	}

	//下载文件的本地路径缓存，每次启动时为空，管理运行时的下载路径
	QMap<QString, QString > g_loclFilePathForDownload;

	//本地资源路径处理
	void disposeFileRes(UIMsgInfo &msg)
	{
		if (msg.contentType != CRIM_MsgType_Image && msg.contentType != CRIM_MsgType_File
			&& msg.contentType != CRIM_MsgType_Video && msg.contentType != CRIM_MsgType_Audio)
		{
			return;
		}

		//本地文件路径处理
		QString fileDir = msg.getFileStorageDir();
		QString locFilePath = msg.getLocFilePath();
		if (!msg.isMyMsg(false))
		{
			if (msg.contentType == CRIM_MsgType_Image)
			{
				QFileInfo picInfo(!msg.pictureElem.sourcePath.isEmpty() ? msg.pictureElem.sourcePath : msg.pictureElem.sourcePicture.uuid);
				msg.pictureElem.sourcePath = fileDir + msg.clientMsgID + "." + picInfo.suffix();
				locFilePath = msg.pictureElem.sourcePath;
			}
			else if (msg.contentType == CRIM_MsgType_File)
			{
// 				QFileInfo fi(msg.fileElem.fileName);
// 				QString fileName = fi.baseName() + "." + msg.clientMsgID + "." + fi.suffix();
// 				msg.fileElem.filePath = fileDir + fileName;
//				locFilePath = msg.fileElem.filePath;
			}
			else if (msg.contentType == CRIM_MsgType_Video)
			{
				QFileInfo videoFileInfo(msg.videoElem.videoUUID);
				QFileInfo snapshotFileInfo(msg.videoElem.snapshotUUID);
				msg.videoElem.videoPath = fileDir + msg.clientMsgID + "." + videoFileInfo.suffix();
				msg.videoElem.snapshotPath = fileDir + msg.clientMsgID + "." + snapshotFileInfo.suffix();
				locFilePath = msg.videoElem.videoPath;
			}
			else if (msg.contentType == CRIM_MsgType_Audio)
			{
				QFileInfo fi(msg.soundElem.uuid);
				msg.soundElem.soundPath = fileDir + msg.clientMsgID + "." + fi.suffix();
				locFilePath = msg.soundElem.soundPath;
			}
		}

		//文件状态处理
		msg.fileState = QFileInfo::exists(locFilePath) ? UFS_NORMAL : UFS_NORECV;
	}

	//转义字符处理
	QString disposeEscapeChar(const QString &text)
	{
		QString strHtml(text);
		//处理文本
		{
			QRegExp UrlRx("((www\\.|(http|https|ftp)://)([.@]?[a-z0-9/_:@=+?,&;#%~-]{1,}){1,})");
			UrlRx.setCaseSensitivity(Qt::CaseInsensitive);
			int pos = 0;
			int lastPos = 0;
			QString strTempFragment;
			if (strHtml.contains(UrlRx))
			{
				while ((pos = UrlRx.indexIn(strHtml, pos)) != -1)
				{
					QString strTemp = strHtml.mid(lastPos, pos - lastPos);
					strTemp.replace("&", "&amp;");
					strTemp.replace("\t", "&nbsp;&nbsp;");
					strTemp.replace(" ", "&nbsp;");
					strTemp.replace("<", "&lt;");
					strTemp.replace(">", "&gt;");
					strTemp.replace("'", "&apos;");
					strTemp.replace("\"", "&quot;");
					strTempFragment += strTemp;
					strTempFragment += QString("<a href=\"%1\" target=\"_blank\"><span style = \"color:#2384C8;\">%2</span></a>").arg(UrlRx.cap(1)).arg(UrlRx.cap(1));
					pos += UrlRx.matchedLength();
					lastPos = pos;
				}
				QString strEndFragment = strHtml.mid(lastPos);
				strEndFragment.replace("&", "&amp;");
				strEndFragment.replace("\t", "&nbsp;&nbsp;");
				strEndFragment.replace(" ", "&nbsp;");
				strEndFragment.replace("<", "&lt;");
				strEndFragment.replace(">", "&gt;");
				strEndFragment.replace("'", "&apos;");
				strEndFragment.replace("\"", "&quot;");
				strTempFragment += strEndFragment;
				strHtml = strTempFragment;
			}
			strHtml.replace(QChar::LineFeed, "<br/>");
			strHtml.replace(QChar::LineSeparator, "<br/>");
		}
		return strHtml;
	}

	//解析at消息
	QString parseAtText(const CRIMMessage &msg, bool forPreview = false)
	{
		QString text = msg.atTextElem.text;
		for (auto &info : msg.atTextElem.atUsersInfo)
		{
			const QString &userId = info.atUserID;
			const QString &nickName = info.groupNickname;

			//id替换为昵称
			QString usrIdStr = g_atTextFormat.arg(userId);
			QString usrNameStr = g_atTextFormat.arg(nickName);
			//@我或@所有人
			if (msg.atTextElem.isAtAll() || msg.atTextElem.isAtSelf())
			{
				//非预览加高亮
				if (!forPreview)
				{
					usrNameStr = QString("<span style = \"color:#2384C8;\">%1</span>").arg(usrNameStr);
				}
			}
			text.replace(usrIdStr, usrNameStr);
 		}
		return text;
	}

	//解析通知消息
	QString parseNoticeText(const CRIMMessage &msg)
	{
		QString strContent;
		CRIMMsgType type = msg.contentType;
		QString opName;
		if (!msg.notificationElem.opUser.isNull())
		{
			opName = msg.notificationElem.opUser->nickname;
		}
		if (type == CRIM_MsgType_GroupOwnerTransferred)
		{
			QString strDstName;
			if (!msg.notificationElem.groupNewOwner.isNull())
			{
				strDstName = msg.notificationElem.groupNewOwner->nickname;
			}
			strContent = QObject::tr("%1将群主身份转让给%2").arg(opName).arg(strDstName);
		}
		else if (type == CRIM_MsgType_DismissGroup)
		{
			strContent = QObject::tr("群已解散");
		}
		else if (type == CRIM_MsgType_MemberInvited)
		{
			if (!msg.notificationElem.invitedUserList.isEmpty())
			{
				QStringList invitedUserNameList;
				for (auto &invitedUser : msg.notificationElem.invitedUserList)
				{
					invitedUserNameList.append(invitedUser->nickname);
				}
				strContent = QObject::tr("%1邀请了%2").arg(opName).arg(invitedUserNameList.join(QObject::tr("、")));
			}
			else if (!msg.notificationElem.entrantUser.isNull())
			{
				strContent = msg.notificationElem.entrantUser->nickname;
			}

		}
		else if (type == CRIM_MsgType_MemberKicked)
		{
			if (!msg.notificationElem.kickedUserList.isEmpty())
			{
				QStringList kickedUserList;
				for (auto &kickedUser : msg.notificationElem.kickedUserList)
				{
					kickedUserList.append(kickedUser->nickname);
				}
				strContent = QObject::tr("%1将%2移出了群聊").arg(opName).arg(kickedUserList.join(QObject::tr("、")));
			}
			else
			{
				qDebug("%s", msg.notificationElem.detail.toUtf8().constData());
			}
		}
		else if (type == CRIM_MsgType_MemberQuit)
		{
            QString strName;
            if (!msg.notificationElem.quitUser.isNull())
            {
                strName = msg.notificationElem.quitUser->nickname;
            }
			strContent = QObject::tr("%1离开了本群").arg(strName);
		}
		else if (type == CRIM_MsgType_FriendDeleted)
		{
			strContent = msg.notificationElem.detail;
		}
		else if (type == CRIM_MsgType_GroupInfoSet)//群信息被修改，不显示到ui上
		{
			strContent.clear();
		}
		else if (type == CRIM_MsgType_FriendAppApproved)
		{
			strContent = QObject::tr("你们已经成为好友，可以开始聊天了");
		}
		else if (type == CRIM_MsgType_GroupCreated)//a 创建了群聊
		{
			strContent = QObject::tr("%1创建了群聊").arg(opName);
		}
		else if (type == CRIM_MsgType_BurnAfterReadingNotification)
		{
			strContent = msg.notificationElem.burnAfterReading->isPrivate?QObject::tr("开启了阅后即焚") : QObject::tr("关闭了阅后即焚");
		}
		else if (type == CRIM_MsgType_GroupSetNameNotification)
		{
			strContent = QObject::tr("%1修改了群名称").arg(opName);
		}
		else if (type == CRIM_MsgType_GroupMutedNotification)
		{
			strContent = QObject::tr("%1开启了群禁言").arg(opName);
		}
		else if (type == CRIM_MsgType_GroupCancelMutedNotification)
		{
			strContent = QObject::tr("%1关闭了群禁言").arg(opName);
		}
		else
		{
			strContent = QObject::tr("[暂不支持的消息类型:%1]").arg(type);
		}
		return strContent;
	}

	void UIMsgInfo::fromCRIMMsgInfo(const CRIMMessage &msg)
	{
		*((CRIMMessage*)this) = msg;

		//不支持的类型不处理
		if (!isSupportMsgType(this->contentType))
			return;

		//处理文件资源（状态、本地文件路径）
		disposeFileRes(*this);
		if (this->contentType == CRIM_MsgType_Text || this->contentType == CRIM_MsgType_At || this->contentType == CRIM_MsgType_Quote)//文本消息，at消息，引用消息
		{
			if (this->contentType == CRIM_MsgType_Text)
			{
				this->displayContent = msg.textElem.content;
			}
			else if (this->contentType == CRIM_MsgType_Quote)
			{
				this->displayContent = msg.quoteElem.text;
			}
			else if (this->contentType == CRIM_MsgType_At)
			{
				this->displayContent = parseAtText(msg);
			}
			
			//转义字符处理
			this->displayContent = disposeEscapeChar(this->displayContent);
			//表情处理
			QList<QUrl> rsltFaceUrls;
			this->displayContent = replaceFace2LocRes(this->displayContent, rsltFaceUrls);
			this->faceUrls = rsltFaceUrls;
		}
		else if (this->contentType == CRIM_MsgType_Image)//图片消息
		{
			QUrl url = QUrl::fromLocalFile(this->getLocFilePath());
			this->displayContent = QString("<img src=\"%1\"/>").arg(url.toString());
		}
		else if (this->contentType == CRIM_MsgType_Video)//视频
		{
		}
		else if (this->contentType == CRIM_MsgType_Custom)
		{
			this->displayContent = getCustomMsgText();
			//未成功解析的消息按照空消息显示（即过滤）
			if (this->displayContent.isEmpty())
			{
				this->contentType = CRIM_MsgType_NULL;
			}
		}
		else if (isSystemMsg())//系统通知消息，与会话的消息预览使用一样的文本
		{
			this->displayContent = getMsgPreviewText();
		}
	}

	CRIMMessage UIMsgInfo::toCRIMMsgInfo() const
	{
		return *((CRIMMessage*)this);
	}

	const QString &UIMsgInfo::getMsgID() const
	{
		return clientMsgID;
	}

	const QString &UIMsgInfo::getSenderID() const 
	{ 
		return sendID; 
	}

	const QString &UIMsgInfo::getSenderName() const 
	{ 
		return senderNickname; 
	}

	void UIMsgInfo::setDisplayContent(const QString &content)
	{
		displayContent = content;
	}
	const QString &UIMsgInfo::getDisplayContent() const
	{
		return displayContent;
	}

	const QString &UIMsgInfo::getOrgContent() const
	{
		static QString rslt;
		switch (contentType)
		{
		case CRIMSDK::CRIM_MsgType_Text: return textElem.content;
		case CRIMSDK::CRIM_MsgType_At: return atTextElem.text;
		case CRIMSDK::CRIM_MsgType_Quote: return quoteElem.text;
		default: return rslt;
		}
	}

	QPixmap UIMsgInfo::getSenderFace(const QSize &sz)
	{
		//这里url、name信息可能是旧的，所以不用传
		return ImIconMgr::instance()->getUserIcon(sendID, sz);
	}

	bool UIMsgInfo::isMyMsg(bool includeOtherPlatform) const
	{
		bool bMine = (sendID == CRIMManager_Qt::instance()->getUserID());
		if (!includeOtherPlatform)
		{
			bMine = bMine && (senderPlatformID == CRIMManager_Qt::instance()->getPlatformID());
		}
		return bMine;
	}

	bool UIMsgInfo::isNormalMsg(CRIMMsgType type)
	{
		return type >= CRIM_MsgType_Text  && type <= CRIM_MsgType_Face
			&& type != CRIM_MsgType_Typing;
	}
	bool UIMsgInfo::isSystemMsg(CRIMMsgType type)
	{
		return type == CRIM_MsgType_Revoke || isNotificationMsg(type);
	}
	bool UIMsgInfo::isNotificationMsg(CRIMMsgType type)
	{
		return type >= CRIM_MsgType_FriendAppApproved  && type <= CRIM_MsgType_OrganizationChangedNotification;
	}

	CRIMMsgStatus UIMsgInfo::getMsgState() const
	{ 
		return status;
	}

	bool UIMsgInfo::hasRead() const
	{
		return isRead;
	}

	QDateTime UIMsgInfo::getCreateTime() const
	{ 
		return QDateTime::fromMSecsSinceEpoch(createTime); 
	}

	QDateTime UIMsgInfo::getSendTime() const
	{
		return QDateTime::fromMSecsSinceEpoch(sendTime);
	}

	bool UIMsgInfo::isAtMe() const
	{
		if (contentType != CRIM_MsgType_At)
		{
			return false;
		}
		return atTextElem.isAtSelf();
	}

	QString UIMsgInfo::getFileStorageDir() const
	{
		QString strPath = App::instance()->getAcntFileStorage();
		bool bTempDir = false;
		if (contentType == CRIM_MsgType_File)
		{
			strPath += AddBackslashAtend("File");
		}
		else if (contentType == CRIM_MsgType_Image)
		{
			strPath += AddBackslashAtend("Image");
		}
		else if (contentType == CRIM_MsgType_Audio)
		{
			strPath += AddBackslashAtend("Audio");
		}
		else if (contentType == CRIM_MsgType_Video)
		{
			strPath += AddBackslashAtend("Video");
		}
		else
		{
			bTempDir = true;
			strPath += AddBackslashAtend("Temp");
		}
		if (!bTempDir)
		{
			QString sendTimeStr = QDateTime::fromMSecsSinceEpoch(sendTime).toString("yyyy-MM");
			strPath += AddBackslashAtend(sendTimeStr);
		}
		return strPath;
	}

	const QString &UIMsgInfo::getDownloadUrl() const
	{
		switch (contentType)
		{
		case CRIM_MsgType_Image:	return pictureElem.sourcePicture.url;
		case CRIM_MsgType_File:		return fileElem.sourceUrl;
		case CRIM_MsgType_Video:	return videoElem.videoUrl;
		case CRIM_MsgType_Audio:	return soundElem.sourceUrl;
		default:
		{
			static QString nul;
			return nul;
		}
		}
	}

	void UIMsgInfo::setLocFilePath(const QString &fileName, const QString &conversationID)
	{
		if (contentType == CRIM_MsgType_File)
		{
			//设置到db
			localEx["locFilePath"] = fileName;
			CRIMManager_Qt::instance()->setMsgLocalEx(conversationID, clientMsgID, localEx);
			//设置到内存
			g_loclFilePathForDownload[clientMsgID] = fileName;
		}
	}

	QString UIMsgInfo::getLocFilePath() const
	{
		static QString nul;
		switch (contentType)
		{
		case CRIM_MsgType_Image:	return pictureElem.sourcePath;
		case CRIM_MsgType_File:
		{
			//自己发送的文件，直接用本地路径，如果不存在则会下载到存储路径，后续使用新路径
			if (isMyMsg(false) && QFileInfo::exists(fileElem.filePath))
			{
				return fileElem.filePath;
			}
			//从db中获取
			if (localEx["locFilePath"].isValid())
			{
				return localEx["locFilePath"].toString();
			}
			//从内存中获取
			if (g_loclFilePathForDownload.contains(clientMsgID))
			{
				return g_loclFilePathForDownload.value(clientMsgID);
			}
			//如果没有获取到，创建一个唯一的本地路径
			QFileInfo fi(fileElem.fileName);
			QString fileName = fi.baseName() + "." + clientMsgID + "." + fi.suffix();
			fileName = getFileStorageDir() + fileName;
			return fileName;
		}
		case CRIM_MsgType_Video:	return videoElem.videoPath;
		case CRIM_MsgType_Audio:	return soundElem.soundPath;
		default: return nul;
		}
	}

	QString UIMsgInfo::getSnapshotFilePath() const
	{
		if (contentType == CRIM_MsgType_Image)
		{
			QFileInfo fi(pictureElem.sourcePath);
			QString filePathName = fi.absolutePath() + "/" + fi.baseName() + ".snapshot." + fi.suffix();
			return filePathName;
		}
		else if (contentType == CRIM_MsgType_Video)
		{
			return videoElem.snapshotPath;
		}
		return "";
	}

	QString UIMsgInfo::getFileName() const
	{
		static QString nul;
		switch (contentType)
		{
		case CRIM_MsgType_Image:
		{
			QFileInfo fileInfo(!pictureElem.sourcePath.isEmpty() ? pictureElem.sourcePath : pictureElem.sourcePicture.uuid);
			return fileInfo.fileName();
		}
		case CRIM_MsgType_File:
		{
			return fileElem.fileName;
		}
		case CRIM_MsgType_Video:
		{
			QFileInfo fileInfo(!videoElem.videoPath.isEmpty() ? videoElem.videoPath : videoElem.videoUUID);
			return fileInfo.fileName();
		}
		case CRIM_MsgType_Audio:
		{
			QFileInfo fileInfo(!soundElem.soundPath.isEmpty() ? soundElem.soundPath : soundElem.uuid);
			return fileInfo.fileName();
		}
		default: return nul;
		}
	}

	qint64 UIMsgInfo::getFileSize() const 
	{
		if (contentType == CRIM_MsgType_Image)
		{
			return pictureElem.sourcePicture.size;
		}
		else if (contentType == CRIM_MsgType_File)
		{
			return fileElem.fileSize;
		}
		else if (contentType == CRIM_MsgType_Video)
		{
			return videoElem.videoSize;
		}
		else if (contentType == CRIM_MsgType_Audio)
		{
			return soundElem.dataSize;
		}
		return 0;
	}

	QString UIMsgInfo::getCustomMsgText() const
	{
		QString rsltStr;
		//呼叫消息，不处理邀请、邀请接受类型
		if (customElem.isInviteType())
		{
			if (!customElem.inviteInfo)
				return rsltStr;

			switch (customElem.customType)
			{
			case CRCUSTOMMSG_NewInvitation://invite
				if (customElem.inviteInfo->groupID.length()>0)
				{
					rsltStr = QObject::tr("发起了群通话");
				}
				break;
			case CRCUSTOMMSG_InviteeAccept://accept
				//emit CRIMManager_Qt::instance()->s_inviteeAccepted(signalingInfo);
				break;
			case CRCUSTOMMSG_InviteeReject://reject
			case CRCUSTOMMSG_InvitationCancel://cancel
				{
					rsltStr = getInviteOpTypeString(*this);
				}
				break;
			case CRCUSTOMMSG_InvitationHangUp://hangup
				if (customElem.inviteInfo->duration >= 0 && customElem.inviteInfo->groupID.length()<=0 )
				{
					rsltStr = QObject::tr("通话时长") + CoverDuration2Str(customElem.inviteInfo->duration);
				}
				else
				{
					rsltStr = QObject::tr("通话结束");
				}
				break;
			default:
				break;
			}
		}
		return rsltStr;
	}
	QString UIMsgInfo::getMsgPreviewText() const
	{
		QString text = QObject::tr("[暂不支持的消息类型:%1]").arg(this->contentType);
		CRIMMsgType tmpContentType = this->contentType;
		if (tmpContentType == CRIM_MsgType_NULL)//空消息
		{
			text.clear();
			return text;
		}
		bool isSgnalingType = this->customElem.isInviteType();
		//显示时可能转换成空类型，这里需要使用自定义消息类型处理
		if (isSgnalingType && tmpContentType == CRIMSDK::CRIM_MsgType_NULL)
		{
			tmpContentType = CRIMSDK::CRIM_MsgType_Custom;
		}

		//不支持的类型
		if (!isSupportMsgType(tmpContentType))
		{
			return text;
		}

		//通知类型
		if (tmpContentType >= CRIM_MsgType_FriendAppApproved)
		{
			text = parseNoticeText(*this);
			return text;
		}

		switch (tmpContentType)
		{
		case CRIM_MsgType_NULL:
			break;
		case CRIM_MsgType_Text:
		case CRIM_MsgType_Quote:
		case CRIM_MsgType_At:
			{
				if (tmpContentType == CRIM_MsgType_Text)
				{
					text = textElem.content;
				}
				else if (tmpContentType == CRIM_MsgType_Quote)
				{
					text = quoteElem.text;
				}
				else if (tmpContentType == CRIM_MsgType_At)
				{
					text = parseAtText(*this, true);
				}
				QList<QUrl> faceUrls;
				text = replaceFace2LocRes(text, faceUrls);
				for (auto &url : faceUrls)
				{
					text.replace(QString("<img src = \"%1\"/>").arg(url.toString()), QObject::tr("[表情]"));
				}
			}
			break;
		case CRIM_MsgType_Image:
			text = QObject::tr("[图片]");
			break;
		case CRIM_MsgType_Audio:
			text = QObject::tr("[语音]");
			break;
		case CRIM_MsgType_Video:
			text = QObject::tr("[视频]");
			break;
		case CRIM_MsgType_File:
			text = QObject::tr("[文件]");
			break;
		case CRIM_MsgType_Card:
			text = QObject::tr("[名片]");
			break;
		case CRIM_MsgType_Location:
			text = QObject::tr("[位置]");
			break;
		case CRIM_MsgType_Revoke:
			{
				text = QObject::tr("%1撤回了一条消息").arg(CRIMManager_Qt::instance()->getUserID() == this->sendID ? QObject::tr("你") : this->senderNickname);
			}
			break;
		case CRIM_MsgType_Merge:
			text = QObject::tr("[合并消息]");
			break;
		case CRIM_MsgType_Custom:
			{
				if (isSgnalingType)
				{
					text = QObject::tr("[语音通话]");
				}
			}
			break;
		default: break;
		}

		return text;
	}

	void UIMsgInfo::updateMsgType(CRIMMsgType type)
	{
		contentType = type;
		//按照新类型重新触发一次转换
		fromCRIMMsgInfo(toCRIMMsgInfo());
	}

	QString UIMsgInfo::toJson() const
	{
		QVariantMap varMap;
		CRIM::Struct_Conv(*this, varMap);
		varMap["defHeightForNull"] = defHeightForNull;
		varMap["fileState"] = fileState;
		varMap["fileProgress"] = fileProgress;
		varMap["highlighterText"] = highlighterText;
		varMap["displayContent"] = displayContent;
		varMap["exParams"] = exParams;
		return QVariantToJosnQStr(varMap);

	}
	void UIMsgInfo::fromJson(const QString &json)
	{
		QVariantMap varMap = JsonQStrToQVaraint(json).toMap();
		CRIM::Struct_Conv(varMap, *this);

		defHeightForNull = varMap["defHeightForNull"].toInt();
		fileState = (UIFileState)varMap["fileState"].toInt();
		fileProgress = varMap["fileProgress"].toFloat();
		highlighterText = varMap["highlighterText"].toString();
		displayContent = varMap["displayContent"].toString();
		exParams = varMap["exParams"].toMap();
	}

	QString getPicSavePath()
	{
		QString path = App::instance()->getAcntDatPath();
		QDir dirTmp(path);
		if (!dirTmp.exists())
		{
			dirTmp.mkpath(path);
		}

		return path;
	}

	QString makeANewPicName()
	{
		//生成文件存储的路径
		QString strName = QString("%1%2.jpg").arg(getPicSavePath()).arg(QUuid::createUuid().toString());
		return strName;
	}


	QString save(const QImage& image)
	{
		if (image.isNull())
			return "";

		QString strName = makeANewPicName();
		if (!(image.save(strName, NULL, 95)))
		{
			return "";
		}

		return strName;
	}

	QString getFinalPicPath(const QString& picName)
	{
		QString strFile(picName);
		QString newFileName;

		QUrl u(picName);
		if (u.isLocalFile())
		{
			strFile = u.toLocalFile();
		}

		if (strFile.startsWith("http://"))
		{
			newFileName = QString(":/Res/IM/DefaultIcon.svg");
		}
		else if (strFile.startsWith(":/Res/") || strFile.startsWith(getPicSavePath()))
		{
			newFileName = strFile;
		}
		else if (strFile.contains(".gif"))
		{
			newFileName = strFile;
		}
		else
		{
			QImage img(strFile);
			newFileName = save(img);
			if (newFileName.isEmpty())
			{
				newFileName = strFile;
			}
		}

		u = QUrl::fromLocalFile(newFileName);
		newFileName = u.toString();
		return newFileName;
	}

	QString parseFullHtml(const QString& html, QStringList &picUrls)
	{
		QTextDocument d;
		d.setHtml(html);

		QString strHtml("");
		QTextBlock currentBlock = d.begin();
		while (currentBlock.isValid())
		{
			for (QTextBlock::iterator it = currentBlock.begin(); !(it.atEnd()); ++it)
			{
				QTextFragment currentFragment = it.fragment();
				QTextCharFormat  currentFormat = currentFragment.charFormat();
				if (currentFormat.isImageFormat())
				{
					QTextImageFormat tf = currentFormat.toImageFormat();
					if (tf.isValid())
					{
						QString urlName = getFinalPicPath(tf.name());
						picUrls.append(urlName);
						strHtml += QString("<img src=\"%1\"/>").arg(urlName);
						continue;
					}
				}
				else
				{
					QString fragText = currentFragment.text();
					fragText.replace(0xA0, " ");
					fragText.replace(0x2028, "<br>");
					strHtml += fragText;
				}
			}
			//strHtml += "<br/>";
			currentBlock = currentBlock.next();
		}
		return strHtml;
	}

	QString getInviteOpTypeString(const CRIMMessage &msg)
	{
		static QString reasonForCaller[CRIVT_OP_BUTT] = {
			QObject::tr("呼叫已取消"),
			QObject::tr("呼叫未应答"),
			QObject::tr("呼叫未接听(忙)"),
			QObject::tr("呼叫被拒绝"),
		};
		static QString reasonForCalled[CRIVT_OP_BUTT] = {
			QObject::tr("呼入已取消"),
			QObject::tr("呼入未应答"),
			QObject::tr("呼入未接听(忙)"),
			QObject::tr("呼入已拒绝"),
		};

		int rejReason = CRIVT_OP_Undef;
		bool bIAmCaller = false;
		CRIMInvitationInfo *p = msg.customElem.inviteInfo.data();
		if (p != nullptr)
		{
			rejReason = p->opType;
			if (rejReason < 0 || rejReason >= CRIVT_OP_BUTT)
			{
				rejReason = 0;
			}
			bIAmCaller = (p->inviterUserID == CRIMManager_Qt::instance()->getUserID());
		}

		return (bIAmCaller) ? reasonForCaller[rejReason] : reasonForCalled[rejReason];
	}

}
