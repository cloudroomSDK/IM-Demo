#include "stdafx.h"
#include "CRIM_DataConv.h"

void CRIM::Struct_Conv(const CRIMOfflinePushInfo &src, QVariantMap &dst)
{
	dst["title"] = src.title;
	dst["desc"] = src.desc;
	dst["ex"] = src.ex;
	dst["iOSPushSound"] = src.iOSPushSound;
	dst["iOSBadgeCount"] = src.iOSBadgeCount;
}

void CRIM::Struct_Conv(const QVariantMap &src, CRIMOfflinePushInfo &dst)
{
	dst.title = src["title"].toString();
	dst.desc = src["desc"].toString();
	dst.ex = src["ex"].toString();
	dst.iOSPushSound = src["iOSPushSound"].toString();
	dst.iOSBadgeCount = src["iOSBadgeCount"].toBool();
}

void CRIM::Struct_Conv(const CRIMPictureInfo &src, QVariantMap &dst)
{
	dst["uuid"] = src.uuid;
	dst["type"] = src.type;
	dst["size"] = src.size;
	dst["width"] = src.width;
	dst["height"] = src.height;
	dst["url"] = src.url;
}

void CRIM::Struct_Conv(const QVariantMap &src, CRIMPictureInfo &dst)
{
	dst.uuid = src["uuid"].toString();
	dst.type = src["type"].toString();
	dst.size = src["size"].toLongLong();
	dst.width = src["width"].toInt();
	dst.height = src["height"].toInt();
	dst.url = src["url"].toString();
}

void CRIM::Struct_Conv(const CRIMPictureElem &src, QVariantMap &dst)
{
	dst["sourcePath"] = src.sourcePath;

	QVariantMap varSourcePicture;
	Struct_Conv(src.sourcePicture, varSourcePicture);
	dst["sourcePicture"] = varSourcePicture;

	QVariantMap varBigPicture;
	Struct_Conv(src.bigPicture, varBigPicture);
	dst["bigPicture"] = varBigPicture;

	QVariantMap varSnapshotPicture;
	Struct_Conv(src.snapshotPicture, varSnapshotPicture);
	dst["snapshotPicture"] = varSnapshotPicture;
}

void CRIM::Struct_Conv(const QVariantMap &src, CRIMPictureElem &dst)
{
	dst.sourcePath = src["sourcePath"].toString();
	Struct_Conv(src["sourcePicture"].toMap(), dst.sourcePicture);
	Struct_Conv(src["bigPicture"].toMap(), dst.bigPicture);
	Struct_Conv(src["snapshotPicture"].toMap(), dst.snapshotPicture);
}


void CRIM::Struct_Conv(const CRIMSoundElem &src, QVariantMap &dst) {
	dst["uuid"] = src.uuid;
    dst["soundPath"] = src.soundPath;
    dst["sourceUrl"] = src.sourceUrl;
    dst["dataSize"] = src.dataSize;
    dst["duration"] = src.duration;
}

void CRIM::Struct_Conv(const QVariantMap &src, CRIMSoundElem &dst) {
	dst.uuid = src["uuid"].toString();
    dst.soundPath = src["soundPath"].toString();
    dst.sourceUrl = src["sourceUrl"].toString();
    dst.dataSize = src["dataSize"].toLongLong();
    dst.duration = src["duration"].toLongLong();
}

void CRIM::Struct_Conv(const CRIMVideoElem &src, QVariantMap &dst) {
    dst["videoPath"] = src.videoPath;
    dst["videoUUID"] = src.videoUUID;
    dst["videoUrl"] = src.videoUrl;
    dst["videoType"] = src.videoType;
    dst["videoSize"] = src.videoSize;
    dst["duration"] = src.duration;
    dst["snapshotPath"] = src.snapshotPath;
    dst["snapshotUUID"] = src.snapshotUUID;
    dst["snapshotSize"] = src.snapshotSize;
    dst["snapshotUrl"] = src.snapshotUrl;
    dst["snapshotWidth"] = src.snapshotWidth;
    dst["snapshotHeight"] = src.snapshotHeight;
}

void CRIM::Struct_Conv(const QVariantMap &src, CRIMVideoElem &dst) {
    dst.videoPath = src["videoPath"].toString();
    dst.videoUUID = src["videoUUID"].toString();
    dst.videoUrl = src["videoUrl"].toString();
    dst.videoType = src["videoType"].toString();
    dst.videoSize = src["videoSize"].toLongLong();
    dst.duration = src["duration"].toLongLong();
    dst.snapshotPath = src["snapshotPath"].toString();
    dst.snapshotUUID = src["snapshotUUID"].toString();
    dst.snapshotSize = src["snapshotSize"].toLongLong();
    dst.snapshotUrl = src["snapshotUrl"].toString();
    dst.snapshotWidth = src["snapshotWidth"].toInt();
    dst.snapshotHeight = src["snapshotHeight"].toInt();
}

void CRIM::Struct_Conv(const CRIMFileElem &src, QVariantMap &dst) {
    dst["filePath"] = src.filePath;
	dst["uuid"] = src.uuid;
    dst["sourceUrl"] = src.sourceUrl;
    dst["fileName"] = src.fileName;
    dst["fileSize"] = src.fileSize;
}

void CRIM::Struct_Conv(const QVariantMap &src, CRIMFileElem &dst) {
    dst.filePath = src["filePath"].toString();
	dst.uuid = src["uuid"].toString();
    dst.sourceUrl = src["sourceUrl"].toString();
	dst.fileName = src["fileName"].toString();
    dst.fileSize = src["fileSize"].toLongLong();
}

void CRIM::Struct_Conv(const CRIMAtInfo &src, QVariantMap &dst) {
    dst["atUserID"] = src.atUserID;
    dst["groupNickname"] = src.groupNickname;
}

void CRIM::Struct_Conv(const QVariantMap &src, CRIMAtInfo &dst) {
    dst.atUserID = src["atUserID"].toString();
    dst.groupNickname = src["groupNickname"].toString();
}

