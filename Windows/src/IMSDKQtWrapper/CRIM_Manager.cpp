#include "stdafx.h"
#include "CRIM_Manager.h"
#include "CRIMSDK.h"
#include "CRIM_DataConv.h"
#include "CRIM_Callback.h"

#define qstrToCharPtr(str) str.toUtf8().constData()

#define QVariantToJson QVariantToJosnStr
#define QVariantToJsonPtr(variant)		QVariantToJson(variant).constData()
#define JsonToQVariantMap(by)			JsonStrToQVaraint(by).toMap()

#define DECLARED_CALLRSP_CONNECT(FUNC) \
connect(this, &CRIMManager_Qt::m_##FUNC##Success, this, &CRIMManager_Qt::slot_##FUNC##Success); \
connect(this, &CRIMManager_Qt::m_##FUNC##Failed, this, &CRIMManager_Qt::slot_##FUNC##Failed)

#define DECLARED_NOTIFY_CONNECT(FUNC) \
connect(this, &CRIMManager_Qt::m_##FUNC, this, &CRIMManager_Qt::slot_##FUNC)

CRIMManager_Qt* CRIMManager_Qt::instance()
{
    static CRIMManager_Qt ins;
    return &ins;
}

QString CRIMManager_Qt::getSDKVersion()
{
	return CRIMSDK::getVersion();
}

CRIMManager_Qt::CRIMManager_Qt()
{
    qRegisterMetaType<CRIMMessage>("CRIMMessage");
    qRegisterMetaType<QList<CRIMMessage>>("QList<CRIMMessage>");
    qRegisterMetaType<CRIMGrpInfo>("CRIMGrpInfo");
    qRegisterMetaType<QList<CRIMGrpInfo>>("QList<CRIMGrpInfo>");
    qRegisterMetaType<CRIMGrpMemberInfo>("CRIMGrpMemberInfo");
    qRegisterMetaType<QList<CRIMGrpMemberInfo>>("QList<CRIMGrpMemberInfo>");
    qRegisterMetaType<QList<CRIMSimpleResultInfo>>("QList<CRIMSimpleResultInfo>");
    qRegisterMetaType<CRIMGrpReqInfo>("CRIMGrpReqInfo");
    qRegisterMetaType<QList<CRIMFriendReqInfo>>("QList<CRIMFriendReqInfo>");
    qRegisterMetaType<CRIMConversationInfo>("CRIMConversationInfo");
    qRegisterMetaType<QList<CRIMConversationInfo>>("QList<CRIMConversationInfo>");
    qRegisterMetaType<CRIMSearchResultInfo>("CRIMSearchResultInfo");
    qRegisterMetaType<CRIMInvitationResultInfo>("CRIMInvitationResultInfo");
    qRegisterMetaType<QList<CRIMReadReceiptInfo>>("QList<CRIMReadReceiptInfo>");
	qRegisterMetaType<CRIMFriendReqInfo>("CRIMFriendReqInfo");
	qRegisterMetaType<CRIMFriendInfo>("CRIMFriendInfo");
	qRegisterMetaType<QList<CRIMFriendInfo>>("QList<CRIMFriendInfo>");
    qRegisterMetaType<CRIMBlackListInfo>("CRIMBlackListInfo");
    qRegisterMetaType<CRIMInvitationInfo>("CRIMInvitationInfo");
	qRegisterMetaType<QList<CRIMNotDisturbInfo>>("QList<CRIMNotDisturbInfo>");
	qRegisterMetaType<CRIMAdvancedMsg>("CRIMAdvancedMsg");
	qRegisterMetaType<CRIMRevokedInfo>("CRIMRevokedInfo");
	qRegisterMetaType<CRIMUserInfo>("CRIMUserInfo");
	qRegisterMetaType<QList<CRIMUserInfo>>("QList<CRIMUserInfo>");
	qRegisterMetaType<CRIMUserStatusInfo>("CRIMUserStatusInfo");
	qRegisterMetaType<QList<CRIMUserStatusInfo>>("QList<CRIMUserStatusInfo>");
	
    m_imsdk = new CRIMSDK::CRIMManager();
    m_callBack = new CRIMCallbacker_Qt;
    DECLARED_CALLRSP_CONNECT(login);
    DECLARED_NOTIFY_CONNECT(connectFailed);
	DECLARED_NOTIFY_CONNECT(kickedOffline);
	DECLARED_NOTIFY_CONNECT(syncServerFinish);
	DECLARED_CALLRSP_CONNECT(getAllConversationList);
    DECLARED_NOTIFY_CONNECT(newConversation);
	DECLARED_CALLRSP_CONNECT(getSelfInfo);
    DECLARED_CALLRSP_CONNECT(getJoinedGrpList);
    DECLARED_CALLRSP_CONNECT(getGrpMemberList);
    DECLARED_CALLRSP_CONNECT(getSpecifiedGrpsInfo);
    DECLARED_NOTIFY_CONNECT(joinedGrpAdded);
    DECLARED_NOTIFY_CONNECT(joinedGrpDeleted);
    DECLARED_NOTIFY_CONNECT(joinedGrpDismissed);
    DECLARED_NOTIFY_CONNECT(grpInfoChanged);
    DECLARED_NOTIFY_CONNECT(grpMemberInfoChanged);
    DECLARED_NOTIFY_CONNECT(grpMemberAdded);
    DECLARED_NOTIFY_CONNECT(grpMemberDeleted);
    DECLARED_CALLRSP_CONNECT(getFriendList);
    DECLARED_NOTIFY_CONNECT(friendAdded);
    DECLARED_NOTIFY_CONNECT(friendDeleted);
    DECLARED_NOTIFY_CONNECT(friendInfoChanged);
    connect(&m_checkGrpTimer, &QTimer::timeout, this, &CRIMManager_Qt::slot_checkGrpTimeout);


	connect(this, &CRIMManager_Qt::m_getHistoryMsgListSuccess, this, [this](const CRIMAdvancedMsg &hisMsg, const QString &cookie){
		this->m_lastMinSeq = hisMsg.lastMinSeq;
		emit s_getHistoryMsgListSuccess(hisMsg.messageList, hisMsg.isEnd, cookie);
	});
	connect(this, &CRIMManager_Qt::m_getHistoryMsgListFailed, this, [this](int code, const QString &error, const QString &cookie){
		emit s_getHistoryMsgListFailed(code, error, cookie);
	});
	connect(this, &CRIMManager_Qt::m_getHistoryMsgListReverseSuccess, this, [this](const CRIMAdvancedMsg &hisMsg, const QString &cookie){
		this->m_lastMinSeq = hisMsg.lastMinSeq;
		emit s_getHistoryMsgListReverseSuccess(hisMsg.messageList, hisMsg.isEnd, cookie);
	});
	connect(this, &CRIMManager_Qt::m_getHistoryMsgListReverseFailed, this, [this](int code, const QString &error, const QString &cookie){
		emit s_getHistoryMsgListReverseFailed(code, error, cookie);
	});
}

CRIMManager_Qt::~CRIMManager_Qt()
{
    unInitSDK();
    delete m_callBack;
    delete m_imsdk;
}

//! [imsdk qt封装接口]
bool CRIMManager_Qt::initSDK(const QString &apiUrl, bool skipVerifyCert, const QString &storageDir, const QString &logDir, CRIMLogLevel logLevel)
{
    QVariantMap varMap;
    varMap["platformID"] = m_platform;
	varMap["sdkServer"] = apiUrl;
	varMap["skipVerifyCert"] = skipVerifyCert;
    varMap["dataDir"] = storageDir;
    varMap["logFilePath"] = logDir;
    varMap["logLevel"] = logLevel;

    CRIMMgrLogDebug("initIMSDK, ver:%s, apiUrl:%s, skipVerifyCert:%d, storageDir:%s, logDir:%s, logLv:%d", CRIMSDK::getVersion()
		, apiUrl.toUtf8().constData(), skipVerifyCert, storageDir.toUtf8().constData(), logDir.toUtf8().constData(), logLevel);
    m_bInit = m_imsdk->initSDK(QVariantToJsonPtr(varMap));
    if (m_bInit)
    {
        m_imsdk->addCallBack(m_callBack);
    }
    else
    {
        CRIMMgrLogWarn("initSDK failed!");
    }
    return m_bInit;
}

void CRIMManager_Qt::unInitSDK()
{
    if (m_bInit)
    {
        CRIMMgrLogDebug("unInitSDK...");
		m_imsdk->rmCallBack(m_callBack);
        m_imsdk->unInitSDK();
    }
    m_bInit = false;
}

void CRIMManager_Qt::loginWithAppID(const QString &userID, const QString &appId, const QString &appSecret, const QString &cookie)
{
    m_userID = userID;
    m_usrLoginCookie = cookie;

	m_loginState = CRIMMGR_LOGINING;
	m_curLoginSession = makeUUID();
	m_cacheDataState = CACHE_NULL;
    m_bUiGrpInfosGeted = false;
	m_bFriendInfoGeted = false;
	m_bSyncServerFinish = false;
    m_UiGrpInfos.clear();
    m_friendInfos.clear();

    CRIMMgrLogDebug("loginWithAppID...uid:%s, appId:%s", userID.toUtf8().constData(), appId.toUtf8().constData());
    m_imsdk->loginWithAppID(qstrToCharPtr(userID), qstrToCharPtr(appId), qstrToCharPtr(appSecret), qstrToCharPtr(cookie));
}

void CRIMManager_Qt::loginWithToken(const QString &userID, const QString &token, const QString &cookie)
{
    m_userID = userID;
    m_usrLoginCookie = cookie;

	m_loginState = CRIMMGR_LOGINING;
	m_curLoginSession = makeUUID();
	m_cacheDataState = CACHE_NULL;
    m_bUiGrpInfosGeted = false;
	m_bFriendInfoGeted = false;
	m_bSyncServerFinish = false;
    m_UiGrpInfos.clear();
    m_friendInfos.clear();

    CRIMMgrLogDebug("loginWithToken...uid:%s", userID.toUtf8().constData());
    m_imsdk->loginWithToken(qstrToCharPtr(userID), qstrToCharPtr(token),  qstrToCharPtr(cookie));
}

void CRIMManager_Qt::logout(const QString &cookie)
{
	if (m_loginState == CRIMMGR_LOGOUT)
	{
		return;
	}

	m_loginState = CRIMMGR_LOGOUT;
    m_userID = "";
    m_curLoginSession.clear();
    m_checkGrpTimer.stop();
	m_selfInfo.userID.clear();

    CRIMMgrLogDebug("logout...");
    m_imsdk->logout(qstrToCharPtr(cookie));
    QThread::msleep(30);
}

int64_t CRIMManager_Qt::getCurrentSvrTime()
{
	int64_t ts = m_imsdk->getCurrentSvrTime();
	return ts;
}


void CRIMManager_Qt::uploadFile(const QString &filePath, const QString &cause, const QString &cookie)
{
	QFileInfo fi(filePath);
	QVariantMap varMap;
	varMap["filepath"] = filePath;
	varMap["name"] = fi.fileName();
	varMap["cause"] = cause;
	QVariantMap cookieMap;
	cookieMap["filepath"] = filePath;
	cookieMap["usrCookie"] = cookie;

	CRIMMgrLogDebug("uploadFile...filePath:%s", filePath.toUtf8().constData());
	m_imsdk->uploadFile(QVariantToJsonPtr(varMap), QVariantToJsonPtr(cookieMap));
}

CRIMMessage CRIMManager_Qt::createTextMsg(const QString &text)
{
    CRIMString jsonMsg = m_imsdk->createTextMsg(qstrToCharPtr(text));
    return CRIM::toStruct<CRIMMessage>(jsonMsg.constData());
}

CRIMMessage CRIMManager_Qt::createTextAtMsg(const QString &text, const QStringList &atUserIDList, const QList<CRIMAtInfo> &atUserInfoList, const CRIMMessage &quoteMessage)
{
    QByteArray jsonQuoteMessage;
    if (!quoteMessage.clientMsgID.isEmpty())
    {
        jsonQuoteMessage = CRIM::toJson(quoteMessage);
    }

    CRIMString jsonMsg = m_imsdk->createTextAtMsg(qstrToCharPtr(text), QVariantToJsonPtr(atUserIDList), CRIM::toJsonList(atUserInfoList).constData(), jsonQuoteMessage.constData());

    return CRIM::toStruct<CRIMMessage>(jsonMsg.constData());
}

CRIMMessage CRIMManager_Qt::createLocationMsg(const QString &description, double longitude, double latitude)
{
    CRIMString jsonMsg = m_imsdk->createLocationMsg(qstrToCharPtr(description), longitude, latitude);
	
    return CRIM::toStruct<CRIMMessage>(jsonMsg.constData());
}

CRIMMessage CRIMManager_Qt::createCustomMsg(const QString &data, const QString &extension, const QString &description)
{
    CRIMString jsonMsg = m_imsdk->createCustomMsg(qstrToCharPtr(data), qstrToCharPtr(extension), qstrToCharPtr(description));

    return CRIM::toStruct<CRIMMessage>(jsonMsg.constData());
}

CRIMMessage CRIMManager_Qt::createQuoteMsg(const QString &text, const CRIMMessage &message)
{
    CRIMString jsonMsg = m_imsdk->createQuoteMsg(qstrToCharPtr(text), CRIM::toJson(message).constData());

    return CRIM::toStruct<CRIMMessage>(jsonMsg.constData());
}

CRIMMessage CRIMManager_Qt::createCardMsg(const CRIMCardElem &card)
{
    CRIMString jsonMsg = m_imsdk->createCardMsg(CRIM::toJson(card).constData());

    return CRIM::toStruct<CRIMMessage>(jsonMsg.constData());
}

CRIMMessage CRIMManager_Qt::createImageMsgFromFullPath(const QString &imagePath)
{
    CRIMString jsonMsg = m_imsdk->createImageMsgFromFullPath(qstrToCharPtr(imagePath));
    return CRIM::toStruct<CRIMMessage>(jsonMsg.constData());
}

CRIMMessage CRIMManager_Qt::createImageMsgByURL(const QString &sourcePath, const CRIMPictureInfo &sourcePicture, const CRIMPictureInfo &bigPicture, const CRIMPictureInfo &snapshotPicture)
{
    CRIMString jsonMsg = m_imsdk->createImageMsgByURL(qstrToCharPtr(sourcePath), CRIM::toJson(sourcePicture).constData(), CRIM::toJson(bigPicture).constData(), CRIM::toJson(snapshotPicture).constData());

    return CRIM::toStruct<CRIMMessage>(jsonMsg.constData());
}

CRIMMessage CRIMManager_Qt::createVideoMsgFromFullPath(const QString &videoPath, const QString &videoType, qint64 duration, const QString &snapshotPath)
{
    CRIMString jsonMsg = m_imsdk->createVideoMsgFromFullPath(qstrToCharPtr(videoPath), qstrToCharPtr(videoType), duration, qstrToCharPtr(snapshotPath));

    return CRIM::toStruct<CRIMMessage>(jsonMsg.constData());
}

CRIMMessage CRIMManager_Qt::createVideoMsgByURL(const CRIMVideoElem &videoElem)
{
    CRIMString jsonMsg = m_imsdk->createVideoMsgByURL(CRIM::toJson(videoElem).constData());

    return CRIM::toStruct<CRIMMessage>(jsonMsg.constData());
}

CRIMMessage CRIMManager_Qt::createSoundMsgFromFullPath(const QString &soundPath, qint64 duration)
{
    CRIMString jsonMsg = m_imsdk->createSoundMsgFromFullPath(qstrToCharPtr(soundPath), duration);

    return CRIM::toStruct<CRIMMessage>(jsonMsg.constData());
}

CRIMMessage CRIMManager_Qt::createSoundMsgByURL(const CRIMSoundElem &soundElem)
{
    CRIMString jsonMsg = m_imsdk->createSoundMsgByURL(CRIM::toJson(soundElem).constData());

    return CRIM::toStruct<CRIMMessage>(jsonMsg.constData());
}

CRIMMessage CRIMManager_Qt::createFileMsgFromFullPath(const QString &filePath, const QString &fileName)
{
    CRIMString jsonMsg = m_imsdk->createFileMsgFromFullPath(qstrToCharPtr(filePath), qstrToCharPtr(fileName));

    return CRIM::toStruct<CRIMMessage>(jsonMsg.constData());
}

CRIMMessage CRIMManager_Qt::createFileMsgByURL(const CRIMFileElem &fileElem)
{
    CRIMString jsonMsg = m_imsdk->createFileMsgByURL(CRIM::toJson(fileElem).constData());

    return CRIM::toStruct<CRIMMessage>(jsonMsg.constData());
}

CRIMMessage CRIMManager_Qt::createMergerMsg(const QList<CRIMMessage> &messageList, const QString &title, const QStringList &summaryList)
{
    CRIMString jsonMsg = m_imsdk->createMergerMsg(CRIM::toJsonList(messageList).constData(), qstrToCharPtr(title), QVariantToJsonPtr(summaryList));

    return CRIM::toStruct<CRIMMessage>(jsonMsg.constData());
}

CRIMMessage CRIMManager_Qt::createFaceMsg(int index, const QString &data)
{
    CRIMString jsonMsg = m_imsdk->createFaceMsg(index, qstrToCharPtr(data));

    return CRIM::toStruct<CRIMMessage>(jsonMsg.constData());
}

CRIMMessage CRIMManager_Qt::createForwardMsg(const CRIMMessage &message)
{
    CRIMString jsonMsg = m_imsdk->createForwardMsg(CRIM::toJson(message).constData());

    return CRIM::toStruct<CRIMMessage>(jsonMsg.constData());
}

void CRIMManager_Qt::sendMsg(const CRIMMessage &message, const QString &recvUid, const QString &recvGid, const CRIMOfflinePushInfo &offlinePushInfo, const QString &cookie)
{
    QByteArray jsonMessage = CRIM::toJson<CRIMMessage>(message);

    QVariantMap cookieMap;
    cookieMap["msgID"] = message.clientMsgID;
    cookieMap["usrCookie"] = cookie;
    QByteArray newCookie = QVariantToJson(cookieMap);

	CRIMMgrLogDebug("sendMsg...recvUid:%s, recvGid:%s, clientMsgID:%s", recvUid.toUtf8().constData(), recvGid.toUtf8().constData(), message.clientMsgID.toUtf8().constData());
    m_imsdk->sendMsg(jsonMessage.constData(), qstrToCharPtr(recvUid), qstrToCharPtr(recvGid), CRIM::toJson(offlinePushInfo).constData(), false, newCookie.constData());
}

void CRIMManager_Qt::revokeMsg(const QString &conversationID, const QString &clientMsgID, const QString &cookie)
{
	QVariantMap cookieMap;
	cookieMap["conversationID"] = conversationID;
	cookieMap["msgID"] = clientMsgID;
	cookieMap["usrCookie"] = cookie;
	QByteArray newCookie = QVariantToJson(cookieMap);

	CRIMMgrLogDebug("revokeMsg...conversationID:%s, clientMsgID:%s", conversationID.toUtf8().constData(), clientMsgID.toUtf8().constData());
	m_imsdk->revokeMsg(qstrToCharPtr(conversationID), qstrToCharPtr(clientMsgID), newCookie.constData());
}

void CRIMManager_Qt::markMsgAsReadByMsgID(const QString &conversationID, const QStringList &clientMsgIDs, const QString &cookie)
{
    CRIMMgrLogDebug("markMsgAsReadByMsgID...conversationID:%s", conversationID.toUtf8().constData());
	QVariantMap cookieMap;
	cookieMap["conversationID"] = conversationID;
	cookieMap["msgIds"] = clientMsgIDs;
	cookieMap["usrCookie"] = cookie;
	QByteArray newCookie = QVariantToJson(cookieMap);

	m_imsdk->markMsgAsReadByMsgID(qstrToCharPtr(conversationID), QVariantToJsonPtr(QVariant(clientMsgIDs)), newCookie.constData());
}

void CRIMManager_Qt::typingStatusUpdate(const QString &userID, const QString &msgTip, const QString &cookie)
{
    CRIMMgrLogDebug("typingStatusUpdate...uid:%s, msgTip:%s", userID.toUtf8().constData(), msgTip.toUtf8().constData());
    m_imsdk->typingStatusUpdate(qstrToCharPtr(userID), qstrToCharPtr(msgTip), qstrToCharPtr(cookie));
}

void CRIMManager_Qt::getHistorySeqDeal(const QString &conversationID)
{
    if (conversationID != m_lastConversationID)
    {
        m_lastMinSeq = 0;
        m_lastConversationID = conversationID;
    }
}

void CRIMManager_Qt::getHistoryMsgList(const QString &conversationID, const QString &startClientMsgID, int count, const QString &cookie)
{
	getHistorySeqDeal(conversationID);

    QVariantMap varMap;
    varMap["lastMinSeq"] = m_lastMinSeq;
    varMap["conversationID"] = conversationID;
	varMap["startClientMsgID"] = startClientMsgID;
    varMap["count"] = count;

    QVariantMap cookieMap;
    cookieMap["conversationID"] = conversationID;
    cookieMap["usrCookie"] = cookie;
    QByteArray newCookie = QVariantToJson(cookieMap);

	CRIMMgrLogDebug("getHistoryMsgList...conversationID:%s, startMsgID:%s, count:%d", conversationID.toUtf8().constData(), startClientMsgID.toUtf8().constData(), count);
    m_imsdk->getAdvancedHistoryMsgList(QVariantToJsonPtr(varMap), newCookie.constData());
}

void CRIMManager_Qt::getHistoryMsgListReverse(const QString &conversationID, const QString &startClientMsgID, int count, const QString &cookie)
{
	getHistorySeqDeal(conversationID);
    QVariantMap varMap;
    varMap["lastMinSeq"] = m_lastMinSeq;
    varMap["conversationID"] = conversationID;
	varMap["startClientMsgID"] = startClientMsgID;
    varMap["count"] = count;

    QVariantMap cookieMap;
    cookieMap["conversationID"] = conversationID;
    cookieMap["usrCookie"] = cookie;
    QByteArray newCookie = QVariantToJson(cookieMap);
	CRIMMgrLogDebug("getHistoryMsgListReverse...conversationID:%s, startMsgID:%s, count:%d", conversationID.toUtf8().constData(), startClientMsgID.toUtf8().constData(), count);
    m_imsdk->getAdvancedHistoryMsgListReverse(QVariantToJsonPtr(varMap), newCookie.constData());
}

void CRIMManager_Qt::deleteMsg(const QString &conversationID, const QString &clientMsgID, const QString &cookie)
{
    CRIMMgrLogDebug("deleteMsg...conversationID:%s, clientMsgID:%s", conversationID.toUtf8().constData(), clientMsgID.toUtf8().constData());
    m_imsdk->deleteMsg(qstrToCharPtr(conversationID), qstrToCharPtr(clientMsgID), qstrToCharPtr(cookie));
}

void CRIMManager_Qt::deleteMsgFromLocal(const QString &conversationID, const QString &clientMsgID, const QString &cookie)
{
    CRIMMgrLogDebug("deleteMsgFromLocal...conversationID:%s, clientMsgID:%s", conversationID.toUtf8().constData(), clientMsgID.toUtf8().constData());
    m_imsdk->deleteMsgFromLocal(qstrToCharPtr(conversationID), qstrToCharPtr(clientMsgID), qstrToCharPtr(cookie));
}

void CRIMManager_Qt::deleteAllMsg(const QString &cookie)
{
    CRIMMgrLogDebug("deleteAllMsg...");
    m_imsdk->deleteAllMsg(qstrToCharPtr(cookie));
}

void CRIMManager_Qt::deleteAllMsgFromLocal(const QString &cookie)
{
    CRIMMgrLogDebug("deleteAllMsgFromLocal...");
    m_imsdk->deleteAllMsgFromLocal(qstrToCharPtr(cookie));
}

void CRIMManager_Qt::insertSingleMsgToLocal(const CRIMMessage &message, const QString &receiver, const QString &sender, const QString &cookie)
{
    QByteArray jsonMessage = CRIM::toJson(message);
	CRIMMgrLogDebug("insertSingleMsgToLocal...sendID:%s, recvID:%s, gid:%s, clientMsgID:%s", message.recvID.toUtf8().constData(), message.recvID.toUtf8().constData(), message.groupID.toUtf8().constData(), message.clientMsgID.toUtf8().constData());

    m_imsdk->insertSingleMsgToLocal(jsonMessage.constData(), qstrToCharPtr(receiver), qstrToCharPtr(sender), qstrToCharPtr(cookie));
}

void CRIMManager_Qt::insertGrpMsgToLocal(const CRIMMessage &message, const QString &groupID, const QString &sendID, const QString &cookie)
{
    QByteArray jsonMessage = CRIM::toJson(message);
	CRIMMgrLogDebug("insertGrpMsgToLocal...sendID:%s, recvID:%s, gid:%s, clientMsgID:%s", message.recvID.toUtf8().constData(), message.recvID.toUtf8().constData(), message.groupID.toUtf8().constData(), message.clientMsgID.toUtf8().constData());

    m_imsdk->insertGrpMsgToLocal(jsonMessage.constData(), qstrToCharPtr(groupID), qstrToCharPtr(sendID), qstrToCharPtr(cookie));
}

void CRIMManager_Qt::findMsgList(const QMap<QString, QStringList>  &findParams, const QString &cookie)
{
    QVariantList varList;
    for (auto it = findParams.begin(); it != findParams.end(); it++)
    {
        QVariantMap varMap;
        const QString &conversationID = it.key();
        const QStringList &clientMsgIDList = it.value();
        varMap["conversationID"] = conversationID;
        varMap["clientMsgIDList"] = QVariant(clientMsgIDList);
        varList.push_back(varMap);
    }
    CRIMMgrLogDebug("findMsgList...");
    
    m_imsdk->findMsgList(QVariantToJsonPtr(varList), qstrToCharPtr(cookie));
}

void CRIMManager_Qt::searchLocalMsgs(const QString &conversationID,
                                     const QStringList &keywordList,
                                     int keywordListMatchType,
                                     const QStringList &senderUserIDList,
                                     const QList<CRIMMsgType> &messageTypeList,
                                     int searchTimePosition,
                                     int searchTimePeriod,
                                     int pageIndex,
                                     int count,
                                     const QString &cookie)
{
    QVariantMap varMap;
    varMap["conversationID"] = conversationID;
    varMap["keywordList"] = keywordList;
    varMap["keywordListMatchType"] = keywordListMatchType;
    varMap["senderUserIDList"] = senderUserIDList;
    QVariantList varTypeList;
    for (auto type : messageTypeList)
    {
        varTypeList.append(type);
    }
    varMap["messageTypeList"] = varTypeList;
    varMap["searchTimePosition"] = searchTimePosition;
    varMap["searchTimePeriod"] = searchTimePeriod;
    varMap["pageIndex"] = pageIndex;
    varMap["count"] = count;

    CRIMMgrLogDebug("searchLocalMsgs...");
    m_imsdk->searchLocalMsgs(QVariantToJsonPtr(varMap), qstrToCharPtr(cookie));
}

void CRIMManager_Qt::setMsgLocalEx(const QString &conversationID, const QString &clientMsgID, const QVariantMap &localEx, const QString &cookie)
{
	CRIMMgrLogDebug("setMsgLocalEx...conversationID:%s, clientMsgID:%s", conversationID.toUtf8().constData(), clientMsgID.toUtf8().constData());

	m_imsdk->setMsgLocalEx(qstrToCharPtr(conversationID), qstrToCharPtr(clientMsgID), QVariantToJsonPtr(localEx), qstrToCharPtr(cookie));
}

void CRIMManager_Qt::getAllConversationList(const QString &cookie)
{
    CRIMMgrLogDebug("getAllConversationList...");
    m_imsdk->getAllConversationList(qstrToCharPtr(cookie));
}

void CRIMManager_Qt::getConversationListSplit(int offset, int count, const QString &cookie)
{
    CRIMMgrLogDebug("getConversationListSplit...offset:%d, count:%d", offset, count);
    m_imsdk->getConversationListSplit(offset, count, qstrToCharPtr(cookie));
}

QString CRIMManager_Qt::getConversationIDBySessionType(const QString &sourceId, CRIMConversationType sessionType)
{
    CRIMString cId = m_imsdk->getConversationIDBySessionType(qstrToCharPtr(sourceId), sessionType);
	QString str = QString::fromUtf8(cId.constData(), cId.size());
	if (str.startsWith('"'))
	{
		str.remove(0, 1);
		str.remove(str.length() - 1, 1);
	}
	return str;
}

void CRIMManager_Qt::getOneConversation(const QString &sourceId, CRIMConversationType sessionType, const QString &cookie)
{
    CRIMMgrLogDebug("getOneConversation...sourceId:%s, sessionType:%d", sourceId.toUtf8().constData(), sessionType);
    m_imsdk->getOneConversation(sessionType, qstrToCharPtr(sourceId), qstrToCharPtr(cookie));
}

void CRIMManager_Qt::getMultipleConversation(const QStringList &conversationIDs, const QString &cookie)
{
    CRIMMgrLogDebug("getMultipleConversation...");
    m_imsdk->getMultipleConversation(QVariantToJsonPtr(QVariant(conversationIDs)), qstrToCharPtr(cookie));
}

void CRIMManager_Qt::hideConversation(const QString &conversationID, const QString &cookie)
{
    CRIMMgrLogDebug("hideConversation...");
    m_imsdk->hideConversation(qstrToCharPtr(conversationID), qstrToCharPtr(cookie));
}

void CRIMManager_Qt::hideAllConversations(const QString &cookie)
{
    CRIMMgrLogDebug("hideAllConversations...");
    m_imsdk->hideAllConversations(qstrToCharPtr(cookie));
}

void CRIMManager_Qt::clearConversationAndDeleteAllMsg(const QString &conversationID, const QString &cookie)
{
    CRIMMgrLogDebug("clearConversationAndDeleteAllMsg...conversationID:%s", conversationID.toUtf8().constData());
    m_imsdk->clearConversationAndDeleteAllMsg(qstrToCharPtr(conversationID), qstrToCharPtr(cookie));
}

void CRIMManager_Qt::deleteConversationAndDeleteAllMsg(const QString &conversationID, const QString &cookie)
{
    CRIMMgrLogDebug("deleteConversationAndDeleteAllMsg...conversationID:%s", conversationID.toUtf8().constData());
	QVariantMap cookieMap;
	cookieMap["conversationID"] = conversationID;
	cookieMap["usrCookie"] = cookie;
	QByteArray newCookie = QVariantToJson(cookieMap);

	m_imsdk->deleteConversationAndDeleteAllMsg(qstrToCharPtr(conversationID), newCookie.constData());
}

void CRIMManager_Qt::pinConversation(const QString &conversationID, bool isPinned, const QString &cookie)
{
    CRIMMgrLogDebug("pinConversation...conversationID:%s, isPinned:%d", conversationID.toUtf8().constData(), isPinned);
    m_imsdk->pinConversation(qstrToCharPtr(conversationID), isPinned, qstrToCharPtr(cookie));
}

void CRIMManager_Qt::setConversationDraft(const QString &conversationID, const QString &draftText, const QString &cookie)
{
    CRIMMgrLogDebug("setConversationDraft...conversationID:%s, draftText:%s", conversationID.toUtf8().constData(), draftText.toUtf8().constData());
	m_imsdk->setConversationDraft(qstrToCharPtr(conversationID), qstrToCharPtr(draftText), qstrToCharPtr(cookie));
}

void CRIMManager_Qt::resetConversationGrpAtType(const QString &conversationID, const QString &cookie)
{
    CRIMMgrLogDebug("resetConversationGrpAtType...conversationID:%s", conversationID.toUtf8().constData());
    m_imsdk->resetConversationGrpAtType(qstrToCharPtr(conversationID), qstrToCharPtr(cookie));
}

QString CRIMManager_Qt::getAtAllTag()
{
    CRIMString cTag = m_imsdk->getAtAllTag();
    return QString::fromUtf8(cTag.constData(), cTag.size());
}

void CRIMManager_Qt::markConversationMsgAsRead(const QString &conversationID, const QString &cookie)
{
    CRIMMgrLogDebug("markConversationMsgAsRead...conversationID:%s", conversationID.toUtf8().constData());
	m_imsdk->markConversationMsgAsRead(qstrToCharPtr(conversationID), qstrToCharPtr(cookie));
}

void CRIMManager_Qt::getTotalUnreadMsgCount(const QString &cookie)
{
    CRIMMgrLogDebug("getTotalUnreadMsgCount...");
    m_imsdk->getTotalUnreadMsgCount(qstrToCharPtr(cookie));
}

void CRIMManager_Qt::setConversationPrivateChat(const QString &conversionID, bool isPrivate, int duration, const QString &cookie)
{
	CRIMMgrLogDebug("setConversationPrivateChat...conversionID:%s, isPrivate:%d, duration:%d", conversionID.toUtf8().constData(), isPrivate, duration);
    m_imsdk->setConversationPrivateChat(qstrToCharPtr(conversionID), isPrivate, duration, qstrToCharPtr(cookie));
}

void CRIMManager_Qt::setConversationRecvMsgOpt(const QString &conversationID, CRIMReceiveMsgOpt status, const QString &cookie)
{
    CRIMMgrLogDebug("setConversationRecvMsgOpt...conversationID:%s, status:%d", conversationID.toUtf8().constData(), status);
    m_imsdk->setConversationRecvMsgOpt(qstrToCharPtr(conversationID), status, qstrToCharPtr(cookie));
}

void CRIMManager_Qt::setSelfInfo(const QString &nickname, const QString &faceURL, const QString &ex, const QString &cookie) 
{
    QVariantMap varMap;
    varMap["nickname"] = nickname;
	varMap["faceURL"] = faceURL;
    varMap["ex"] = ex;
    CRIMMgrLogDebug("setSelfInfo...");
    
    m_imsdk->setSelfInfo(QVariantToJsonPtr(varMap), qstrToCharPtr(cookie));
}


void CRIMManager_Qt::setSelfLocInfo(const QString &nickname, const QString &faceURL, const QVariantMap &ex)
{
	QString userId = CRIMManager_Qt::instance()->getUserID();

	//更新m_selfInfo
	if (m_selfInfo.nickname != nickname
		|| m_selfInfo.faceURL != faceURL
		|| m_selfInfo.ex != ex)
	{
		CRIMUserInfo oldSelfInfo = m_selfInfo;
		m_selfInfo.nickname = nickname;
		m_selfInfo.faceURL = faceURL;
		m_selfInfo.ex = ex;
		emit s_selfInfoUpdated(oldSelfInfo, m_selfInfo);
	}

	//更新m_friendInfos
	auto fItem = m_friendInfos.find(userId);
	if (fItem != m_friendInfos.end())
	{
		CRIMFriendInfo &crInfo = fItem.value();
		CRIMFriendInfo old = crInfo;
		crInfo.nickname = nickname;
		crInfo.faceURL = faceURL;
		crInfo.ex = ex;
		emit s_friendInfoChanged(crInfo);
		emit s_friendInfoChanged2(old, crInfo);
	}

	//更新所有群
	for (auto &gInfo : m_UiGrpInfos)
	{
		auto mPos = gInfo._members.find(userId);
		if (mPos != gInfo._members.end())
		{
			CRIMGrpMemberInfo newMIfo = mPos.value();
			newMIfo.faceURL = faceURL;
			newMIfo.nickname = nickname;
			newMIfo.operatorUserID = userId;
			newMIfo.ex = ex;
			slot_grpMemberInfoChanged(newMIfo);
		}
	}
}


void CRIMManager_Qt::getSelfInfoFromSDK(const QString &cookie)
{
    CRIMMgrLogDebug("getSelfInfo...");
    m_imsdk->getSelfInfo(qstrToCharPtr(cookie));
}

void CRIMManager_Qt::getUsersInfo(const QStringList &uidList, const QString &cookie)
{
    CRIMMgrLogDebug("getUsersInfo...");
    m_imsdk->getUsersInfo(QVariantToJsonPtr(QVariant(uidList)), qstrToCharPtr(cookie));
}

void CRIMManager_Qt::getUserStatus(const QStringList &uidList, const QString &cookie)
{
    CRIMMgrLogDebug("getUserStatus...");
    m_imsdk->getUserStatus(QVariantToJsonPtr(QVariant(uidList)), qstrToCharPtr(cookie));
}

void CRIMManager_Qt::subscribeUsersStatus(const QStringList &uidList, const QString &cookie)
{
    CRIMMgrLogDebug("subscribeUsersStatus...");
    m_imsdk->subscribeUsersStatus(QVariantToJsonPtr(QVariant(uidList)), qstrToCharPtr(cookie));
}

void CRIMManager_Qt::unsubscribeUsersStatus(const QStringList &uidList, const QString &cookie)
{
    CRIMMgrLogDebug("unsubscribeUsersStatus...");
    m_imsdk->unsubscribeUsersStatus(QVariantToJsonPtr(QVariant(uidList)), qstrToCharPtr(cookie));
}

void CRIMManager_Qt::getSubscribeUsersStatus(const QString &cookie)
{
    CRIMMgrLogDebug("getSubscribeUsersStatus...");
    m_imsdk->getSubscribeUsersStatus(qstrToCharPtr(cookie));
}

void CRIMManager_Qt::getFriendListFromSDK(bool filterBlack, const QString &cookie)
{
    CRIMMgrLogDebug("getFriendList...");
	m_imsdk->getFriendList(filterBlack, qstrToCharPtr(cookie));
}

void CRIMManager_Qt::addFriend(const QString &uid, const QString &reqMessage, const QString &cookie)
{
    QVariantMap varMap;
    varMap["toUserID"] = uid;
    varMap["reqMsg"] = reqMessage;


    CRIMMgrLogDebug("addFriend...uid:%s", uid.toUtf8().constData());
    m_imsdk->addFriend(QVariantToJsonPtr(varMap), qstrToCharPtr(cookie));
}

void CRIMManager_Qt::deleteFriend(const QString &uid, const QString &cookie)
{
    CRIMMgrLogDebug("deleteFriend...uid:%s", uid.toUtf8().constData());
    m_imsdk->deleteFriend(qstrToCharPtr(uid), qstrToCharPtr(cookie));
}

void CRIMManager_Qt::setFriendRemark(const QString &uid, const QString &remark, const QString &cookie)
{
	CRIMUpdateFriendsReq req;
	req.ownerUserID = m_userID;
	req.friendUserIDs.append(uid);
	req.remark = remark;
	QByteArray jsonReq = CRIM::toJson<CRIMUpdateFriendsReq>(req);
    CRIMMgrLogDebug("setFriendRemark...uid:%s", uid.toUtf8().constData());
	m_imsdk->updateFriends(jsonReq.constData(), qstrToCharPtr(cookie));
}

QList<CRIMFriendInfo*> CRIMManager_Qt::searchFriends(const QString &keyword)
{
    QList<CRIMFriendInfo*> rslt;
    for (auto &fInfo : m_friendInfos)
    {
        if (isKeywordMatchFriend(keyword, fInfo))
        {
            rslt.push_back(&fInfo);
        }
    }
    return rslt;
}

bool CRIMManager_Qt::isKeywordMatchFriend(const QString &keyword, const CRIMFriendInfo& info)
{
    if (info.userID.contains(keyword, Qt::CaseInsensitive))
        return true;
    if (info.getDisplayName().contains(keyword, Qt::CaseInsensitive))
        return true;
    return false;
}

void CRIMManager_Qt::getFriendReqListAsApplicant(int offset, int count, const QString &cookie)
{
	CRIMGetFriendReqListAsApplicantReq req;
	req.offset = offset;
	req.count = count;
	QByteArray jsonReq = CRIM::toJson<CRIMGetFriendReqListAsApplicantReq>(req);


    CRIMMgrLogDebug("getFriendReqListAsApplicant...");
	m_imsdk->getFriendReqListAsApplicant(jsonReq.constData(), qstrToCharPtr(cookie));
}

void CRIMManager_Qt::getFriendReqListAsRecipient(int offset, int count, const QString &cookie)
{
	CRIMGetFriendReqListAsRecipientReq req;
	req.offset = offset;
	req.count = count;
	QByteArray jsonReq = CRIM::toJson<CRIMGetFriendReqListAsRecipientReq>(req);
    CRIMMgrLogDebug("getFriendReqListAsRecipient...");
	m_imsdk->getFriendReqListAsRecipient(jsonReq.constData(), qstrToCharPtr(cookie));
}

void CRIMManager_Qt::acceptFriendReq(const QString &uid, const QString &handleMsg, const QString &cookie)
{
    QVariantMap varMap;
    varMap["toUserID"] = uid;
    varMap["handleMsg"] = handleMsg;
    CRIMMgrLogDebug("acceptFriendReq...uid:%s, handleMsg:%s", uid.toUtf8().constData(), handleMsg.toUtf8().constData());
    m_imsdk->acceptFriendReq(QVariantToJsonPtr(varMap), qstrToCharPtr(cookie));

    //查询一下好友信息
    CRIMMgrLogDebug("start get my friends...");
    getFriendListFromSDK(false, m_curLoginSession);
}

void CRIMManager_Qt::refuseFriendReq(const QString &uid, const QString &handleMsg, const QString &cookie)
{
    QVariantMap varMap;
    varMap["toUserID"] = uid;
    varMap["handleMsg"] = handleMsg;

    CRIMMgrLogDebug("refuseFriendReq...uid:%s, handleMsg:%s", uid.toUtf8().constData(), handleMsg.toUtf8().constData());
    m_imsdk->refuseFriendReq(QVariantToJsonPtr(varMap), qstrToCharPtr(cookie));
}

void CRIMManager_Qt::getBlackList(const QString &cookie)
{
    CRIMMgrLogDebug("getBlackList...");
    m_imsdk->getBlackList(qstrToCharPtr(cookie));
}


void CRIMManager_Qt::addToBlackList(const QString &uid, const QString &cookie)
{
    CRIMMgrLogDebug("addToBlackList...");
    m_imsdk->addToBlackList(qstrToCharPtr(uid), "", qstrToCharPtr(cookie));
}

void CRIMManager_Qt::removeFromBlackList(const QString &uid, const QString &cookie)
{
    CRIMMgrLogDebug("removeFromBlackList...");
	m_imsdk->removeFromBlackList(qstrToCharPtr(uid), qstrToCharPtr(cookie));
}

void CRIMManager_Qt::createGrp(const CRIMGrpCreateInfo &info, const QString &cookie)
{
	QByteArray jsonInfo = CRIM::toJson(info);
    CRIMMgrLogDebug("createGrp...info:%s", jsonInfo.constData());

	m_imsdk->createGrp(jsonInfo.constData(), qstrToCharPtr(cookie));
}

void CRIMManager_Qt::joinGrp(const QString &gid, const QString &reason, CRIMJoinType joinSource, const QString &cookie)
{
    CRIMMgrLogDebug("joinGrp...gid:%s, reason:%s, joinSource:%d", gid.toUtf8().constData(), reason.toUtf8().constData(), joinSource);
    m_imsdk->joinGrp(qstrToCharPtr(gid), qstrToCharPtr(reason), joinSource, qstrToCharPtr(cookie));
}

void CRIMManager_Qt::quitGrp(const QString &gid, const QString &cookie)
{
    CRIMMgrLogDebug("quitGrp...gid:%s", gid.toUtf8().constData());
    m_imsdk->quitGrp(qstrToCharPtr(gid), qstrToCharPtr(cookie));
}

void CRIMManager_Qt::dismissGrp(const QString &gid, const QString &cookie)
{
    CRIMMgrLogDebug("dismissGrp...gid:%s", gid.toUtf8().constData());
    m_imsdk->dismissGrp(qstrToCharPtr(gid), qstrToCharPtr(cookie));
}

void CRIMManager_Qt::setGrpInfo(const CRIMGrpInfo &info, const QString &cookie)
{
    QByteArray jsonInfo = CRIM::toJson(info);
	CRIMMgrLogDebug("setGrpInfo...gid:%s", info.groupID.toUtf8().constData());
    m_imsdk->setGrpInfo(jsonInfo.constData(), qstrToCharPtr(cookie));
}

void CRIMManager_Qt::getJoinedGrpListFromSDK(const QString &cookie)
{
    CRIMMgrLogDebug("getJoinedGrpList...");
    m_imsdk->getJoinedGrpList(qstrToCharPtr(cookie));
}

void CRIMManager_Qt::getSpecifiedGrpsInfoFromSDK(const QStringList &gidList, const QString &cookie)
{
    CRIMMgrLogDebug("getSpecifiedGrpsInfo..");
	m_imsdk->getSpecifiedGrpsInfo(QVariantToJsonPtr(QVariant(gidList)), qstrToCharPtr(cookie));
}

void CRIMManager_Qt::changeGrpMute(const QString &gid, bool mute, const QString &cookie)
{
    CRIMMgrLogDebug("changeGrpMute...gid:%s, mute:%d", gid.toUtf8().constData(), mute);
    m_imsdk->changeGrpMute(qstrToCharPtr(gid), mute, qstrToCharPtr(cookie));
}

void CRIMManager_Qt::changeGrpMemberMute(const QString &gid, const QString &uid, qint64 seconds, const QString &cookie)
{
    CRIMMgrLogDebug("changeGrpMemberMute...gid:%s, uid:%s, seconds:%lld", gid.toUtf8().constData(), uid.toUtf8().constData(), seconds);
    m_imsdk->changeGrpMemberMute(qstrToCharPtr(gid), qstrToCharPtr(uid), (int)seconds, qstrToCharPtr(cookie));
}

void CRIMManager_Qt::setGrpMemberRoleLevel(const QString &groupID, const QString &userID, CRIMGroupMemberRole roleLevel, const QString &cookie)
{
    CRIMMgrLogDebug("setGrpMemberRoleLevel...gid:%s, uid:%s, roleLevel:%d", groupID.toUtf8().constData(), userID.toUtf8().constData(), roleLevel);
    m_imsdk->setGrpMemberRoleLevel(qstrToCharPtr(groupID), qstrToCharPtr(userID), roleLevel, qstrToCharPtr(cookie));
}

void CRIMManager_Qt::setGrpLookMemberInfo(const QString &groupID, int rule, const QString &cookie)
{
    CRIMMgrLogDebug("setGrpLookMemberInfo...gid:%s, rule:%d", groupID.toUtf8().constData(), rule);
    m_imsdk->setGrpLookMemberInfo(qstrToCharPtr(groupID), rule, qstrToCharPtr(cookie));
}

void CRIMManager_Qt::getGrpMemberListFromSDK(const QString &groupId, CRIMGroupMemberFilter filter, int offset, int count, const QVariantMap &cookie)
{
    auto findIt = m_UiGrpInfos.find(groupId);
    if (findIt != m_UiGrpInfos.end())
    {
        CRIMGrpInfo_Ex &info = findIt.value();
        //如果在获取中或者获取异常，则此次不获取
		if (info._bMembersGetState == 1 || info._bMembersGetState == -1)
        {
            return;
        }
        info._bMembersGetState = 1;
    }
	QVariantMap newCookie = cookie;
	newCookie["getCount"] = count;

    CRIMMgrLogDebug("getGrpMemberList...gid:%s, filter:%d, offset:%d, count:%d", groupId.toUtf8().constData(), filter, offset, count);
	m_imsdk->getGrpMemberList(qstrToCharPtr(groupId), filter, offset, count, QVariantToJsonPtr(newCookie));
}

void CRIMManager_Qt::setGrpMemberInfo(const CRIMGrpMemberInfo &info, const QString &cookie)
{
    QByteArray jsonInfo = CRIM::toJson(info);
    CRIMMgrLogDebug("setGrpMemberInfo...gid:%s", info.groupID.toUtf8().constData());
    m_imsdk->setGrpMemberInfo(jsonInfo.constData(), qstrToCharPtr(cookie));
}

void CRIMManager_Qt::getGrpMembersInfoFromSDK(const QString &groupId, const QStringList &uidList, const QString &cookie)
{
    CRIMMgrLogDebug("getGrpMembersInfo...gid:%s", groupId.toUtf8().constData());
    m_imsdk->getSpecifiedGrpMembersInfo(qstrToCharPtr(groupId), QVariantToJsonPtr(QVariant(uidList)), qstrToCharPtr(cookie));
}

void CRIMManager_Qt::setGrpMemberNickname(const QString &gid, const QString &uid, const QString &groupNickname, const QString &cookie)
{
    CRIMMgrLogDebug("setGrpMemberNickname...gid:%s, uid:%s, groupNickname:%s", gid.toUtf8().constData(), uid.toUtf8().constData(), groupNickname.toUtf8().constData());
    m_imsdk->setGrpMemberNickname(qstrToCharPtr(gid), qstrToCharPtr(uid), qstrToCharPtr(groupNickname), qstrToCharPtr(cookie));
}

void CRIMManager_Qt::getGrpMemberOwnerAndAdmin(const QString &gid, const QString &cookie)
{
    CRIMMgrLogDebug("getGrpMemberOwnerAndAdmin...gid:%s", gid.toUtf8().constData());
    m_imsdk->getGrpMemberOwnerAndAdmin(qstrToCharPtr(gid), qstrToCharPtr(cookie));
}

void CRIMManager_Qt::setGrpVerification(const QString &groupID, CRIMGroupVerificationType needVerification, const QString &cookie)
{
    CRIMMgrLogDebug("setGrpVerification...gid:%s, needVerification:%d", groupID.toUtf8().constData(), needVerification);
    m_imsdk->setGrpVerification(qstrToCharPtr(groupID), needVerification, qstrToCharPtr(cookie));
}

void CRIMManager_Qt::inviteUserToGrp(const QString &groupId, const QStringList &uidList, const QString &reason, const QString &cookie)
{
    CRIMMgrLogDebug("inviteUserToGrp...gid:%s, reason:%s", groupId.toUtf8().constData(), reason.toUtf8().constData());
	m_imsdk->inviteUserToGrp(qstrToCharPtr(groupId), QVariantToJsonPtr(QVariant(uidList)), qstrToCharPtr(reason), qstrToCharPtr(cookie));
}

void CRIMManager_Qt::kickGrpMember(const QString &groupId, const QStringList &uidList, const QString &reason, const QString &cookie)
{
    CRIMMgrLogDebug("kickGrpMember...gid:%s, reason:%s", groupId.toUtf8().constData(), reason.toUtf8().constData());
	m_imsdk->kickGrpMember(qstrToCharPtr(groupId), QVariantToJsonPtr(QVariant(uidList)), qstrToCharPtr(reason), qstrToCharPtr(cookie));
}

void CRIMManager_Qt::transferGrpOwner(const QString &gid, const QString &uid, const QString &cookie)
{
    CRIMMgrLogDebug("transferGrpOwner...gid:%s, uid:%s", gid.toUtf8().constData(), uid.toUtf8().constData());
    m_imsdk->transferGrpOwner(qstrToCharPtr(gid), qstrToCharPtr(uid), qstrToCharPtr(cookie));
}

void CRIMManager_Qt::getGrpReqListAsRecipient(QStringList groupIDs, int offset, int count, const QString &cookie)
{
	CRIMGetGroupReqListAsRecipientReq req;
	req.groupIDs = groupIDs;
	req.offset = offset;
	req.count = count;
	QByteArray jsonReq = CRIM::toJson<CRIMGetGroupReqListAsRecipientReq>(req);

    CRIMMgrLogDebug("getGrpReqListAsRecipient...");
	m_imsdk->getGrpReqListAsRecipient(jsonReq.constData(), qstrToCharPtr(cookie));
}

void CRIMManager_Qt::getGrpReqListAsApplicant(QStringList groupIDs, int offset, int count, const QString &cookie)
{
	CRIMGetGroupReqListAsApplicantReq req;
	req.groupIDs = groupIDs;
	req.offset = offset;
	req.count = count;
	QByteArray jsonReq = CRIM::toJson<CRIMGetGroupReqListAsApplicantReq>(req);
    CRIMMgrLogDebug("getGrpReqListAsApplicant...");
	m_imsdk->getGrpReqListAsApplicant(jsonReq.constData(), qstrToCharPtr(cookie));
}

void CRIMManager_Qt::acceptGrpReq(const QString &gid, const QString &uid, const QString &handleMsg, const QString &cookie)
{
    CRIMMgrLogDebug("acceptGrpReq...gid:%s, uid:%s, handleMsg:%s", gid.toUtf8().constData(), uid.toUtf8().constData(), handleMsg.toUtf8().constData());
    m_imsdk->acceptGrpReq(qstrToCharPtr(gid), qstrToCharPtr(uid), qstrToCharPtr(handleMsg), qstrToCharPtr(cookie));
}

void CRIMManager_Qt::refuseGrpReq(const QString &gid, const QString &uid, const QString &handleMsg, const QString &cookie)
{
    CRIMMgrLogDebug("refuseGrpReq...gid:%s, uid:%s, handleMsg:%s", gid.toUtf8().constData(), uid.toUtf8().constData(), handleMsg.toUtf8().constData());
    m_imsdk->refuseGrpReq(qstrToCharPtr(gid), qstrToCharPtr(uid), qstrToCharPtr(handleMsg), qstrToCharPtr(cookie));
}

void CRIMManager_Qt::isJoinGrp(const QString &gid, const QString &cookie)
{
    CRIMMgrLogDebug("isJoinGrp...gid:%s", gid.toUtf8().constData());
    m_imsdk->isJoinGrp(qstrToCharPtr(gid), qstrToCharPtr(cookie));
}

void CRIMManager_Qt::searchGrpMembers(const QString &groupID, const QStringList &keywordList, bool isSearchUserID,
                                      bool isSearchMemberNickname, int offset, int count, const QString &cookie)
{
    QVariantMap varMap;
    varMap["groupID"] = groupID;
    varMap["keywordList"] = keywordList;
    varMap["isSearchUserID"] = isSearchUserID;
    varMap["isSearchMemberNickname"] = isSearchMemberNickname;
    varMap["offset"] = offset;
    varMap["count"] = count;
    CRIMMgrLogDebug("searchGrpMembers...gid:%s", groupID.toUtf8().constData());

    m_imsdk->searchGrpMembers(QVariantToJsonPtr(varMap), qstrToCharPtr(cookie));
}

CRIMMessage CRIMManager_Qt::createInviteMsg(const CRIMInvitationInfo &info, CRIM_CustomMsgType customType)
{
	QVariantMap dataMap;
	CRIM::Struct_Conv(info, dataMap);

	QVariantMap customMap;
	customMap["customType"] = customType;
	customMap["data"] = dataMap;
	QByteArray jsonStr = VarMapToJson(customMap);
	CRIMMessage msg = createCustomMsg(jsonStr, "", "");

	msg.sessionType = info.groupID.isEmpty() ? CRIM_Conversation_1v1 : CRIM_Conversation_Group;
	msg.sendID = m_userID;
	msg.groupID = info.groupID;

	//非群消息，填写合适的接收者
	if (msg.groupID.isEmpty())
	{
		//我为邀请者
		if (info.inviterUserID == m_userID)
		{
			//从接收都列表中取第1个接收者
			msg.recvID = info.inviteeUserIDList.value(0);
		}
		else  //我回复邀请者，接收人为邀请者
		{
			msg.recvID = info.inviterUserID;
		}
	}
	return msg;
}

void CRIMManager_Qt::getTargetInfo(const CRIMInvitationInfo &info, QString &dstGrpID, QString &dstUsrID)
{
	dstGrpID.clear();
	dstUsrID.clear();
	if (info.groupID.length()>0)
	{
		dstGrpID = info.groupID;
	}
	else
	{
		if (info.inviteeUserIDList.size() > 0)
		{
			dstUsrID = info.inviteeUserIDList.at(0);
		}
	}
}

CRIMOfflinePushInfo CRIMManager_Qt::makeOfflinePushInfo(const CRIMInvitationInfo &info, CRIM_CustomMsgType signalingType)
{
	CRIMOfflinePushInfo offInfo;

	//群消息用群名
	if (info.groupID.length()>0)
	{
		auto pGrp = getJoinedGrpInfo(info.groupID);
		if (pGrp != nullptr)
		{
			offInfo.title = pGrp->groupName;
		}
	}
	//群无效，或私聊，用我的昵称
	if ( offInfo.title.isEmpty() )
	{
		offInfo.title = m_selfInfo.nickname;
	}

	switch (signalingType)
	{
	case CRCUSTOMMSG_NewInvitation:
		offInfo.desc = tr("%1邀请你进行通话").arg(m_selfInfo.nickname);
		break;
	case CRCUSTOMMSG_InviteeAccept:
		offInfo.desc = tr("%1接受了通话邀请").arg(m_selfInfo.nickname);
		break;
	case CRCUSTOMMSG_InviteeReject:
		offInfo.desc = tr("%1拒绝了通话邀请").arg(m_selfInfo.nickname);
		break;
	case CRCUSTOMMSG_InvitationCancel:
		offInfo.desc = tr("%1取消了通话邀请").arg(m_selfInfo.nickname);
		break;
	case CRCUSTOMMSG_InvitationHangUp:
		offInfo.desc = tr("%1挂断了通话").arg(m_selfInfo.nickname);
		break;
	default:
		break;
	}

	return offInfo;
}


void CRIMManager_Qt::call(const CRIMInvitationInfo &info, const QByteArray &cookie)
{
	CRIMMgrLogDebug("call...invitationMsgId:%s", info.invitationMsgId.toUtf8().constData());
	CRIMMessage message = createInviteMsg(info, CRCUSTOMMSG_NewInvitation);
	CRIMOfflinePushInfo offInfo = makeOfflinePushInfo(info, CRCUSTOMMSG_NewInvitation);
	sendMsg(message, message.recvID, message.groupID, offInfo, cookie);
}

void CRIMManager_Qt::acceptCall(const CRIMInvitationInfo &info, const QByteArray &cookie)
{
	//群呼消息，不需要回复
	if (info.groupID.length()>0)
		return;

	CRIMMgrLogDebug("acceptCall...invitationMsgId:%s", info.invitationMsgId.toUtf8().constData());
	CRIMMessage message = createInviteMsg(info, CRCUSTOMMSG_InviteeAccept);
	CRIMOfflinePushInfo offInfo = makeOfflinePushInfo(info, CRCUSTOMMSG_InviteeAccept);
	sendMsg(message, message.recvID, message.groupID, offInfo, cookie);
}

void CRIMManager_Qt::rejectCall(const CRIMInvitationInfo &info, const QByteArray &cookie)
{
	//群呼消息，不需要回复
	if (info.groupID.length()>0)
		return;

	CRIMMgrLogDebug("rejectCall...invitationMsgId:%s", info.invitationMsgId.toUtf8().constData());
	CRIMMessage message = createInviteMsg(info, CRCUSTOMMSG_InviteeReject);
	CRIMOfflinePushInfo offInfo = makeOfflinePushInfo(info, CRCUSTOMMSG_InviteeReject);
	sendMsg(message, message.recvID, message.groupID, offInfo, cookie);
}

void CRIMManager_Qt::cancelCall(const CRIMInvitationInfo &info, const QByteArray &cookie)
{
	CRIMMgrLogDebug("cancelCall...invitationMsgId:%s", info.invitationMsgId.toUtf8().constData());
	CRIMMessage message = createInviteMsg(info, CRCUSTOMMSG_InvitationCancel);
	CRIMOfflinePushInfo offInfo = makeOfflinePushInfo(info, CRCUSTOMMSG_InvitationCancel);
	sendMsg(message, message.recvID, message.groupID, offInfo, cookie);
}

void CRIMManager_Qt::hangUpCall(const CRIMInvitationInfo &info, const QByteArray &cookie)
{
	CRIMMgrLogDebug("hangUpCall...invitationMsgId:%s", info.invitationMsgId.toUtf8().constData());
	CRIMMessage message = createInviteMsg(info, CRCUSTOMMSG_InvitationHangUp);
	CRIMOfflinePushInfo offInfo = makeOfflinePushInfo(info, CRCUSTOMMSG_InvitationHangUp);
	sendMsg(message, message.recvID, message.groupID, offInfo, cookie);
}
//! [imsdk qt封装接口]

////////////////////////////////////////////////////////////////////////

//! [自定义封装接口]
//! [自定义封装接口 - 主调]
void CRIMManager_Qt::refreshGrpsInfoFromSDK(const QStringList &gidList, bool bOnlyQuitted)
{
    QStringList newGidList;
    for (auto &gid : gidList)
    {
        if (m_UiGrpInfos.contains(gid) && bOnlyQuitted)
            continue;

        newGidList.append(gid);
    }

    if (newGidList.size() <= 0)
    {
        return;
    }
    QVariantMap cks;
    cks["session"] = m_curLoginSession;
    cks["onlyQuitted"] = bOnlyQuitted;
    getSpecifiedGrpsInfoFromSDK(newGidList, QVariantToJsonPtr(cks));
}

const QMap<QString, CRIMGrpInfo_Ex>& CRIMManager_Qt::getJoinedGrpList()
{
    return m_UiGrpInfos;
}

const CRIMGrpInfo_Ex* CRIMManager_Qt::getJoinedGrpInfo(const QString &groupId)
{
    auto gPos = m_UiGrpInfos.find(groupId);
    if (gPos == m_UiGrpInfos.end())
        return NULL;
    return &(gPos.value());
}

const CRIMGrpInfo* CRIMManager_Qt::getQuittedGrpInfo(const QString &groupId)
{
    auto gPos = m_quittedInfos.find(groupId);
    if (gPos == m_quittedInfos.end())
        return NULL;
    return &(gPos.value());
}

const QMap<QString, CRIMGrpMemberInfo>& CRIMManager_Qt::getGrpMembersInfo(const QString &groupId)
{
    auto gPos = m_UiGrpInfos.find(groupId);
    if (gPos == m_UiGrpInfos.end())
    {
        static QMap<QString, CRIMGrpMemberInfo> nul;
        return nul;
    }
    return gPos->_members;
}

const CRIMGrpMemberInfo* CRIMManager_Qt::getGrpMemberInfo(const QString &groupId, const QString &userId)
{
    auto gPos = m_UiGrpInfos.find(groupId);
    if (gPos == m_UiGrpInfos.end())
        return NULL;
    auto mPos = gPos->_members.find(userId);
    if (mPos == gPos->_members.end())
        return NULL;
    return &(mPos.value());
}

QString CRIMManager_Qt::getGrpMemberDisplayName(const QString &groupId, const QString &userId)
{
	//群内人员，如果是好友，优先使用好友备注名
	auto pFriendInfo = getFriendInfo(userId);
	if (pFriendInfo != nullptr)
	{
		return pFriendInfo->getDisplayName();
	}

	auto pGrpMbInfo = getGrpMemberInfo(groupId, userId);
	if (pGrpMbInfo != nullptr)
	{
		return pGrpMbInfo->getDisplayName();
	}

	return QString();
}


const CRIMGrpMemberInfo* CRIMManager_Qt::getGrpMemberInfoFromAll(const QString &userId)
{
    for (auto it = m_UiGrpInfos.begin(); it != m_UiGrpInfos.end(); it++)
    {
        auto mPos = it->_members.find(userId);
        if (mPos != it->_members.end())
        {
            return &(mPos.value());
        }
    }
    return NULL;
}

QList<CRIMGrpInfo_Ex*> CRIMManager_Qt::searchGrps(const QString &keyword)
{
    QList<CRIMGrpInfo_Ex*> rslt;
    for (auto &it : m_UiGrpInfos)
    {
        if (it.groupName.contains(keyword, Qt::CaseInsensitive))
        {
            rslt.push_back(&it);
        }
    }
    return rslt;
}

const QHash<QString, CRIMFriendInfo>& CRIMManager_Qt::getFriendList()
{
    return m_friendInfos;
}

const CRIMFriendInfo* CRIMManager_Qt::getFriendInfo(const QString &userID)
{
    auto fItem = m_friendInfos.find(userID);
    if (fItem == m_friendInfos.end())
        return NULL;
    return &(fItem.value());
}

//! [自定义封装接口 - 主调]

//! [自定义封装接口 - 回调处理]
//同步数据到本地缓存
void CRIMManager_Qt::syncDataToLocal()
{
	if (m_cacheDataState == LOGIN_CACHEING)
		return;
	m_cacheDataState = LOGIN_CACHEING;

	CRIMMgrLogDebug("start sync all data to local...");
	m_UiGrpInfos.clear();
	m_bUiGrpInfosGeted = false;
	m_bFriendInfoGeted = false;

	CRIMMgrLogDebug("start get my self info...");
	getSelfInfoFromSDK(m_curLoginSession);

	CRIMMgrLogDebug("start get my grps...");
	getJoinedGrpListFromSDK(m_curLoginSession);

	CRIMMgrLogDebug("start get my friends...");
	getFriendListFromSDK(false, m_curLoginSession);

	m_checkGrpTimer.start(1000);
}

void CRIMManager_Qt::slot_loginSuccess(const QString &data, const QString &cookie)
{
	m_loginState = CRIMMGR_LOGINSYNCDAT;
	syncDataToLocal();
}

void CRIMManager_Qt::slot_loginFailed(int code, const QString &error, const QString &cookie)
{
	CRIMMgrLogDebug("CRIMManager LoginFailed!"); 
	m_loginState = CRIMMGR_LOGOUT;
    m_userID.clear();
    m_curLoginSession.clear();
	QString usrCookie(m_usrLoginCookie);
    m_usrLoginCookie.clear();

	m_imsdk->logout();
	emit s_loginFailed(code, error, usrCookie);
}

//同步完成，开始获取数据缓存到本地
void CRIMManager_Qt::slot_syncServerFinish()
{
	CRIMMgrLogDebug("CRIMManager im data sync finished!");
	m_bSyncServerFinish = true;

	//已经登录成功的情况下，需要重新同步数据到缓存
	if (m_loginState == CRIMMGR_LOGINSUCCESS)
	{
		syncDataToLocal();
	}
}

void CRIMManager_Qt::slot_getSelfInfoSuccess(const CRIMUserInfo &info, const QString &cookie)
{
	if (cookie != m_curLoginSession)
		return;

	CRIMMgrLogDebug("CRIMManager getSelfInfoSuccess!");
	m_selfInfo = info;

	emitSyncFinishedIfAllDataReady();
}

void CRIMManager_Qt::slot_getSelfInfoFailed(int code, const QString &error, const QString &cookie)
{
	if (cookie != m_curLoginSession)
		return;
	CRIMMgrLogWarn("CRIMManager slot_getSelfInfoFailed!");
	slot_loginFailed(code, error, cookie);
}


void CRIMManager_Qt::slot_connectFailed(int errCode)
{
    if (errCode == CRIM_TokenKicked)
    {
        //由cb_OnKickedOffline回调外层
        return;
    }

	if (m_loginState == CRIMMGR_LOGOUT)
    {
        return;
    }

    CRIMMgrLogWarn("connectFailed...");

    logout();
    emit s_connectFailed(errCode);
}

void CRIMManager_Qt::slot_kickedOffline(int errCode)
{
	if (m_loginState == CRIMMGR_LOGOUT || m_loginState == CRIMMGR_LOGINING)
    {
        return;
    }

    CRIMMgrLogWarn("OnKickedOffline...(err:%d)", errCode);
    logout();
    emit s_kickedOffline();
}

void CRIMManager_Qt::slot_getAllConversationListSuccess(const QList<CRIMConversationInfo> &conversationInfos, const QString &cookie)
{
	//批量获取群信息(已退出、已解散)，用于获取群状态最新信息
	QStringList groupIds;
	for (auto &info : conversationInfos)
	{
		if (info.conversationType == CRIM_Conversation_Group)
		{
			groupIds << info.groupID;
		}
	}
	
	refreshGrpsInfoFromSDK(groupIds, true);
	emit s_getAllConversationListSuccess(conversationInfos, cookie);
}

void CRIMManager_Qt::slot_getAllConversationListFailed(int code, const QString &error, const QString &cookie)
{
	emit s_getAllConversationListFailed(code, error, cookie);
}

void CRIMManager_Qt::slot_newConversation(const QList<CRIMConversationInfo> &list)
{
    for (auto &info : list)
    {
        //群
        if (info.conversationType == CRIM_Conversation_Group)
        {
            //如果当前已经有群缓存，则需要重新获取一遍成员（可能存在群信息已经加载好，但是群成员数据为空，原因是数据库正在初始化加载）
            if (m_UiGrpInfos.contains(info.groupID))
            {
                QVariantMap cks;
                cks["session"] = m_curLoginSession;
                cks["grpID"] = info.groupID;
                getGrpMemberListFromSDK(info.groupID, CRIM_GMF_All, 0, 65535, cks);
            }
        }
    }
    emit s_newConversation(list);
}

void CRIMManager_Qt::slot_getJoinedGrpListSuccess(const QList<CRIMGrpInfo> &data, const QString &cookie)
{
    if (cookie != m_curLoginSession)
        return;

    m_UiGrpInfos.clear();
    for (auto &info : data)
    {
        addGrp(info);
    }

    m_bUiGrpInfosGeted = true;
	emitSyncFinishedIfAllDataReady();
}

void CRIMManager_Qt::slot_getJoinedGrpListFailed(int code, const QString &error, const QString &cookie)
{
    if (cookie != m_curLoginSession)
		return;

	CRIMMgrLogWarn("CRIMManager slot_getJoinedGrpListFailed!");
	slot_loginFailed(code, error, cookie);
}

void CRIMManager_Qt::slot_getGrpMemberListSuccess(const QList<CRIMGrpMemberInfo> &data, const QString &cookie)
{
    if (m_curLoginSession.isEmpty())
        return;
    QVariantMap cks = JsonToQVariantMap(cookie.toUtf8());
    if (cks.value("session").toByteArray() != m_curLoginSession)
        return;

    QString groupID = cks.value("grpID").toString();
    auto grpItem = m_UiGrpInfos.find(groupID);
    if (grpItem == m_UiGrpInfos.end())
        return;

    //更新群组成员
    QMap<QString, CRIMGrpMemberInfo> newMembers;
    for (auto &mItem : data)
    {
        newMembers[mItem.userID] = mItem;
    }
    //统计删除
    QList<CRIMGrpMemberInfo> needDelMembers;
    for (auto it = grpItem->_members.begin(); it != grpItem->_members.end(); it++)
    {
        if (newMembers.contains(it.key()))
        {
            continue;
        }
        needDelMembers.append(it.value());
    }
    //统计新增、更新项
    QList<CRIMGrpMemberInfo> needAddMembers;
    QList<CRIMGrpMemberInfo> needUpdateMembers;
    for (auto it = newMembers.begin(); it != newMembers.end(); it++)
    {
        if (grpItem->_members.contains(it.key()))
        {
            needUpdateMembers.append(it.value());
            continue;
        }
        needAddMembers.append(it.value());
    }
    //更新数据
    grpItem->_members.clear();
    grpItem->_members.swap(newMembers);
    grpItem->updateFirst4Members();

	int getCount = cks.value("getCount").toInt();
	//如果实际获取的数据小于预计获取数据，并且本地缓存的成员数不是群成员数，则代表数据获取异常
	bool bGetErrData = data.size() < getCount && grpItem->_members.count() != grpItem->memberCount;
	grpItem->_bMembersGetState = bGetErrData ? -1 : 2;

    CRIMMgrLogDebug("getGrpMemberListSuccess, gid:%s(%s), memberCount:%d, add:%d, del:%d, update:%d", groupID.toUtf8().constData(), grpItem->groupName.toUtf8().constData(), grpItem->_members.size(), needAddMembers.size(), needDelMembers.size(), needUpdateMembers.size());
    CRIMMgrLogDebug("Grp memberCount(%d), memberListCount(%d)", grpItem->memberCount, grpItem->_members.size());

    //检查数据是否同步完成
	if (m_cacheDataState == LOGIN_CACHEING)
    {
        emitSyncFinishedIfAllDataReady();
        return;
    }

    if (cks.value("joinedGrpAdded").toInt() == 1)
    {
        emit s_joinedGrpAdded(grpItem.value());
    }
    //通知界面成员新增、删除、更新项
    for (auto &info : needAddMembers)
    {
        emit s_grpMemberAdded(info);
    }
    for (auto &info : needDelMembers)
    {
        emit s_grpMemberDeleted(info);
    }
    for (auto &info : needUpdateMembers)
    {
        emit s_grpMemberInfoChanged(info);
    }
}

void CRIMManager_Qt::slot_getGrpMemberListFailed(int code, const QString &error, const QString &cookie)
{
    QList<CRIMGrpMemberInfo> nul;
    slot_getGrpMemberListSuccess(nul, cookie);
}

void CRIMManager_Qt::slot_getSpecifiedGrpsInfoSuccess(const QList<CRIMGrpInfo> &infos, const QString &cookie)
{
    QVariantMap cks = JsonToQVariantMap(cookie.toUtf8());
    if (cks["session"].toByteArray() != m_curLoginSession)
        return;

	//已退出的群信息
    if (cks["onlyQuitted"].toBool())
    {
        for (auto &gInfo : infos)
        {
            m_quittedInfos[gInfo.groupID] = gInfo;
            emit s_grpInfoChanged(m_quittedInfos[gInfo.groupID]);
        }
    }
}

void CRIMManager_Qt::slot_getSpecifiedGrpsInfoFailed(int code, const QString &error, const QString &cookie)
{
}

void CRIMManager_Qt::slot_joinedGrpAdded(const CRIMGrpInfo &info)
{
    QVariantMap cookie;
    cookie["joinedGrpAdded"] = 1;
    addGrp(info, cookie);
}

void CRIMManager_Qt::slot_joinedGrpDeleted(const CRIMGrpInfo &info)
{
    m_UiGrpInfos.remove(info.groupID);
	//更新退出数据缓存，退出后sdk底层状态是CRIM_GroupStatus_Ok
	CRIMGrpInfo quitinfo = info;
	quitinfo.status = CRIM_GroupStatus_Ok;
	m_quittedInfos[quitinfo.groupID] = quitinfo;

    emit s_joinedGrpDeleted(info);
}

void CRIMManager_Qt::slot_joinedGrpDismissed(const CRIMGrpInfo &info)
{
	m_UiGrpInfos.remove(info.groupID);
	//更新退出数据缓存，解散后sdk底层状态是CRIM_GroupStatus_Dismissed
	CRIMGrpInfo quitinfo = info;
	quitinfo.status = CRIM_GroupStatus_Dismissed;
	m_quittedInfos[quitinfo.groupID] = quitinfo;

    emit s_joinedGrpDismissed(info);
}

void CRIMManager_Qt::slot_grpInfoChanged(const CRIMGrpInfo &info)
{
    CRIMGrpInfo_Ex &mInfo = m_UiGrpInfos[info.groupID];
    CRIMGrpInfo old = mInfo;
    *((CRIMGrpInfo*)&mInfo) = info;
    emit s_grpInfoChanged(mInfo);
    emit s_grpInfoChanged2(old, mInfo);
}

void CRIMManager_Qt::slot_grpMemberInfoChanged(const CRIMGrpMemberInfo &info)
{
    CRIMGrpMemberInfo old;
    const CRIMGrpMemberInfo* pInfo = getGrpMemberInfo(info.groupID, info.userID);
    if (pInfo != NULL)
    {
        old = *pInfo;
    }
    if (old == info)
    {
        return;
    }

    addGrpMember(info);
    emit s_grpMemberInfoChanged(info);
    emit s_grpMemberInfoChanged2(old, info);
}

void CRIMManager_Qt::slot_grpMemberAdded(const CRIMGrpMemberInfo &info)
{
    addGrpMember(info);
    emit s_grpMemberAdded(info);
}

void CRIMManager_Qt::slot_grpMemberDeleted(const CRIMGrpMemberInfo &info)
{
    auto gPos = m_UiGrpInfos.find(info.groupID);
    if (gPos == m_UiGrpInfos.end())
        return;

    CRIMGrpInfo_Ex &grpItem = gPos.value();
    auto mPos = grpItem._members.find(info.userID);
    if (mPos == grpItem._members.end())
        return;

    CRIMGrpMemberInfo &mItem = grpItem._members[info.userID];
    int nRemoved = grpItem._first4Members.removeAll(&mItem);
    grpItem._members.erase(mPos);
    if (nRemoved > 0)
    {
        grpItem.updateFirst4Members();
    }
	if (grpItem.memberCount != grpItem._members.size())
	{
		CRIMMgrLogWarn("Grp memberCount(%d)!= memberListCount(%d)", grpItem.memberCount, grpItem._members.size());
	}
    emit s_grpMemberDeleted(info);
}

void CRIMManager_Qt::addGrp(const CRIMGrpInfo& info, const QVariantMap &cookie)
{
    CRIMGrpInfo_Ex &mInfo = m_UiGrpInfos[info.groupID];
    *((CRIMGrpInfo*)&mInfo) = info;
	//维护群退出数据缓存
	m_quittedInfos.remove(info.groupID);

    QVariantMap cks(cookie);
    cks["session"] = m_curLoginSession;
    cks["grpID"] = mInfo.groupID;
    getGrpMemberListFromSDK(mInfo.groupID, CRIM_GMF_All, 0, 65535, cks);
}

bool CRIMManager_Qt::addGrpMember(const CRIMGrpMemberInfo& info)
{
    auto gPos = m_UiGrpInfos.find(info.groupID);
    if (gPos == m_UiGrpInfos.end())
        return false;

    CRIMGrpInfo_Ex &grpItem = gPos.value();
    CRIMGrpMemberInfo &mItem = grpItem._members[info.userID];
    mItem = info;
    if (grpItem._first4Members.size() < 4)
    {
        if (!grpItem._first4Members.contains(&mItem))
        {
            grpItem._first4Members.push_back(&mItem);
        }
    }
	if (grpItem.memberCount != grpItem._members.size())
	{
		CRIMMgrLogWarn("Grp memberCount(%d)!= memberListCount(%d)", grpItem.memberCount, grpItem._members.size());
	}
    return true;
}

void CRIMManager_Qt::slot_getFriendListSuccess(const QList<CRIMFriendInfo> &infos, const QString &cookie)
{
    if (cookie != m_curLoginSession)
        return;
    for (auto &item : infos)
    {
		m_friendInfos[item.userID] = item;
    }

    m_bFriendInfoGeted = true;
    emitSyncFinishedIfAllDataReady();
}

void CRIMManager_Qt::slot_getFriendListFailed(int code, const QString &error, const QString &cookie)
{
	QList<CRIMFriendInfo> nul;
    slot_getFriendListSuccess(nul, cookie);
}

void CRIMManager_Qt::slot_friendAdded(const CRIMFriendInfo &info)
{
    CRIMFriendInfo &crInfo = m_friendInfos[info.userID];
    *((CRIMFriendInfo*)&crInfo) = info;
    emit s_friendAdded(crInfo);
}

void CRIMManager_Qt::slot_friendDeleted(const CRIMFriendInfo &info)
{
    auto fItem = m_friendInfos.find(info.userID);
    if (fItem == m_friendInfos.end())
        return;

    CRIMFriendInfo crInfo = fItem.value();
    m_friendInfos.erase(fItem);
    emit s_friendDeleted(crInfo);
}

void CRIMManager_Qt::slot_friendInfoChanged(const CRIMFriendInfo &info)
{
    CRIMFriendInfo crInfo;
    const CRIMFriendInfo *pOldInfo = getFriendInfo(info.userID);
    if (pOldInfo != NULL)
    {
        crInfo = *pOldInfo;
    }
    *((CRIMFriendInfo*)&crInfo) = info;
    slot_crFriendInfoChanged(crInfo);
}

void CRIMManager_Qt::slot_crFriendInfoChanged(const CRIMFriendInfo &info)
{
    CRIMFriendInfo &crInfo = m_friendInfos[info.userID];
    if (crInfo == info)
        return;

    CRIMFriendInfo old = crInfo;
    crInfo = info;
    emit s_friendInfoChanged(crInfo);
    emit s_friendInfoChanged2(old, crInfo);
}

void CRIMManager_Qt::emitSyncFinishedIfAllDataReady()
{
	if (m_cacheDataState == CACHE_NULL)
        return;

	//我的信息
	if (m_selfInfo.userID.isEmpty())
		return;

    //群组未获取
    if (!m_bUiGrpInfosGeted)
        return;

    //群组成员未获取
    for (auto &item : m_UiGrpInfos)
    {
        if (item._bMembersGetState == 0)
            return;
    }

    //好友未获取
    if (!m_bFriendInfoGeted)
        return;

	CRIMMgrLogDebug("CRIMManager sync data finished!");
	m_cacheDataState = LOGIN_CACHEED;

	//登录完成后的数据缓存成功
	if (m_loginState == CRIMMGR_LOGINSUCCESS)
	{
		emit s_syncServerFinish();
	}
	else
	{
		m_loginState = CRIMMGR_LOGINSUCCESS;
		emit s_loginSuccess(m_userID, m_usrLoginCookie);

		//如果当前im db已经同步完成，则进行第二次缓存数据(很可能数据有更新）
		if (m_bSyncServerFinish)
		{
			syncDataToLocal();
		}
	}
}

void CRIMManager_Qt::slot_checkGrpTimeout()
{
    //检测已加载过的群成员数量是否正确，如果不正确需要重新获取
    for (auto it = m_UiGrpInfos.begin(); it != m_UiGrpInfos.end(); it++)
    {
        const CRIMGrpInfo_Ex &mInfo = it.value();
        if (mInfo.memberCount != mInfo._members.count() && mInfo._bMembersGetState == 2)
        {
            QVariantMap cks;
            cks["session"] = m_curLoginSession;
            cks["grpID"] = mInfo.groupID;
			getGrpMemberListFromSDK(mInfo.groupID, CRIM_GMF_All, 0, 65535, cks);
        }
    }
}

//! [自定义封装接口]