void CRIM::Struct_Conv(const CRIMAtElem &src, QVariantMap &dst) {
    dst["text"] = src.text;
    dst["atUserList"] = src.atUserList;
    
    QVariantList infoList;
	infoList.reserve(src.atUsersInfo.size());
    for (auto &info: src.atUsersInfo) {
        QVariantMap varInfo;
        Struct_Conv(info, varInfo);
        infoList.append(varInfo);
    }
    dst["atUsersInfo"] = infoList;
	if (src.quoteMessage != NULL)
    {
		QVariantMap varQuoteMessage;
		Struct_Conv(*src.quoteMessage, varQuoteMessage);
		dst["quoteMessage"] = varQuoteMessage;
    }
}

void CRIM::Struct_Conv(const QVariantMap &src, CRIMAtElem &dst) {
    dst.text = src["text"].toString();
    dst.atUserList = src["atUserList"].toStringList();
    
	QList<QVariant> varAtUsersInfo = src["atUsersInfo"].toList();
	dst.atUsersInfo.reserve(varAtUsersInfo.size());
	for (auto &info : varAtUsersInfo) {
        CRIMAtInfo varInfo;
        Struct_Conv(info.toMap(), varInfo);
        dst.atUsersInfo.append(varInfo);
    }
	if (src["quoteMessage"].isValid())
	{
		dst.quoteMessage = CRIMMessagePtr(new CRIMMessage());
		Struct_Conv(src["quoteMessage"].toMap(), *dst.quoteMessage);
    }
}

void CRIM::Struct_Conv(const CRIMFaceElem &src, QVariantMap &dst) {
    dst["index"] = src.index;
    dst["data"] = src.data;
}

void CRIM::Struct_Conv(const QVariantMap &src, CRIMFaceElem &dst) {
    dst.index = src["index"].toInt();
    dst.data = src["data"].toString();
}

void CRIM::Struct_Conv(const CRIMLocationElem &src, QVariantMap &dst) {
    dst["description"] = src.description;
    dst["longitude"] = src.longitude;
    dst["latitude"] = src.latitude;
}

void CRIM::Struct_Conv(const QVariantMap &src, CRIMLocationElem &dst) {
    dst.description = src["description"].toString();
    dst.longitude = src["longitude"].toDouble();
    dst.latitude = src["latitude"].toDouble();
}

void CRIM::Struct_Conv(const CRIMCustomElem &src, QVariantMap &dst) {
    dst["data"] = src.data;
    dst["description"] = src.description;
	dst["extension"] = VarMapToJson(src.extension);
}

void CRIM::Struct_Conv(const QVariantMap &src, CRIMCustomElem &dst) {
    dst.data = src["data"].toString();
    dst.description = src["description"].toString();
	dst.extension = JsonToVarMap(src["extension"].toString());
	if (!dst.data.isEmpty())
	{
		QVariantMap datMap = QJsonDocument::fromJson(dst.data.toUtf8()).toVariant().toMap();
		dst.customType = CRIM_CustomMsgType(datMap["customType"].toInt());
		if (dst.isInviteType())
		{
			dst.inviteInfo = CRIMInvitationInfoPtr::create();
			CRIM::Struct_Conv(datMap["data"].toMap(), *dst.inviteInfo);
		}
	}
}

void CRIM::Struct_Conv(const CRIMMergeElem &src, QVariantMap &dst) {
    dst["title"] = src.title;
    dst["abstractList"] = src.abstractList;
    
    QVariantList msgList;
	msgList.reserve(src.multiMessage.size());
    for (auto &message: src.multiMessage) {
		if (message != NULL)
		{
			QVariantMap varMessage;
			Struct_Conv(*message, varMessage);
			msgList.append(varMessage);
		}
    }
    dst["multiMessage"] = msgList;
}

void CRIM::Struct_Conv(const QVariantMap &src, CRIMMergeElem &dst) {
    dst.title = src["title"].toString();
    dst.abstractList = src["abstractLis"].toStringList();
    
	QList<QVariant> varmultiMessage = src["multiMessage"].toList();
	dst.multiMessage.reserve(varmultiMessage.size());
	for (auto &message : varmultiMessage)
	{
		CRIMMessagePtr msgPtr = CRIMMessagePtr(new CRIMMessage());
		Struct_Conv(message.toMap(), *msgPtr);
		dst.multiMessage.append(msgPtr);
    }
}

void CRIM::Struct_Conv(const CRIMQuoteElem &src, QVariantMap &dst) {
    dst["text"] = src.text;
	if (src.quoteMessage != NULL)
	{
		QVariantMap varMessage;
		Struct_Conv(*src.quoteMessage, varMessage);
		dst["quoteMessage"] = varMessage;
	}
}

void CRIM::Struct_Conv(const QVariantMap &src, CRIMQuoteElem &dst) {
    dst.text = src["text"].toString();
	if (src["quoteMessage"].isValid())
	{
		dst.quoteMessage = CRIMMessagePtr(new CRIMMessage());
		Struct_Conv(src["quoteMessage"].toMap(), *dst.quoteMessage);
	}
}

void CRIM::Struct_Conv(const CRIMNotificationElem &src, QVariantMap &dst) {
	dst["detail"] = src.detail;
	dst["defaultTips"] = src.defaultTips;
}

void CRIM::Struct_Conv(const QVariantMap &src, CRIMNotificationElem &dst) {
    dst.detail = src["detail"].toString();
	dst.defaultTips = src["defaultTips"].toString();
	if (dst.detail.length() > 0)
	{
		QVariantMap varMap = JsonToVarMap(dst.detail);
		if (varMap["opUser"].isValid())
		{
			dst.opUser = CRIMGrpMemberInfoPtr(new CRIMGrpMemberInfo());
			Struct_Conv(varMap["opUser"].toMap(), *dst.opUser);
		}
		if (varMap["quitUser"].isValid())
		{
			dst.quitUser = CRIMGrpMemberInfoPtr(new CRIMGrpMemberInfo());
			Struct_Conv(varMap["quitUser"].toMap(), *dst.quitUser);
		}
		if (varMap["entrantUser"].isValid())
		{
			dst.entrantUser = CRIMGrpMemberInfoPtr(new CRIMGrpMemberInfo());
			Struct_Conv(varMap["entrantUser"].toMap(), *dst.entrantUser);
		}
		if (varMap["newGroupOwner"].isValid())
		{
			dst.groupNewOwner = CRIMGrpMemberInfoPtr(new CRIMGrpMemberInfo());
			Struct_Conv(varMap["newGroupOwner"].toMap(), *dst.groupNewOwner);
		}
		if (varMap["group"].isValid())
		{
			dst.group = CRIMGrpInfoPtr(new CRIMGrpInfo());
			Struct_Conv(varMap["group"].toMap(), *dst.group);
		}
		if (varMap["kickedUserList"].isValid())
		{
			QList<QVariant> varkickedUserList = varMap["kickedUserList"].toList();
			dst.kickedUserList.reserve(varkickedUserList.size());
			for (auto &var : varkickedUserList)
			{
				CRIMGrpMemberInfoPtr kickedUser = CRIMGrpMemberInfoPtr(new CRIMGrpMemberInfo());
				Struct_Conv(var.toMap(), *kickedUser);
				dst.kickedUserList.append(kickedUser);
			}
		}
		if (varMap["invitedUserList"].isValid())
		{
			QList<QVariant> varinvitedUserList = varMap["invitedUserList"].toList();
			dst.invitedUserList.reserve(varinvitedUserList.size());
			for (auto &var : varinvitedUserList)
			{
				CRIMGrpMemberInfoPtr invitedUser = CRIMGrpMemberInfoPtr(new CRIMGrpMemberInfo());
				Struct_Conv(var.toMap(), *invitedUser);
				dst.invitedUserList.append(invitedUser);
			}
		}
		if (varMap["isPrivate"].isValid())
		{
			dst.burnAfterReading = CRIMBurnAfterReadingPtr(new CRIMBurnAfterReading());
			dst.burnAfterReading->recvID = varMap["recvID"].toString();
			dst.burnAfterReading->sendID = varMap["sendID"].toString();
			dst.burnAfterReading->conversationID = varMap["conversationID"].toString();
			dst.burnAfterReading->isPrivate = varMap["isPrivate"].toBool();
		}
	}
}

void CRIM::Struct_Conv(const CRIMGroupHasReadInfo &src, QVariantMap &dst) {
    dst["hasReadUserIDList"] = src.hasReadUserIDList;
    dst["hasReadCount"] = src.hasReadCount;
    dst["groupMemberCount"] = src.groupMemberCount;
}

void CRIM::Struct_Conv(const QVariantMap &src, CRIMGroupHasReadInfo &dst) {
    dst.hasReadUserIDList = src["hasReadUserIDList"].toStringList();
    dst.hasReadCount = src["hasReadCount"].toInt();
    dst.groupMemberCount = src["groupMemberCount"].toInt();
}


void CRIM::Struct_Conv(const CRIMAttachedInfoElem &src, QVariantMap &dst) {

	QVariantMap varReadInfo;
	Struct_Conv(src.groupHasReadInfo, varReadInfo);
	dst["groupHasReadInfo"] = varReadInfo;
	dst["isPrivateChat"] = src.isPrivateChat;
	dst["burnDuration"] = src.burnDuration;
	dst["hasReadTime"] = src.hasReadTime;
	dst["notSenderNotificationPush"] = src.notSenderNotificationPush;
	dst["isEncryption"] = src.isEncryption;
	dst["inEncryptStatus"] = src.inEncryptStatus;
}

void CRIM::Struct_Conv(const QVariantMap &src, CRIMAttachedInfoElem &dst) {
	Struct_Conv(src["groupHasReadInfo"].toMap(), dst.groupHasReadInfo);
	dst.isPrivateChat = src["isPrivateChat"].toBool();
	dst.burnDuration = src["burnDuration"].toLongLong();
	dst.hasReadTime = src["hasReadTime"].toLongLong();
	dst.notSenderNotificationPush = src["notSenderNotificationPush"].toBool();
	dst.isEncryption = src["isEncryption"].toBool();
	dst.inEncryptStatus = src["inEncryptStatus"].toBool();
}

void CRIM::Struct_Conv(const CRIMMessage &src, QVariantMap &dst) {
    dst["clientMsgID"] = src.clientMsgID;
    dst["serverMsgID"] = src.serverMsgID;
    dst["createTime"] = src.createTime;
    dst["sendTime"] = src.sendTime;
    dst["sessionType"] = src.sessionType;
    dst["sendID"] = src.sendID;
    dst["recvID"] = src.recvID;
    dst["msgFrom"] = src.msgFrom;
    dst["contentType"] = src.contentType;
    dst["senderPlatformID"] = src.senderPlatformID;
    dst["senderNickname"] = src.senderNickname;
    dst["senderFaceUrl"] = src.senderFaceUrl;
    dst["groupID"] = src.groupID;
    dst["seq"] = src.seq;
    dst["isRead"] = src.isRead;
	dst["status"] = src.status;
    
    QVariantMap varofflinePush;
    Struct_Conv(src.offlinePush, varofflinePush);
    dst["offlinePush"] = varofflinePush;
    
    dst["attachedInfo"] = src.attachedInfo;
    dst["ext"] = src.ext;
	dst["ex"] = VarMapToJson(src.ex);
	dst["localEx"] = VarMapToJson(src.localEx);

    QVariantMap varpictureElem;
    Struct_Conv(src.pictureElem, varpictureElem);
    dst["pictureElem"] = varpictureElem;
    
    QVariantMap varsoundElem;
    Struct_Conv(src.soundElem, varsoundElem);
    dst["soundElem"] = varsoundElem;
    
    QVariantMap varvideoElem;
    Struct_Conv(src.videoElem, varvideoElem);
    dst["videoElem"] = varvideoElem;
    
    QVariantMap varfileElem;
    Struct_Conv(src.fileElem, varfileElem);
    dst["fileElem"] = varfileElem;
    
    QVariantMap varatElem;
	Struct_Conv(src.atTextElem, varatElem);
    dst["atTextElem"] = varatElem;
    
    QVariantMap varlocationElem;
    Struct_Conv(src.locationElem, varlocationElem);
    dst["locationElem"] = varlocationElem;
    
    QVariantMap varcustomElem;
    Struct_Conv(src.customElem, varcustomElem);
    dst["customElem"] = varcustomElem;
    
    QVariantMap varquoteElem;
    Struct_Conv(src.quoteElem, varquoteElem);
    dst["quoteElem"] = varquoteElem;
    
    QVariantMap varmergeElem;
    Struct_Conv(src.mergeElem, varmergeElem);
    dst["mergeElem"] = varmergeElem;
    
    QVariantMap varnotificationElem;
    Struct_Conv(src.notificationElem, varnotificationElem);
    dst["notificationElem"] = varnotificationElem;
    
    QVariantMap varfaceElem;
    Struct_Conv(src.faceElem, varfaceElem);
    dst["faceElem"] = varfaceElem;
    
    QVariantMap varattachedInfoElem;
    Struct_Conv(src.attachedInfoElem, varattachedInfoElem);
	dst["attachedInfoElem"] = varattachedInfoElem;

	QVariantMap vartextElem;
	Struct_Conv(src.textElem, vartextElem);
	dst["textElem"] = vartextElem;

	QVariantMap varcardElem;
	Struct_Conv(src.cardElem, varcardElem);
	dst["cardElem"] = varcardElem;

	QVariantMap vartypingElem;
	Struct_Conv(src.typingElem, vartypingElem);
	dst["typingElem"] = vartypingElem;
}

void CRIM::Struct_Conv(const QVariantMap &src, CRIMMessage &dst) {
    dst.clientMsgID = src["clientMsgID"].toString();
    dst.serverMsgID = src["serverMsgID"].toString();
    dst.createTime = src["createTime"].toLongLong();
    dst.sendTime = src["sendTime"].toLongLong();
	dst.sessionType = (CRIMConversationType)src["sessionType"].toInt();
    dst.sendID = src["sendID"].toString();
    dst.recvID = src["recvID"].toString();
    dst.msgFrom = (CRIMMsgLevel)src["msgFrom"].toInt();
    dst.contentType = (CRIMMsgType)src["contentType"].toInt();
	dst.senderPlatformID = (CRIMPlatform)src["senderPlatformID"].toInt();
    dst.senderNickname = src["senderNickname"].toString();
    dst.senderFaceUrl = src["senderFaceUrl"].toString();
    dst.groupID = src["groupID"].toString();
    dst.seq = src["seq"].toInt();
    dst.isRead = src["isRead"].toBool();
	dst.status = (CRIMMsgStatus)src["status"].toInt();
    
    Struct_Conv(src["offlinePush"].toMap(), dst.offlinePush);
    
    dst.attachedInfo = src["attachedInfo"].toString();
    dst.ext = src["ext"].toString();
	dst.ex = JsonToVarMap(src["ex"].toString());
	dst.localEx = JsonToVarMap(src["localEx"].toString());
    
    Struct_Conv(src["pictureElem"].toMap(), dst.pictureElem);
    Struct_Conv(src["soundElem"].toMap(), dst.soundElem);
    Struct_Conv(src["videoElem"].toMap(), dst.videoElem);
    Struct_Conv(src["fileElem"].toMap(), dst.fileElem);
	Struct_Conv(src["atTextElem"].toMap(), dst.atTextElem);
    Struct_Conv(src["locationElem"].toMap(), dst.locationElem);
    Struct_Conv(src["quoteElem"].toMap(), dst.quoteElem);
    Struct_Conv(src["mergeElem"].toMap(), dst.mergeElem);
    Struct_Conv(src["notificationElem"].toMap(), dst.notificationElem);
    Struct_Conv(src["faceElem"].toMap(), dst.faceElem);
	Struct_Conv(src["attachedInfoElem"].toMap(), dst.attachedInfoElem);
	Struct_Conv(src["textElem"].toMap(), dst.textElem);
	Struct_Conv(src["cardElem"].toMap(), dst.cardElem);
	Struct_Conv(src["typingElem"].toMap(), dst.typingElem);
	Struct_Conv(src["customElem"].toMap(), dst.customElem);
}

void CRIM::Struct_Conv(const CRIMReadReceiptInfo &src, QVariantMap &dst) {
    dst["userID"] = src.userID;
    dst["groupID"] = src.groupID;
    dst["msgIDList"] = src.msgIDList;
    dst["readTime"] = src.readTime;
    dst["msgFrom"] = src.msgFrom;
    dst["contentType"] = src.contentType;
    dst["sessionType"] = src.sessionType;
}

void CRIM::Struct_Conv(const QVariantMap &src, CRIMReadReceiptInfo &dst) {
    dst.userID = src["userID"].toString();
    dst.groupID = src["groupID"].toString();
    dst.msgIDList = src["msgIDList"].toStringList();
    dst.readTime = src["readTime"].toLongLong();
	dst.msgFrom = (CRIMMsgLevel)src["msgFrom"].toInt();
    dst.contentType = (CRIMMsgType)src["contentType"].toInt();
	dst.sessionType = (CRIMConversationType)src["sessionType"].toInt();
}

void CRIM::Struct_Conv(const CRIMPublicUserInfo &src, QVariantMap &dst) {
    dst["userID"] = src.userID;
	dst["nickname"] = src.nickname;
    dst["faceURL"] = src.faceURL;
	dst["ex"] = VarMapToJson(src.ex);
}

void CRIM::Struct_Conv(const QVariantMap &src, CRIMPublicUserInfo &dst) {
    dst.userID = src["userID"].toString();
	dst.nickname = src["nickname"].toString();
    dst.faceURL = src["faceURL"].toString();
	dst.ex = JsonToVarMap(src["ex"].toString());
}

void CRIM::Struct_Conv(const CRIMBlackListInfo &src, QVariantMap &dst) {
	Struct_Conv(*((CRIMPublicUserInfo*)&src), dst);
	dst["createTime"] = src.createTime;
	dst["addSource"] = src.addSource;
	dst["operatorUserID"] = src.operatorUserID;
	dst["attachedInfo"] = src.attachedInfo;
}

void CRIM::Struct_Conv(const QVariantMap &src, CRIMBlackListInfo &dst) {
	Struct_Conv(src, *((CRIMPublicUserInfo*)&dst));
	dst.createTime = src["createTime"].toLongLong();
	dst.addSource = src["addSource"].toInt();
	dst.operatorUserID = src["operatorUserID"].toString();
	dst.attachedInfo = src["attachedInfo"].toString();
}

void CRIM::Struct_Conv(const CRIMFriendInfo &src, QVariantMap &dst) {
	Struct_Conv(*((CRIMPublicUserInfo*)&src), dst);
	dst["remark"] = src.remark;
	dst["createTime"] = src.createTime;
	dst["addSource"] = src.addSource;
	dst["operatorUserID"] = src.operatorUserID;
	dst["attachedInfo"] = src.attachedInfo;
}

void CRIM::Struct_Conv(const QVariantMap &src, CRIMFriendInfo &dst) {
	Struct_Conv(src, *((CRIMPublicUserInfo*)&dst));
	dst.remark = src["remark"].toString();
	dst.createTime = src["createTime"].toLongLong();
	dst.addSource = src["addSource"].toLongLong();
	dst.operatorUserID = src["operatorUserID"].toString();
	dst.attachedInfo = src["attachedInfo"].toString();
}

void CRIM::Struct_Conv(const CRIMUserInfo &src, QVariantMap &dst) {
	Struct_Conv(*((CRIMPublicUserInfo*)&src), dst);
	dst["createTime"] = src.createTime;
	dst["attachedInfo"] = src.attachedInfo;
	dst["globalRecvMsgOpt"] = src.globalRecvMsgOpt;
}

void CRIM::Struct_Conv(const QVariantMap &src, CRIMUserInfo &dst) {
	Struct_Conv(src, *((CRIMPublicUserInfo*)&dst));
	dst.createTime = src["createTime"].toLongLong();
	dst.attachedInfo = src["attachedInfo"].toString();
	dst.globalRecvMsgOpt = (CRIMReceiveMsgOpt)src["globalRecvMsgOpt"].toInt();
}

void CRIM::Struct_Conv(const CRIMUserStatusInfo &src, QVariantMap &dst) {
	dst["userID"] = src.userID;
	QVariantList varPlatformIDs;
	for (auto id : src.platformIDs)
	{
		varPlatformIDs.append(id);
	}
	dst["platformIDs"] = varPlatformIDs;
	dst["status"] = src.status;
}

void CRIM::Struct_Conv(const QVariantMap &src, CRIMUserStatusInfo &dst) {
	dst.userID = src["userID"].toString();
	dst.platformIDs.clear();
	QVariantList varPlatformIDs = src["platformIDs"].toList();
	for (auto &var: varPlatformIDs)
	{
		dst.platformIDs.append((CRIMPlatform)var.toInt());
	}
	dst.status = src["status"].toInt();
}

void CRIM::Struct_Conv(const CRIMFriendReqInfo &src, QVariantMap &dst) {
    dst["fromUserID"] = src.fromUserID;
    dst["fromNickname"] = src.fromNickname;
    dst["fromFaceURL"] = src.fromFaceURL;
    dst["toUserID"] = src.toUserID;
    dst["toNickname"] = src.toNickname;
    dst["toFaceURL"] = src.toFaceURL;
    dst["handleResult"] = src.handleResult;
    dst["reqMsg"] = src.reqMsg;
    dst["createTime"] = src.createTime;
    dst["handlerUserID"] = src.handlerUserID;
    dst["handleMsg"] = src.handleMsg;
    dst["handleTime"] = src.handleTime;
    dst["ex"] = VarMapToJson(src.ex);
}

void CRIM::Struct_Conv(const QVariantMap &src, CRIMFriendReqInfo &dst) {
    dst.fromUserID = src["fromUserID"].toString();
    dst.fromNickname = src["fromNickname"].toString();
    dst.fromFaceURL = src["fromFaceURL"].toString();
    dst.toUserID = src["toUserID"].toString();
    dst.toNickname = src["toNickname"].toString();
    dst.toFaceURL = src["toFaceURL"].toString();
	dst.handleResult = CRIMReqStatus(src["handleResult"].toInt());
    dst.reqMsg = src["reqMsg"].toString();
    dst.createTime = src["createTime"].toLongLong();
    dst.handlerUserID = src["handlerUserID"].toString();
    dst.handleMsg = src["handleMsg"].toString();
    dst.handleTime = src["handleTime"].toLongLong();
    dst.ex = JsonToVarMap(src["ex"].toString());
}

void CRIM::Struct_Conv(const CRIMGrpBaseInfo &src, QVariantMap &dst) {
	dst["groupType"] = src.groupType;
	dst["groupName"] = src.groupName;
    dst["notification"] = src.notification;
    dst["introduction"] = src.introduction;
    dst["faceURL"] = src.faceURL;
    dst["ex"] = VarMapToJson(src.ex);
}

void CRIM::Struct_Conv(const QVariantMap &src, CRIMGrpBaseInfo &dst) {
	dst.groupType = (CRIMGroupType)src["groupType"].toInt();
	dst.groupName = src["groupName"].toString();
    dst.notification = src["notification"].toString();
    dst.introduction = src["introduction"].toString();
    dst.faceURL = src["faceURL"].toString();
    dst.ex = JsonToVarMap(src["ex"].toString());
}

void CRIM::Struct_Conv(const CRIMGrpCreateInfo &src, QVariantMap &dst) {

	QVariantMap vargroupInfo;
	Struct_Conv(src.groupInfo, vargroupInfo);
	dst["groupInfo"] = vargroupInfo;
	dst["memberUserIDs"] = src.memberUserIDs;
	dst["adminUserIDs"] = src.adminUserIDs;
	dst["ownerUserID"] = src.ownerUserID;
}

void CRIM::Struct_Conv(const QVariantMap &src, CRIMGrpCreateInfo &dst) {
	Struct_Conv(src["groupInfo"].toMap(), dst.groupInfo);
	dst.memberUserIDs = src["memberUserIDs"].toStringList();
	dst.adminUserIDs = src["adminUserIDs"].toStringList();
	dst.ownerUserID = src["ownerUserID"].toString();
}

void CRIM::Struct_Conv(const CRIMGrpInfo &src, QVariantMap &dst) {
	Struct_Conv(*((CRIMGrpBaseInfo*)&src), dst);
    dst["groupID"] = src.groupID;
    dst["ownerUserID"] = src.ownerUserID;
    dst["createTime"] = src.createTime;
    dst["memberCount"] = src.memberCount;
    dst["status"] = src.status;
	dst["creatorUserID"] = src.creatorUserID;
	dst["needVerification"] = src.needVerification;
	dst["lookMemberInfo"] = src.lookMemberInfo;
	dst["applyMemberFriend"] = src.applyMemberFriend;
	dst["notificationUpdateTime"] = src.notificationUpdateTime;
	dst["notificationUserID"] = src.notificationUserID;
}

void CRIM::Struct_Conv(const QVariantMap &src, CRIMGrpInfo &dst) {
	Struct_Conv(src, *((CRIMGrpBaseInfo*)&dst));
    dst.groupID = src["groupID"].toString();
    dst.ownerUserID = src["ownerUserID"].toString();
    dst.createTime = src["createTime"].toLongLong();
    dst.memberCount = src["memberCount"].toInt();
	dst.status = (CRIMGroupStatus)src["status"].toInt();
	dst.creatorUserID = src["creatorUserID"].toString();
	dst.needVerification = (CRIMGroupVerificationType)src["needVerification"].toInt();
	dst.lookMemberInfo = src["lookMemberInfo"].toInt();
	dst.applyMemberFriend = src["applyMemberFriend"].toInt();
	dst.notificationUpdateTime = src["notificationUpdateTime"].toLongLong();
	dst.notificationUserID = src["notificationUserID"].toString();
}


void CRIM::Struct_Conv(const CRIMGrpMemberInfo &src, QVariantMap &dst) {
	Struct_Conv(*((CRIMPublicUserInfo*)&src), dst);
	dst["groupID"] = src.groupID;
	dst["joinTime"] = src.joinTime;
	dst["joinSource"] = src.joinSource;
	dst["operatorUserID"] = src.operatorUserID;
	dst["muteEndTime"] = src.muteEndTime;
	dst["inviterUserID"] = src.inviterUserID;
	dst["roleLevel"] = src.roleLevel;
}

void CRIM::Struct_Conv(const QVariantMap &src, CRIMGrpMemberInfo &dst) {
	Struct_Conv(src, *((CRIMPublicUserInfo*)&dst));
	dst.groupID = src["groupID"].toString();
	dst.joinTime = src["joinTime"].toLongLong();
	dst.joinSource = (CRIMJoinType)src["joinSource"].toInt();
	dst.operatorUserID = src["operatorUserID"].toString();
	dst.muteEndTime = src["muteEndTime"].toLongLong();
	dst.inviterUserID = src["inviterUserID"].toString();
	dst.roleLevel = (CRIMGroupMemberRole)src["roleLevel"].toInt();
}

void CRIM::Struct_Conv(const CRIMGrpReqInfo &src, QVariantMap &dst) {
    dst["groupID"] = src.groupID;
    dst["groupName"] = src.groupName;
    dst["groupFaceURL"] = src.groupFaceURL;
    dst["userID"] = src.userID;
    dst["nickname"] = src.nickname;
    dst["userFaceURL"] = src.userFaceURL;
    dst["handleResult"] = src.handleResult;
    dst["reqMsg"] = src.reqMsg;
    dst["handledMsg"] = src.handledMsg;
    dst["reqTime"] = src.reqTime;
    dst["handleUserID"] = src.handleUserID;
    dst["handledTime"] = src.handledTime;
	dst["ex"] = VarMapToJson(src.ex);
	dst["joinSource"] = src.joinSource;
	dst["inviterUserID"] = src.inviterUserID;
}

void CRIM::Struct_Conv(const QVariantMap &src, CRIMGrpReqInfo &dst) {
    dst.groupID = src["groupID"].toString();
    dst.groupName = src["groupName"].toString();
    dst.groupFaceURL = src["groupFaceURL"].toString();
    dst.userID = src["userID"].toString();
    dst.nickname = src["nickname"].toString();
    dst.userFaceURL = src["userFaceURL"].toString();
	dst.handleResult = (CRIMReqStatus)src["handleResult"].toInt();
    dst.reqMsg = src["reqMsg"].toString();
    dst.handledMsg = src["handledMsg"].toString();
    dst.reqTime = src["reqTime"].toLongLong();
    dst.handleUserID = src["handleUserID"].toString();
    dst.handledTime = src["handledTime"].toLongLong();
	dst.ex = JsonToVarMap(src["ex"].toString());
	dst.joinSource = (CRIMJoinType)src["joinSource"].toInt();
	dst.inviterUserID = src["inviterUserID"].toString();
}

void CRIM::Struct_Conv(const CRIMSimpleResultInfo &src, QVariantMap &dst) {
    dst["userID"] = src.userID;
    dst["result"] = src.result;
}

void CRIM::Struct_Conv(const QVariantMap &src, CRIMSimpleResultInfo &dst) {
    dst.userID = src["userID"].toString();
    dst.result = src["result"].toInt();
}

void CRIM::Struct_Conv(const CRIMConversationInfo &src, QVariantMap &dst) {
	dst["conversationID"] = src.conversationID;
	dst["conversationType"] = src.conversationType;
	dst["userID"] = src.userID;
	dst["groupID"] = src.groupID;
	dst["showName"] = src.orgName;
	dst["faceURL"] = src.faceURL;
	dst["recvMsgOpt"] = src.recvMsgOpt;
	dst["unreadCount"] = src.unreadCount;
	dst["groupAtType"] = src.groupAtType;
	dst["latestMsgSendTime"] = src.latestMsgSendTime;
	dst["draftText"] = src.draftText;
	dst["draftTextTime"] = src.draftTextTime;
	dst["isPinned"] = src.isPinned;
	dst["isPrivateChat"] = src.isPrivateChat;
	dst["isNotInGroup"] = src.isNotInGroup;
	dst["burnDuration"] = src.burnDuration;
	dst["attachedInfo"] = src.attachedInfo;
	dst["ex"] = VarMapToJson(src.ex);
	if (src.latestMsg != NULL)
	{
		QVariantMap varMsg;
		Struct_Conv(*src.latestMsg, varMsg);
		dst["latestMsg"] = varMsg;
	}
}

void CRIM::Struct_Conv(const QVariantMap &src, CRIMConversationInfo &dst) {
	dst.conversationID = src["conversationID"].toString();
	dst.conversationType = (CRIMConversationType)src["conversationType"].toInt();
	dst.userID = src["userID"].toString();
	dst.groupID = src["groupID"].toString();
	dst.orgName = src["showName"].toString();
	dst.faceURL = src["faceURL"].toString();
	dst.recvMsgOpt = (CRIMReceiveMsgOpt)src["recvMsgOpt"].toInt();
	dst.unreadCount = src["unreadCount"].toInt();
	dst.groupAtType = (CRIMGroupAtType)src["groupAtType"].toInt();
	dst.latestMsgSendTime = src["latestMsgSendTime"].toLongLong();
	dst.draftText = src["draftText"].toString();
	dst.draftTextTime = src["draftTextTime"].toLongLong();
	dst.isPinned = src["isPinned"].toBool();
	dst.isPrivateChat = src["isPrivateChat"].toBool();
	dst.isNotInGroup = src["isNotInGroup"].toBool();
	dst.isPrivateChat = src["isPrivateChat"].toBool();
	dst.burnDuration = src["burnDuration"].toLongLong();	
	dst.attachedInfo = src["attachedInfo"].toString();
	dst.ex = JsonToVarMap(src["ex"].toString());
	if (src["latestMsg"].isValid())
	{
		dst.latestMsg = CRIMMessagePtr(new CRIMMessage());
		*dst.latestMsg = toStruct<CRIMMessage>(src["latestMsg"].toString().toUtf8());
	}
}

void CRIM::Struct_Conv(const CRIMNotDisturbInfo &src, QVariantMap &dst) {
	dst["conversationId"] = src.conversationId;
	dst["result"] = src.result;
}

void CRIM::Struct_Conv(const QVariantMap &src, CRIMNotDisturbInfo &dst) {
	dst.conversationId = src["conversationId"].toString();
	dst.result = src["result"].toInt();
}

void CRIM::Struct_Conv(const CRIMInvitationInfo &src, QVariantMap &dst) {
	dst["invitationMsgId"] = src.invitationMsgId;
    dst["inviterUserID"] = src.inviterUserID;
    dst["groupID"] = src.groupID;
    dst["inviteeUserIDList"] = src.inviteeUserIDList;
    dst["roomID"] = src.roomID;
    dst["mediaType"] = src.mediaType;
	dst["initiateTime"] = src.initiateTime;
    dst["timeout"] = src.timeout;
	dst["acceptTime"] = src.acceptTime;
	dst["duration"] = src.duration;
	dst["opType"] = src.opType;
	dst["opDesc"] = src.opDesc;
}

void CRIM::Struct_Conv(const QVariantMap &src, CRIMInvitationInfo &dst) {
	dst.invitationMsgId = src["invitationMsgId"].toString();
    dst.inviterUserID = src["inviterUserID"].toString();
    dst.groupID = src["groupID"].toString();
    dst.inviteeUserIDList = src["inviteeUserIDList"].toStringList();
    dst.roomID = src["roomID"].toString();
    dst.mediaType = src["mediaType"].toString();
	dst.initiateTime = src["initiateTime"].toLongLong();
    dst.timeout = src["timeout"].toLongLong();
	dst.acceptTime = src["acceptTime"].toLongLong();
	dst.duration = src["duration"].toLongLong();
	dst.opType = (CRInvitationOpType)src["opType"].toInt();
	dst.opDesc = src["opDesc"].toString();
}

void CRIM::Struct_Conv(const CRIMInvitationResultInfo &src, QVariantMap &dst) {
    dst["token"] = src.token;
    dst["roomID"] = src.roomID;
    dst["liveURL"] = src.liveURL;
}

void CRIM::Struct_Conv(const QVariantMap &src, CRIMInvitationResultInfo &dst) {
    dst.token = src["token"].toString();
    dst.roomID = src["roomID"].toString();
    dst.liveURL = src["liveURL"].toString();
}

void CRIM::Struct_Conv(const CRIMSearchResultItem &src, QVariantMap &dst) {
    dst["conversationID"] = src.conversationID;
	dst["messageCount"] = src.messageCount;
	dst["showName"] = src.showName;
	dst["faceURL"] = src.faceURL;
    
	QVariantList varMessageList;
	varMessageList.reserve(src.messageList.size());
	for (auto &msg : src.messageList) {
		if (msg != NULL)
		{
			QVariantMap varMessage;
			Struct_Conv(*msg, varMessage);
			varMessageList.append(varMessage);
		}
	}
	dst["messageList"] = varMessageList;
}

void CRIM::Struct_Conv(const QVariantMap &src, CRIMSearchResultItem &dst) {
    dst.conversationID = src["conversationID"].toString();
    dst.messageCount = src["messageCount"].toInt();
	dst.showName = src["showName"].toString();
	dst.faceURL = src["faceURL"].toString();

	QList<QVariant> varmessageList = src["messageList"].toList();
	dst.messageList.reserve(varmessageList.size());
	for (auto &var : varmessageList)
	{
		CRIMMessagePtr msgPtr = CRIMMessagePtr(new CRIMMessage());
		Struct_Conv(var.toMap(), *msgPtr);
		dst.messageList.append(msgPtr);
	}
}


void CRIM::Struct_Conv(const QVariantMap &src, CRIMSearchResultInfo &dst) {

    dst.totalCount = src["totalCount"].toInt();
	dst.items.clear();

	QVariant varRsltItems = src["searchResultItems"];
	if (varRsltItems.isValid())
	{
		QList<QVariant> varsearchResultItems = varRsltItems.toList();
		dst.items.reserve(varsearchResultItems.size());
		for (auto &infoVar : varsearchResultItems) {
			CRIMSearchResultItem info;
			Struct_Conv(infoVar.toMap(), info);
			dst.items.append(info);
		}
		return;
	}
    
	varRsltItems = src["findResultItems"];
	if (varRsltItems.isValid())
	{
		QList<QVariant> varsearchResultItems = varRsltItems.toList();
		dst.items.reserve(varsearchResultItems.size());
		for (auto &infoVar : varsearchResultItems) {
			CRIMSearchResultItem info;
			Struct_Conv(infoVar.toMap(), info);
			dst.items.append(info);
		}
		return;
	}
}

void CRIM::Struct_Conv(const CRIMTextElem &src, QVariantMap &dst) {
	dst["content"] = src.content;
}

void CRIM::Struct_Conv(const QVariantMap &src, CRIMTextElem &dst) {
	dst.content = src["content"].toString();
}

void CRIM::Struct_Conv(const CRIMCardElem &src, QVariantMap &dst) {
	dst["userID"] = src.userID;
	dst["nickname"] = src.nickname;
	dst["faceURL"] = src.faceURL;
	dst["ex"] = src.ex;
}

void CRIM::Struct_Conv(const QVariantMap &src, CRIMCardElem &dst) {
	dst.userID = src["userID"].toString();
	dst.nickname = src["nickname"].toString();
	dst.faceURL = src["faceURL"].toString();
	dst.ex = src["ex"].toString();
}

void CRIM::Struct_Conv(const CRIMTypingElem &src, QVariantMap &dst) {
	dst["msgTips"] = src.msgTips;
}

void CRIM::Struct_Conv(const QVariantMap &src, CRIMTypingElem &dst) {
	dst.msgTips = src["msgTips"].toString();
}

void CRIM::Struct_Conv(const CRIMAdvancedMsg &src, QVariantMap &dst) {
	dst["errCode"] = src.errCode;
	dst["errMsg"] = src.errMsg;
	dst["lastMinSeq"] = src.lastMinSeq;
	dst["isEnd"] = src.isEnd;

	QVariantList varListVar;
	varListVar.reserve(src.messageList.size());
	for (auto &st : src.messageList) {
		QVariantMap var;
		Struct_Conv(st, var);
		varListVar.append(var);
	}
	dst["messageList"] = varListVar;
}

void CRIM::Struct_Conv(const QVariantMap &src, CRIMAdvancedMsg &dst) {
	dst.errCode = src["errCode"].toInt();
	dst.errMsg = src["errMsg"].toString();
	dst.lastMinSeq = src["lastMinSeq"].toInt();
	dst.isEnd = src["isEnd"].toBool();

	dst.messageList.clear();
	QVariantList varList = src["messageList"].toList();
	for (auto &var : varList) {
		CRIMMessage st;
		Struct_Conv(var.toMap(), st);
		dst.messageList.append(st);
	}
}

void CRIM::Struct_Conv(const CRIMRevokedInfo &src, QVariantMap &dst) {
	dst["revokerID"] = src.revokerID;
	dst["revokerRole"] = src.revokerRole;
	dst["revokerNickname"] = src.revokerNickname;
	dst["clientMsgID"] = src.clientMsgID;
	dst["revokeTime"] = src.revokeTime;
	dst["sourceMessageSendTime"] = src.sourceMessageSendTime;
	dst["sourceMessageSendID"] = src.sourceMessageSendID;
	dst["sourceMessageSenderNickname"] = src.sourceMessageSenderNickname;
	dst["sessionType"] = src.sessionType;
}

void CRIM::Struct_Conv(const QVariantMap &src, CRIMRevokedInfo &dst) {
	dst.revokerID = src["revokerID"].toString();
	dst.revokerRole = (CRIMGroupMemberRole)src["revokerRole"].toInt();
	dst.revokerNickname = src["revokerNickname"].toString();
	dst.clientMsgID = src["clientMsgID"].toString();
	dst.revokeTime = src["revokeTime"].toLongLong();
	dst.sourceMessageSendTime = src["sourceMessageSendTime"].toLongLong();
	dst.sourceMessageSendID = src["sourceMessageSendID"].toString();
	dst.sourceMessageSenderNickname = src["sourceMessageSenderNickname"].toString();
	dst.sessionType = (CRIMConversationType)src["sessionType"].toInt();
}

void CRIM::Struct_Conv(const CRIMGetFriendReqListAsRecipientReq &src, QVariantMap &dst)
{
	QVariantList handleResultsVar;
	handleResultsVar.reserve(src.handleResults.size());
	for (auto &item : src.handleResults) {
		handleResultsVar.append(item);
	}
	dst["handleResults"] = handleResultsVar;
	dst["offset"] = src.offset;
	dst["count"] = src.count;
}

void CRIM::Struct_Conv(const CRIMGetFriendReqListAsApplicantReq &src, QVariantMap &dst)
{
	dst["offset"] = src.offset;
	dst["count"] = src.count;
}

void CRIM::Struct_Conv(const CRIMGetGroupReqListAsRecipientReq &src, QVariantMap &dst)
{
	dst["offset"] = src.groupIDs;
	QVariantList handleResultsVar;
	handleResultsVar.reserve(src.handleResults.size());
	for (auto &item : src.handleResults) {
		handleResultsVar.append(item);
	}
	dst["handleResults"] = handleResultsVar;
	dst["offset"] = src.offset;
	dst["count"] = src.count;
}

void CRIM::Struct_Conv(const CRIMGetGroupReqListAsApplicantReq &src, QVariantMap &dst)
{
	dst["offset"] = src.groupIDs;
	QVariantList handleResultsVar;
	handleResultsVar.reserve(src.handleResults.size());
	for (auto &item : src.handleResults) {
		handleResultsVar.append(item);
	}
	dst["handleResults"] = handleResultsVar;
	dst["offset"] = src.offset;
	dst["count"] = src.count;
}

void CRIM::Struct_Conv(const CRIMUpdateFriendsReq &src, QVariantMap &dst)
{
	dst["ownerUserID"] = src.ownerUserID;
	dst["friendUserIDs"] = src.friendUserIDs;
	dst["remark"] = src.remark;
	dst["ex"] = src.ex;
	dst["isPinned"] = src.isPinned;
}