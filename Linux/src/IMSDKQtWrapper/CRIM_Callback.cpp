#include "stdafx.h"
#include "CRIM_Callback.h"
#include "CRIM_Manager.h"
#include "CRIM_DataConv.h"

void CRIMCallbacker_Qt::loginSuccess(const char *data, const char *cookie)
{
	CRIMMgrLogDebug("login im success!");
	emit CRIMManager_Qt::instance()->m_loginSuccess(QString::fromUtf8(data), QString::fromUtf8(cookie));
}

void CRIMCallbacker_Qt::loginFailure(int errCode, const char *errMsg, const char *cookie)
{
	CRIMMgrLogWarn("login im failure, errCode:%d, errMsg:%s", errCode, errMsg);
	emit CRIMManager_Qt::instance()->m_loginFailed(errCode, QString::fromUtf8(errMsg), QString::fromUtf8(cookie));
}

void CRIMCallbacker_Qt::logoutSuccess(const char *data, const char *cookie)
{
	CRIMMgrLogDebug("logout im success!");
	emit CRIMManager_Qt::instance()->s_logoutSuccess(QString::fromUtf8(data), QString::fromUtf8(cookie));
}

void CRIMCallbacker_Qt::logoutFailure(int errCode, const char *errMsg, const char *cookie)
{
	CRIMMgrLogWarn("logout im failure...errCode:%d, errMsg:%s", errCode, errMsg);
	emit CRIMManager_Qt::instance()->s_logoutFailed(errCode, QString::fromUtf8(errMsg), QString::fromUtf8(cookie));
}

void CRIMCallbacker_Qt::uploadFileSuccess(const char *data, const char *cookie)
{
	QVariantMap datMap = QJsonDocument::fromJson(data).toVariant().toMap();
	QString url = datMap["url"].toString();

	QVariantMap varMap = QJsonDocument::fromJson(cookie).toVariant().toMap(); 
	QString filepath = varMap["filepath"].toString();
	QString usrCookie = varMap["usrCookie"].toString();

	CRIMMgrLogDebug("uploadFileSuccess...filepath:%s, url:%s", filepath.toUtf8().constData(), url.toUtf8().constData());
	emit CRIMManager_Qt::instance()->s_uploadFileSuccess(filepath, url, usrCookie);
}

void CRIMCallbacker_Qt::uploadFileFailure(int errCode, const char *errMsg, const char *cookie)
{
	QVariantMap varMap = QJsonDocument::fromJson(cookie).toVariant().toMap();
	QString filepath = varMap["filepath"].toString();
	QString usrCookie = varMap["usrCookie"].toString();

	CRIMMgrLogWarn("uploadFileFailure...filepath:%s, errCode:%d, errMsg:%s", filepath.toUtf8().constData(), errCode, errMsg);
	emit CRIMManager_Qt::instance()->s_uploadFileFailed(filepath, errCode, QString::fromUtf8(errMsg), usrCookie);
}

void CRIMCallbacker_Qt::uploadFileProgress(int64_t totalBytes, int64_t currentBytes, int64_t saveBytes, const char *cookie)
{
	QVariantMap varMap = QJsonDocument::fromJson(cookie).toVariant().toMap();
	QString filepath = varMap["filepath"].toString();
	QString usrCookie = varMap["usrCookie"].toString();

	CRIMMgrLogDebug("uploadFileProgress...filepath:%s, saveBytes:%lld, currentBytes:%lld, totalBytes:%lld", filepath.toUtf8().constData(), saveBytes, currentBytes, totalBytes);
	emit CRIMManager_Qt::instance()->s_uploadFileProgress(filepath, saveBytes, currentBytes, totalBytes, usrCookie);
}

void CRIMCallbacker_Qt::sendMsgSuccess(const char *data, const char *cookie)
{
	QVariantMap varMap = QJsonDocument::fromJson(cookie).toVariant().toMap();
	QString usrCookie = varMap["usrCookie"].toString();
	CRIMMessage msg = CRIM::toStruct<CRIMMessage>(data);

	CRIMMgrLogDebug("sendMsgSuccess...sendID:%s, recvID:%s, groupID:%s, clientMsgID:%s", msg.recvID.toUtf8().constData(), msg.recvID.toUtf8().constData(), msg.groupID.toUtf8().constData(), msg.clientMsgID.toUtf8().constData());
	emit CRIMManager_Qt::instance()->s_sendMsgSuccess(msg, usrCookie);
}

void CRIMCallbacker_Qt::sendMsgFailure(int errCode, const char *errMsg, const char *cookie)
{
	QVariantMap varMap = QJsonDocument::fromJson(cookie).toVariant().toMap();
	QString msgID = varMap["msgID"].toString();
	QString usrCookie = varMap["usrCookie"].toString();

	CRIMMgrLogWarn("sendMsgFailure...clientMsgID:%s, errCode:%d, errMsg:%s", msgID.toUtf8().constData(), errCode, errMsg);
	emit CRIMManager_Qt::instance()->s_sendMsgFailed(msgID, errCode, QString::fromUtf8(errMsg), usrCookie);
}

void CRIMCallbacker_Qt::sendMsgProgress(int progress, const char *cookie)
{
	QVariantMap varMap = QJsonDocument::fromJson(cookie).toVariant().toMap();
	QString msgID = varMap["msgID"].toString();
	QString usrCookie = varMap["usrCookie"].toString();

	CRIMMgrLogDebug("sendMsgProgress...clientMsgID:%s, progress:%d", msgID.toUtf8().constData(), progress);
	emit CRIMManager_Qt::instance()->s_sendMsgProgress(msgID, progress, usrCookie);
}

void CRIMCallbacker_Qt::revokeMsgSuccess(const char *data, const char *cookie)
{
	QVariantMap varMap = QJsonDocument::fromJson(cookie).toVariant().toMap();
	QString conversationID = varMap["conversationID"].toString();
	QString msgID = varMap["msgID"].toString();
	QString usrCookie = varMap["usrCookie"].toString();
	CRIMMgrLogDebug("revokeMsgSuccess...clientMsgID:%s", msgID.toUtf8().constData());

	emit CRIMManager_Qt::instance()->s_revokeMsgSuccess(conversationID, msgID, usrCookie);
}

void CRIMCallbacker_Qt::revokeMsgFailure(int errCode, const char *errMsg, const char *cookie)
{
	QVariantMap varMap = QJsonDocument::fromJson(cookie).toVariant().toMap();
	QString msgID = varMap["msgID"].toString();
	QString usrCookie = varMap["usrCookie"].toString();
	CRIMMgrLogWarn("revokeMsgFailure...clientMsgID:%s, errCode:%d, errMsg:%s", msgID.toUtf8().constData(), errCode, errMsg); 

	emit CRIMManager_Qt::instance()->s_revokeMsgFailed(errCode, QString::fromUtf8(errMsg), usrCookie);
}

void CRIMCallbacker_Qt::markMsgAsReadByMsgIDSuccess(const char *data, const char *cookie)
{
	QVariantMap varMap = QJsonDocument::fromJson(cookie).toVariant().toMap();
	QString conversationID = varMap["conversationID"].toString();
	QStringList msgIDs = varMap["msgIds"].toStringList();
	QString usrCookie = varMap["usrCookie"].toString();
	CRIMMgrLogDebug("markMsgAsReadByMsgIDSuccess."); 
	emit CRIMManager_Qt::instance()->s_markMsgAsReadByMsgIDSuccess(conversationID, msgIDs, usrCookie);
}

void CRIMCallbacker_Qt::markMsgAsReadByMsgIDFailure(int errCode, const char *errMsg, const char *cookie)
{
	QVariantMap varMap = QJsonDocument::fromJson(cookie).toVariant().toMap();
	QString usrCookie = varMap["usrCookie"].toString();
	CRIMMgrLogWarn("markMsgAsReadByMsgIDFailure...errCode:%d, errMsg:%s", errCode, errMsg); 
	emit CRIMManager_Qt::instance()->s_markMsgAsReadByMsgIDFailed(errCode, QString::fromUtf8(errMsg), usrCookie);
}

void CRIMCallbacker_Qt::typingStatusUpdateSuccess(const char *data, const char *cookie)
{
	CRIMMgrLogDebug("typingStatusUpdateSuccess."); 
	emit CRIMManager_Qt::instance()->s_typingStatusUpdateSuccess(QString::fromUtf8(data), cookie);
}

void CRIMCallbacker_Qt::typingStatusUpdateFailure(int errCode, const char *errMsg, const char *cookie)
{
	CRIMMgrLogWarn("typingStatusUpdateFailure...errCode:%d, errMsg:%s", errCode, errMsg); 
	emit CRIMManager_Qt::instance()->s_typingStatusUpdateFailed(errCode, QString::fromUtf8(errMsg), cookie);
}

void CRIMCallbacker_Qt::getAdvancedHistoryMsgListSuccess(const char *data, const char *cookie)
{
	CRIMAdvancedMsg adMsg = CRIM::toStruct<CRIMAdvancedMsg>(data);
	QVariantMap cookieMap = QJsonDocument::fromJson(cookie).toVariant().toMap();
	QByteArray usrCookie = cookieMap["usrCookie"].toByteArray();
	QString conversationID = cookieMap["conversationID"].toString();
	CRIMMgrLogDebug("getHistoryMsgListSuccess...msgs:%d, conversationID:%s", adMsg.messageList.size(), conversationID.toUtf8().constData());

	emit CRIMManager_Qt::instance()->m_getHistoryMsgListSuccess(adMsg, usrCookie);
}

void CRIMCallbacker_Qt::getAdvancedHistoryMsgListFailure(int errCode, const char *errMsg, const char *cookie)
{
	QVariantMap cookieMap = QJsonDocument::fromJson(cookie).toVariant().toMap();
	QByteArray usrCookie = cookieMap["usrCookie"].toByteArray();
	QString conversationID = cookieMap["conversationID"].toString();
	CRIMMgrLogWarn("getHistoryMsgListFailure...conversationID:%s, errCode:%d, errMsg:%s", conversationID.toUtf8().constData(), errCode, errMsg);

	emit CRIMManager_Qt::instance()->m_getHistoryMsgListFailed(errCode, QString::fromUtf8(errMsg), usrCookie);
}

void CRIMCallbacker_Qt::getAdvancedHistoryMsgListReverseSuccess(const char *data, const char *cookie)
{
	CRIMAdvancedMsg adMsg = CRIM::toStruct<CRIMAdvancedMsg>(data);
	QVariantMap cookieMap = QJsonDocument::fromJson(cookie).toVariant().toMap();
	QByteArray usrCookie = cookieMap["usrCookie"].toByteArray();
	QString conversationID = cookieMap["conversationID"].toString();
	CRIMMgrLogDebug("getAdvancedHistoryMsgListReverseSuccess...msgs:%d, conversationID:%s", adMsg.messageList.size(), conversationID.toUtf8().constData());

	emit CRIMManager_Qt::instance()->m_getHistoryMsgListReverseSuccess(adMsg, usrCookie);
}

void CRIMCallbacker_Qt::getAdvancedHistoryMsgListReverseFailure(int errCode, const char *errMsg, const char *cookie)
{
	QVariantMap cookieMap = QJsonDocument::fromJson(cookie).toVariant().toMap();
	QByteArray usrCookie = cookieMap["usrCookie"].toByteArray();
	QString conversationID = cookieMap["conversationID"].toString();
	CRIMMgrLogWarn("getAdvancedHistoryMsgListReverseFailure...conversationID:%s, errCode:%d, errMsg:%s", conversationID.toUtf8().constData(), errCode, errMsg);

	emit CRIMManager_Qt::instance()->m_getHistoryMsgListReverseFailed(errCode, QString::fromUtf8(errMsg), usrCookie);
}

void CRIMCallbacker_Qt::deleteMsgSuccess(const char *data, const char *cookie)
{
	CRIMMgrLogDebug("deleteMsgSuccess."); 
	emit CRIMManager_Qt::instance()->s_deleteMsgSuccess(QString::fromUtf8(data), cookie);
}

void CRIMCallbacker_Qt::deleteMsgFailure(int errCode, const char *errMsg, const char *cookie)
{
	CRIMMgrLogWarn("deleteMsgFailure...errCode:%d, errMsg:%s", errCode, errMsg); 
	emit CRIMManager_Qt::instance()->s_deleteMsgFailed(errCode, QString::fromUtf8(errMsg), cookie);
}

void CRIMCallbacker_Qt::deleteMsgFromLocalSuccess(const char *data, const char *cookie)
{
	CRIMMgrLogDebug("deleteMsgFromLocalSuccess."); 
	emit CRIMManager_Qt::instance()->s_deleteMsgFromLocalSuccess(QString::fromUtf8(data), cookie);
}

void CRIMCallbacker_Qt::deleteMsgFromLocalFailure(int errCode, const char *errMsg, const char *cookie)
{
	CRIMMgrLogWarn("deleteMsgFromLocalFailure...errCode:%d, errMsg:%s", errCode, errMsg); 
	emit CRIMManager_Qt::instance()->s_deleteMsgFromLocalFailed(errCode, QString::fromUtf8(errMsg), cookie);
}

void CRIMCallbacker_Qt::deleteAllMsgSuccess(const char *data, const char *cookie)
{
	CRIMMgrLogDebug("deleteAllMsgSuccess."); 
	emit CRIMManager_Qt::instance()->s_deleteAllMsgSuccess(QString::fromUtf8(data), cookie);
}

void CRIMCallbacker_Qt::deleteAllMsgFailure(int errCode, const char *errMsg, const char *cookie)
{
	CRIMMgrLogWarn("deleteAllMsgFailure...errCode:%d, errMsg:%s", errCode, errMsg); 
	emit CRIMManager_Qt::instance()->s_deleteAllMsgFailed(errCode, QString::fromUtf8(errMsg), cookie);
}

void CRIMCallbacker_Qt::deleteAllMsgFromLocalSuccess(const char *data, const char *cookie)
{
	CRIMMgrLogDebug("deleteAllMsgFromLocalSuccess."); 
	emit CRIMManager_Qt::instance()->s_deleteAllMsgFromLocalSuccess(QString::fromUtf8(data), cookie);
}

void CRIMCallbacker_Qt::deleteAllMsgFromLocalFailure(int errCode, const char *errMsg, const char *cookie)
{
	CRIMMgrLogWarn("deleteAllMsgFromLocalFailure...errCode:%d, errMsg:%s", errCode, errMsg); 
	emit CRIMManager_Qt::instance()->s_deleteAllMsgFromLocalFailed(errCode, QString::fromUtf8(errMsg), cookie);
}

void CRIMCallbacker_Qt::insertSingleMsgToLocalSuccess(const char *data, const char *cookie)
{
	CRIMMgrLogDebug("insertSingleMsgToLocalSuccess."); 
	emit CRIMManager_Qt::instance()->s_insertSingleMsgToLocalSuccess(QString::fromUtf8(data), cookie);
}

void CRIMCallbacker_Qt::insertSingleMsgToLocalFailure(int errCode, const char *errMsg, const char *cookie)
{
	CRIMMgrLogWarn("insertSingleMsgToLocalFailure...errCode:%d, errMsg:%s", errCode, errMsg); 
	emit CRIMManager_Qt::instance()->s_insertSingleMsgToLocalFailed(errCode, QString::fromUtf8(errMsg), cookie);
}

void CRIMCallbacker_Qt::insertGrpMsgToLocalSuccess(const char *data, const char *cookie)
{
	CRIMMgrLogDebug("insertGrpMsgToLocalSuccess."); 
	emit CRIMManager_Qt::instance()->s_insertGrpMsgToLocalSuccess(QString::fromUtf8(data), cookie);
}

void CRIMCallbacker_Qt::insertGrpMsgToLocalFailure(int errCode, const char *errMsg, const char *cookie)
{
	CRIMMgrLogWarn("insertGrpMsgToLocalFailure...errCode:%d, errMsg:%s", errCode, errMsg); 
	emit CRIMManager_Qt::instance()->s_insertGrpMsgToLocalFailed(errCode, QString::fromUtf8(errMsg), cookie);
}

void CRIMCallbacker_Qt::findMsgListSuccess(const char *data, const char *cookie)
{
	CRIMMgrLogDebug("findMsgListSuccess."); 
	
	CRIMSearchResultInfo info = CRIM::toStruct<CRIMSearchResultInfo>(data);
	emit CRIMManager_Qt::instance()->s_findMsgListSuccess(info, cookie);
}

void CRIMCallbacker_Qt::findMsgListFailure(int errCode, const char *errMsg, const char *cookie)
{
	CRIMMgrLogWarn("findMsgListFailure...errCode:%d, errMsg:%s", errCode, errMsg); 
	emit CRIMManager_Qt::instance()->s_findMsgListFailed(errCode, QString::fromUtf8(errMsg), cookie);
}

void CRIMCallbacker_Qt::searchLocalMsgsSuccess(const char *data, const char *cookie)
{
	CRIMMgrLogDebug("searchLocalMsgsSuccess."); 
	CRIMSearchResultInfo info = CRIM::toStruct<CRIMSearchResultInfo>(data);
	emit CRIMManager_Qt::instance()->s_searchLocalMsgsSuccess(info, cookie);
}

void CRIMCallbacker_Qt::searchLocalMsgsFailure(int errCode, const char *errMsg, const char *cookie)
{
	CRIMMgrLogWarn("searchLocalMsgsFailure...errCode:%d, errMsg:%s", errCode, errMsg); 
	emit CRIMManager_Qt::instance()->s_searchLocalMsgsFailed(errCode, QString::fromUtf8(errMsg), cookie);
}

void CRIMCallbacker_Qt::getAllConversationListSuccess(const char *data, const char *cookie)
{
    QList<CRIMConversationInfo> infos = CRIM::toStructList<CRIMConversationInfo>(data);
	CRIMMgrLogDebug("getAllConversationListSuccess...count:%d", infos.size());
	emit CRIMManager_Qt::instance()->m_getAllConversationListSuccess(infos, cookie);
}

void CRIMCallbacker_Qt::getAllConversationListFailure(int errCode, const char *errMsg, const char *cookie)
{
	CRIMMgrLogWarn("getAllConversationListFailure...errCode:%d, errMsg:%s", errCode, errMsg); 
	emit CRIMManager_Qt::instance()->m_getAllConversationListFailed(errCode, QString::fromUtf8(errMsg), cookie);
}

void CRIMCallbacker_Qt::getConversationListSplitSuccess(const char *data, const char *cookie)
{
	QList<CRIMConversationInfo> infos = CRIM::toStructList<CRIMConversationInfo>(data);
	CRIMMgrLogDebug("getConversationListSplitSuccess...count:%d", infos.size());
	emit CRIMManager_Qt::instance()->s_getConversationListSplitSuccess(infos, cookie);
}

void CRIMCallbacker_Qt::getConversationListSplitFailure(int errCode, const char *errMsg, const char *cookie)
{
	CRIMMgrLogWarn("getConversationListSplitFailure...errCode:%d, errMsg:%s", errCode, errMsg); 
	emit CRIMManager_Qt::instance()->s_getConversationListSplitFailed(errCode, QString::fromUtf8(errMsg), cookie);
}

void CRIMCallbacker_Qt::getOneConversationSuccess(const char *data, const char *cookie)
{
	CRIMConversationInfo info = CRIM::toStruct<CRIMConversationInfo>(data);
	CRIMMgrLogDebug("getOneConversationSuccess...gid:%s, uid:%s", info.groupID.toUtf8().constData(), info.userID.toUtf8().constData());
	emit CRIMManager_Qt::instance()->s_getOneConversationSuccess(info, cookie);
}

void CRIMCallbacker_Qt::getOneConversationFailure(int errCode, const char *errMsg, const char *cookie)
{
	CRIMMgrLogWarn("getOneConversationFailure...errCode:%d, errMsg:%s", errCode, errMsg); 
	emit CRIMManager_Qt::instance()->s_getOneConversationFailed(errCode, QString::fromUtf8(errMsg), cookie);
}

void CRIMCallbacker_Qt::getMultipleConversationSuccess(const char *data, const char *cookie)
{
	QList<CRIMConversationInfo> infos = CRIM::toStructList<CRIMConversationInfo>(data);
	CRIMMgrLogDebug("getMultipleConversationSuccess...count:%d", infos.size());
	emit CRIMManager_Qt::instance()->s_getMultipleConversationSuccess(infos, cookie);
}

void CRIMCallbacker_Qt::getMultipleConversationFailure(int errCode, const char *errMsg, const char *cookie)
{
	CRIMMgrLogWarn("getMultipleConversationFailure...errCode:%d, errMsg:%s", errCode, errMsg); 
	emit CRIMManager_Qt::instance()->s_getMultipleConversationFailed(errCode, QString::fromUtf8(errMsg), cookie);
}

void CRIMCallbacker_Qt::hideConversationSuccess(const char *data, const char *cookie)
{
	CRIMMgrLogDebug("hideConversationSuccess."); 
	emit CRIMManager_Qt::instance()->s_hideConversationSuccess(QString::fromUtf8(data), cookie);
}

void CRIMCallbacker_Qt::hideConversationFailure(int errCode, const char *errMsg, const char *cookie)
{
	CRIMMgrLogWarn("hideConversationFailure...errCode:%d, errMsg:%s", errCode, errMsg); 
	emit CRIMManager_Qt::instance()->s_hideConversationFailed(errCode, QString::fromUtf8(errMsg), cookie);
}

void CRIMCallbacker_Qt::hideAllConversationsSuccess(const char *data, const char *cookie)
{
	CRIMMgrLogDebug("hideAllConversationsSuccess."); 
	emit CRIMManager_Qt::instance()->s_hideAllConversationsSuccess(QString::fromUtf8(data), cookie);
}

void CRIMCallbacker_Qt::hideAllConversationsFailure(int errCode, const char *errMsg, const char *cookie)
{
	CRIMMgrLogWarn("hideAllConversationsFailure...errCode:%d, errMsg:%s", errCode, errMsg); 
	emit CRIMManager_Qt::instance()->s_hideAllConversationsFailed(errCode, QString::fromUtf8(errMsg), cookie);
}

void CRIMCallbacker_Qt::clearConversationAndDeleteAllMsgSuccess(const char *data, const char *cookie)
{
	QVariantMap cookieMap = QJsonDocument::fromJson(cookie).toVariant().toMap();
	QString usrCookie = cookieMap["usrCookie"].toString();
	QString conversationID = cookieMap["conversationID"].toString();

	CRIMMgrLogDebug("clearConversationAndDeleteAllMsgSuccess...conversationID:%s", conversationID.toUtf8().constData());
	emit CRIMManager_Qt::instance()->s_clearConversationAndDeleteAllMsgSuccess(conversationID, usrCookie);
}

void CRIMCallbacker_Qt::clearConversationAndDeleteAllMsgFailure(int errCode, const char *errMsg, const char *cookie)
{
	CRIMMgrLogWarn("clearConversationAndDeleteAllMsgFailure...errCode:%d, errMsg:%s", errCode, errMsg); 
	QVariantMap cookieMap = QJsonDocument::fromJson(cookie).toVariant().toMap();
	QString usrCookie = cookieMap["usrCookie"].toString();

	emit CRIMManager_Qt::instance()->s_clearConversationAndDeleteAllMsgFailed(errCode, QString::fromUtf8(errMsg), usrCookie);
}

void CRIMCallbacker_Qt::deleteConversationAndDeleteAllMsgSuccess(const char *data, const char *cookie)
{
	CRIMMgrLogDebug("deleteConversationAndDeleteAllMsgSuccess."); 
	QVariantMap cookieMap = QJsonDocument::fromJson(cookie).toVariant().toMap();
	QString usrCookie = cookieMap["usrCookie"].toString();
	QString conversationID = cookieMap["conversationID"].toString();

	emit CRIMManager_Qt::instance()->s_deleteConversationAndDeleteAllMsgSuccess(conversationID, usrCookie);
}

void CRIMCallbacker_Qt::deleteConversationAndDeleteAllMsgFailure(int errCode, const char *errMsg, const char *cookie)
{
	CRIMMgrLogWarn("deleteConversationAndDeleteAllMsgFailure...errCode:%d, errMsg:%s", errCode, errMsg); 
	QVariantMap cookieMap = QJsonDocument::fromJson(cookie).toVariant().toMap();
	QString usrCookie = cookieMap["usrCookie"].toString();

	emit CRIMManager_Qt::instance()->s_deleteConversationAndDeleteAllMsgFailed(errCode, QString::fromUtf8(errMsg), usrCookie);
}

void CRIMCallbacker_Qt::pinConversationSuccess(const char *data, const char *cookie)
{
	CRIMMgrLogDebug("pinConversationSuccess."); 
	emit CRIMManager_Qt::instance()->s_pinConversationSuccess(QString::fromUtf8(data), cookie);
}

void CRIMCallbacker_Qt::pinConversationFailure(int errCode, const char *errMsg, const char *cookie)
{
	CRIMMgrLogWarn("pinConversationFailure...errCode:%d, errMsg:%s", errCode, errMsg); 
	emit CRIMManager_Qt::instance()->s_pinConversationFailed(errCode, QString::fromUtf8(errMsg), cookie);
}

void CRIMCallbacker_Qt::setConversationDraftSuccess(const char *data, const char *cookie)
{
	CRIMMgrLogDebug("setConversationDraftSuccess."); 
	emit CRIMManager_Qt::instance()->s_setConversationDraftSuccess(QString::fromUtf8(data), cookie);
}

void CRIMCallbacker_Qt::setConversationDraftFailure(int errCode, const char *errMsg, const char *cookie)
{
	CRIMMgrLogWarn("setConversationDraftFailure...errCode:%d, errMsg:%s", errCode, errMsg); 
	emit CRIMManager_Qt::instance()->s_setConversationDraftFailed(errCode, QString::fromUtf8(errMsg), cookie);
}

void CRIMCallbacker_Qt::resetConversationGrpAtTypeSuccess(const char *data, const char *cookie)
{
	CRIMMgrLogDebug("resetConversationGrpAtTypeSuccess."); 
	emit CRIMManager_Qt::instance()->s_resetConversationGrpAtTypeSuccess(QString::fromUtf8(data), cookie);
}

void CRIMCallbacker_Qt::resetConversationGrpAtTypeFailure(int errCode, const char *errMsg, const char *cookie)
{
	CRIMMgrLogWarn("resetConversationGrpAtTypeFailure...errCode:%d, errMsg:%s", errCode, errMsg); 
	emit CRIMManager_Qt::instance()->s_resetConversationGrpAtTypeFailed(errCode, QString::fromUtf8(errMsg), cookie);
}

void CRIMCallbacker_Qt::markConversationMsgAsReadSuccess(const char *data, const char *cookie)
{
	CRIMMgrLogDebug("markConversationMsgAsReadSuccess."); 
	emit CRIMManager_Qt::instance()->s_markConversationMsgAsReadSuccess(QString::fromUtf8(data), cookie);
}

void CRIMCallbacker_Qt::markConversationMsgAsReadFailure(int errCode, const char *errMsg, const char *cookie)
{
	CRIMMgrLogWarn("markConversationMsgAsReadFailure...errCode:%d, errMsg:%s", errCode, errMsg); 
	emit CRIMManager_Qt::instance()->s_markConversationMsgAsReadFailed(errCode, QString::fromUtf8(errMsg), cookie);
}

void CRIMCallbacker_Qt::getTotalUnreadMsgCountSuccess(const char *data, const char *cookie)
{
	CRIMMgrLogDebug("getTotalUnreadMsgCountSuccess."); 
	emit CRIMManager_Qt::instance()->s_getTotalUnreadMsgCountSuccess(QString::fromUtf8(data).toInt(), cookie);
}

void CRIMCallbacker_Qt::getTotalUnreadMsgCountFailure(int errCode, const char *errMsg, const char *cookie)
{
	CRIMMgrLogWarn("getTotalUnreadMsgCountFailure...errCode:%d, errMsg:%s", errCode, errMsg); 
	emit CRIMManager_Qt::instance()->s_getTotalUnreadMsgCountFailed(errCode, QString::fromUtf8(errMsg), cookie);
}

void CRIMCallbacker_Qt::setConversationPrivateChatSuccess(const char *data, const char *cookie)
{
	CRIMMgrLogDebug("setConversationPrivateChatSuccess."); 
	emit CRIMManager_Qt::instance()->s_setConversationPrivateChatSuccess(QString::fromUtf8(data), cookie);
}

void CRIMCallbacker_Qt::setConversationPrivateChatFailure(int errCode, const char *errMsg, const char *cookie)
{
	CRIMMgrLogWarn("setConversationPrivateChatFailure...errCode:%d, errMsg:%s", errCode, errMsg); 
	emit CRIMManager_Qt::instance()->s_setConversationPrivateChatFailed(errCode, QString::fromUtf8(errMsg), cookie);
}

void CRIMCallbacker_Qt::setConversationBurnDurationSuccess(const char *data, const char *cookie)
{
	CRIMMgrLogDebug("setConversationBurnDurationSuccess."); 
	emit CRIMManager_Qt::instance()->s_setConversationBurnDurationSuccess(QString::fromUtf8(data), cookie);
}

void CRIMCallbacker_Qt::setConversationBurnDurationFailure(int errCode, const char *errMsg, const char *cookie)
{
	CRIMMgrLogWarn("setConversationBurnDurationFailure...errCode:%d, errMsg:%s", errCode, errMsg); 
	emit CRIMManager_Qt::instance()->s_setConversationBurnDurationFailed(errCode, QString::fromUtf8(errMsg), cookie);
}

void CRIMCallbacker_Qt::setConversationRecvMsgOptSuccess(const char *data, const char *cookie)
{
	CRIMMgrLogDebug("setConversationRecvMsgOptSuccess."); 
	emit CRIMManager_Qt::instance()->s_setConversationRecvMsgOptSuccess(QString::fromUtf8(data), cookie);
}

void CRIMCallbacker_Qt::setConversationRecvMsgOptFailure(int errCode, const char *errMsg, const char *cookie)
{
	CRIMMgrLogWarn("setConversationRecvMsgOptFailure...errCode:%d, errMsg:%s", errCode, errMsg); 
	emit CRIMManager_Qt::instance()->s_setConversationRecvMsgOptFailed(errCode, QString::fromUtf8(errMsg), cookie);
}

void CRIMCallbacker_Qt::setSelfInfoSuccess(const char *data, const char *cookie)
{
	CRIMMgrLogDebug("setSelfInfoSuccess."); 
	emit CRIMManager_Qt::instance()->s_setSelfInfoSuccess(QString::fromUtf8(data), cookie);
}

void CRIMCallbacker_Qt::setSelfInfoFailure(int errCode, const char *errMsg, const char *cookie)
{
	CRIMMgrLogWarn("setSelfInfoFailure...errCode:%d, errMsg:%s", errCode, errMsg); 
	emit CRIMManager_Qt::instance()->s_setSelfInfoFailed(errCode, QString::fromUtf8(errMsg), cookie);
}

void CRIMCallbacker_Qt::getSelfInfoSuccess(const char *data, const char *cookie)
{
	CRIMMgrLogDebug("getSelfInfoSuccess."); 
	CRIMUserInfo info = CRIM::toStruct<CRIMUserInfo>(data);

	emit CRIMManager_Qt::instance()->m_getSelfInfoSuccess(info, cookie);
}

void CRIMCallbacker_Qt::getSelfInfoFailure(int errCode, const char *errMsg, const char *cookie)
{
	CRIMMgrLogWarn("getSelfInfoFailure...errCode:%d, errMsg:%s", errCode, errMsg); 
	emit CRIMManager_Qt::instance()->m_getSelfInfoFailed(errCode, QString::fromUtf8(errMsg), cookie);
}

void CRIMCallbacker_Qt::getUsersInfoSuccess(const char *data, const char *cookie)
{
	QList<CRIMUserInfo> userInfos = CRIM::toStructList<CRIMUserInfo>(data);
	CRIMMgrLogDebug("getUsersInfoSuccess...count:%d", userInfos.size());
	emit CRIMManager_Qt::instance()->s_getUsersInfoSuccess(userInfos, cookie);
}

void CRIMCallbacker_Qt::getUsersInfoFailure(int errCode, const char *errMsg, const char *cookie)
{
	CRIMMgrLogWarn("getUsersInfoFailure...errCode:%d, errMsg:%s", errCode, errMsg);
	emit CRIMManager_Qt::instance()->s_getUsersInfoFailed(errCode, QString::fromUtf8(errMsg), cookie);
}

void CRIMCallbacker_Qt::getUserStatusSuccess(const char *data, const char *cookie)
{
	QList<CRIMUserStatusInfo> infoList = CRIM::toStructList<CRIMUserStatusInfo>(data);
	CRIMMgrLogDebug("getUserStatusSuccess...count:%d", infoList.size());
	emit CRIMManager_Qt::instance()->s_getUserStatusSuccess(infoList, cookie);
}

void CRIMCallbacker_Qt::getUserStatusFailure(int errCode, const char *errMsg, const char *cookie)
{
	CRIMMgrLogWarn("getUserStatusFailure...errCode:%d, errMsg:%s", errCode, errMsg);
	emit CRIMManager_Qt::instance()->s_getUserStatusFailed(errCode, QString::fromUtf8(errMsg), cookie);
}

void CRIMCallbacker_Qt::subscribeUsersStatusSuccess(const char *data, const char *cookie)
{
	QList<CRIMUserStatusInfo> infoList = CRIM::toStructList<CRIMUserStatusInfo>(data);
	CRIMMgrLogDebug("subscribeUsersStatusSuccess...count:%d", infoList.size());
	emit CRIMManager_Qt::instance()->s_subscribeUsersStatusSuccess(infoList, cookie);
}

void CRIMCallbacker_Qt::subscribeUsersStatusFailure(int errCode, const char *errMsg, const char *cookie)
{
	CRIMMgrLogWarn("subscribeUsersStatusFailure...errCode:%d, errMsg:%s", errCode, errMsg); 
	emit CRIMManager_Qt::instance()->s_subscribeUsersStatusFailed(errCode, QString::fromUtf8(errMsg), cookie);
}

void CRIMCallbacker_Qt::unsubscribeUsersStatusSuccess(const char *data, const char *cookie)
{
	CRIMMgrLogDebug("unsubscribeUsersStatusSuccess."); 
	emit CRIMManager_Qt::instance()->s_unsubscribeUsersStatusSuccess(QString::fromUtf8(data), cookie);
}

void CRIMCallbacker_Qt::unsubscribeUsersStatusFailure(int errCode, const char *errMsg, const char *cookie)
{
	CRIMMgrLogWarn("unsubscribeUsersStatusFailure...errCode:%d, errMsg:%s", errCode, errMsg); 
	emit CRIMManager_Qt::instance()->s_unsubscribeUsersStatusFailed(errCode, QString::fromUtf8(errMsg), cookie);
}

void CRIMCallbacker_Qt::getSubscribeUsersStatusSuccess(const char *data, const char *cookie)
{
	QList<CRIMUserStatusInfo> infoList = CRIM::toStructList<CRIMUserStatusInfo>(data);
	CRIMMgrLogDebug("getSubscribeUsersStatusSuccess...count:%d", infoList.size());
	emit CRIMManager_Qt::instance()->s_getSubscribeUsersStatusSuccess(infoList, cookie);
}

void CRIMCallbacker_Qt::getSubscribeUsersStatusFailure(int errCode, const char *errMsg, const char *cookie)
{
	CRIMMgrLogWarn("getSubscribeUsersStatusFailure...errCode:%d, errMsg:%s", errCode, errMsg); 
	emit CRIMManager_Qt::instance()->s_getSubscribeUsersStatusFailed(errCode, QString::fromUtf8(errMsg), cookie);
}

void CRIMCallbacker_Qt::getFriendListSuccess(const char *data, const char *cookie)
{
	QList<CRIMFriendInfo> infoList = CRIM::toStructList<CRIMFriendInfo>(data);
	CRIMMgrLogDebug("getFriendListSuccess...count:%d", infoList.size()); 
	emit CRIMManager_Qt::instance()->m_getFriendListSuccess(infoList, cookie);
}

void CRIMCallbacker_Qt::getFriendListFailure(int errCode, const char *errMsg, const char *cookie)
{
	CRIMMgrLogWarn("getFriendListFailure...errCode:%d, errMsg:%s", errCode, errMsg); 
	emit CRIMManager_Qt::instance()->m_getFriendListFailed(errCode, QString::fromUtf8(errMsg), cookie);
}

void CRIMCallbacker_Qt::getFriendListPageSuccess(const char *data, const char *cookie)
{
// 	QList<CRIMFullUserInfo> infoList = CRIM::toStructList<CRIMFullUserInfo>(data);
// 	CRIMMgrLogDebug("getFriendListPageSuccess...count:%d", infoList.size()); 
// 	emit CRIMManager_Qt::instance()->m_getFriendListPageSuccess(infoList, cookie);
}

void CRIMCallbacker_Qt::getFriendListPageFailure(int errCode, const char *errMsg, const char *cookie)
{
// 	CRIMMgrLogWarn("getFriendListPageFailure...errCode:%d, errMsg:%s", errCode, errMsg); 
// 	emit CRIMManager_Qt::instance()->m_getFriendListPageFailed(errCode, QString::fromUtf8(errMsg), cookie);
}

void CRIMCallbacker_Qt::getSpecifiedFriendsInfoSuccess(const char *data, const char *cookie)
{
// 	QList<FullUserInfo> infoList = CRIM::toStructList<CRIMFullUserInfo>(data);
// 	CRIMMgrLogDebug("getSpecifiedFriendsInfoSuccess...count:%d", infoList.size()); 
// 	emit CRIMManager_Qt::instance()->m_getSpecifiedFriendsInfoSuccess(infoList, cookie);
}

void CRIMCallbacker_Qt::getSpecifiedFriendsInfoFailure(int errCode, const char *errMsg, const char *cookie)
{
// 	CRIMMgrLogWarn("getSpecifiedFriendsInfoFailure...errCode:%d, errMsg:%s", errCode, errMsg); 
// 	emit CRIMManager_Qt::instance()->m_getSpecifiedFriendsInfoFailed(errCode, QString::fromUtf8(errMsg), cookie);
}

void CRIMCallbacker_Qt::addFriendSuccess(const char *data, const char *cookie)
{
	CRIMMgrLogDebug("addFriendSuccess."); 
	emit CRIMManager_Qt::instance()->s_addFriendSuccess(QString::fromUtf8(data), cookie);
}

void CRIMCallbacker_Qt::addFriendFailure(int errCode, const char *errMsg, const char *cookie)
{
	CRIMMgrLogWarn("addFriendFailure...errCode:%d, errMsg:%s", errCode, errMsg); 
	emit CRIMManager_Qt::instance()->s_addFriendFailed(errCode, QString::fromUtf8(errMsg), cookie);
}

void CRIMCallbacker_Qt::deleteFriendSuccess(const char *data, const char *cookie)
{
	CRIMMgrLogDebug("deleteFriendSuccess."); 
	emit CRIMManager_Qt::instance()->s_deleteFriendSuccess(QString::fromUtf8(data), cookie);
}

void CRIMCallbacker_Qt::deleteFriendFailure(int errCode, const char *errMsg, const char *cookie)
{
	CRIMMgrLogWarn("deleteFriendFailure...errCode:%d, errMsg:%s", errCode, errMsg); 
	emit CRIMManager_Qt::instance()->s_deleteFriendFailed(errCode, QString::fromUtf8(errMsg), cookie);
}

void CRIMCallbacker_Qt::checkFriendSuccess(const char *data, const char *cookie)
{
// 	QList<CRIMSimpleResultInfo> infos = CRIM::toStructList<CRIMSimpleResultInfo>(data);
	// 	CRIMMgrLogDebug("checkFriendSuccess...count:%d", infos.size()); 
	// 	emit CRIMManager_Qt::instance()->s_checkFriendSuccess(infos, cookie);
}

void CRIMCallbacker_Qt::checkFriendFailure(int errCode, const char *errMsg, const char *cookie)
{
// 	CRIMMgrLogWarn("checkFriendFailure...errCode:%d, errMsg:%s", errCode, errMsg); 
// 	emit CRIMManager_Qt::instance()->s_checkFriendFailed(errCode, QString::fromUtf8(errMsg), cookie);
}

void CRIMCallbacker_Qt::updateFriendsSuccess(const char *data, const char *cookie)
{
	CRIMMgrLogDebug("updateFriendsSuccess."); 
	emit CRIMManager_Qt::instance()->s_setFriendRemarkSuccess(QString::fromUtf8(data), cookie);
}

void CRIMCallbacker_Qt::updateFriendsFailure(int errCode, const char *errMsg, const char *cookie)
{
	CRIMMgrLogWarn("updateFriendsFailure...errCode:%d, errMsg:%s", errCode, errMsg); 
	emit CRIMManager_Qt::instance()->s_setFriendRemarkFailed(errCode, QString::fromUtf8(errMsg), cookie);
}

void CRIMCallbacker_Qt::searchFriendsSuccess(const char *data, const char *cookie)
{
	// 	CRIMMgrLogDebug("searchFriendsSuccess.");
	// 	QLCRIMManager::instance()IM::toStructList<FriendInfo>(data);
	// 	emit CRIMManager_Qt::instance()->s_searchFriendsSuccess(infos, cookie);
}

void CRIMCallbacker_Qt::searchFriendsFailure(int errCode, const char *errMsg, const char *cookie)
{
	// 	CRIMMgrLogWarn("searchFriendsFailure...CRIMManager::instance()kie:%s", errCode, errMsg, cookie);
	// 	emit CRIMManager_Qt::instance()->s_searchFriendsFailed(errCode, QString::fromUtf8(errMsg), cookie);
}

void CRIMCallbacker_Qt::getFriendReqListAsRecipientSuccess(const char *data, const char *cookie)
{
	QList<CRIMFriendReqInfo> infos = CRIM::toStructList<CRIMFriendReqInfo>(data);
	CRIMMgrLogDebug("getFriendReqListAsRecipientSuccess...count:%d", infos.size());
	emit CRIMManager_Qt::instance()->s_getFriendReqListAsRecipientSuccess(infos, cookie);
}

void CRIMCallbacker_Qt::getFriendReqListAsRecipientFailure(int errCode, const char *errMsg, const char *cookie)
{
	CRIMMgrLogWarn("getFriendReqListAsRecipientFailure...errCode:%d, errMsg:%s", errCode, errMsg); 
	emit CRIMManager_Qt::instance()->s_getFriendReqListAsRecipientFailed(errCode, QString::fromUtf8(errMsg), cookie);
}

void CRIMCallbacker_Qt::getFriendReqListAsApplicantSuccess(const char *data, const char *cookie)
{
	QList<CRIMFriendReqInfo> infos = CRIM::toStructList<CRIMFriendReqInfo>(data);
	CRIMMgrLogDebug("getFriendReqListAsApplicantSuccess...count:%d", infos.size());
	emit CRIMManager_Qt::instance()->s_getFriendReqListAsApplicantSuccess(infos, cookie);
}

void CRIMCallbacker_Qt::getFriendReqListAsApplicantFailure(int errCode, const char *errMsg, const char *cookie)
{
	CRIMMgrLogWarn("getFriendReqListAsApplicantFailure...errCode:%d, errMsg:%s", errCode, errMsg); 
	emit CRIMManager_Qt::instance()->s_getFriendReqListAsApplicantFailed(errCode, QString::fromUtf8(errMsg), cookie);
}

void CRIMCallbacker_Qt::acceptFriendReqSuccess(const char *data, const char *cookie)
{
	CRIMMgrLogDebug("acceptFriendReqSuccess."); 
	emit CRIMManager_Qt::instance()->s_acceptFriendReqSuccess(QString::fromUtf8(data), cookie);
}

void CRIMCallbacker_Qt::acceptFriendReqFailure(int errCode, const char *errMsg, const char *cookie)
{
	CRIMMgrLogWarn("acceptFriendReqFailure...errCode:%d, errMsg:%s", errCode, errMsg); 
	emit CRIMManager_Qt::instance()->s_acceptFriendReqFailed(errCode, QString::fromUtf8(errMsg), cookie);
}

void CRIMCallbacker_Qt::refuseFriendReqSuccess(const char *data, const char *cookie)
{
	CRIMMgrLogDebug("refuseFriendReqSuccess."); 
	emit CRIMManager_Qt::instance()->s_refuseFriendReqSuccess(QString::fromUtf8(data), cookie);
}

void CRIMCallbacker_Qt::refuseFriendReqFailure(int errCode, const char *errMsg, const char *cookie)
{
	CRIMMgrLogWarn("refuseFriendReqFailure...errCode:%d, errMsg:%s", errCode, errMsg); 
	emit CRIMManager_Qt::instance()->s_refuseFriendReqFailed(errCode, QString::fromUtf8(errMsg), cookie);
}

void CRIMCallbacker_Qt::getBlackListSuccess(const char *data, const char *cookie)
{
	QList<CRIMBlackListInfo> infos = CRIM::toStructList<CRIMBlackListInfo>(data);
	CRIMMgrLogDebug("getBlackListSuccess...count:%d", infos.size());
	emit CRIMManager_Qt::instance()->s_getBlackListSuccess(infos, cookie);
}

void CRIMCallbacker_Qt::getBlackListFailure(int errCode, const char *errMsg, const char *cookie)
{
	CRIMMgrLogWarn("getBlackListFailure...errCode:%d, errMsg:%s", errCode, errMsg); 
	emit CRIMManager_Qt::instance()->s_getBlackListFailed(errCode, QString::fromUtf8(errMsg), cookie);
}

void CRIMCallbacker_Qt::addToBlackListSuccess(const char *data, const char *cookie)
{
	CRIMMgrLogDebug("addBlackListSuccess."); 
	emit CRIMManager_Qt::instance()->s_addToBlackListSuccess(QString::fromUtf8(data), cookie);
}

void CRIMCallbacker_Qt::addToBlackListFailure(int errCode, const char *errMsg, const char *cookie)
{
	CRIMMgrLogWarn("addBlackListFailure...errCode:%d, errMsg:%s", errCode, errMsg); 
	emit CRIMManager_Qt::instance()->s_addToBlackListFailed(errCode, QString::fromUtf8(errMsg), cookie);
}

void CRIMCallbacker_Qt::removeFromBlackListSuccess(const char *data, const char *cookie)
{
	CRIMMgrLogDebug("removeBlackListSuccess."); 
	emit CRIMManager_Qt::instance()->s_removeFromBlackListSuccess(QString::fromUtf8(data), cookie);
}

void CRIMCallbacker_Qt::removeFromBlackListFailure(int errCode, const char *errMsg, const char *cookie)
{
	CRIMMgrLogWarn("removeBlackListFailure...errCode:%d, errMsg:%s", errCode, errMsg); 
	emit CRIMManager_Qt::instance()->s_removeFromBlackListFailed(errCode, QString::fromUtf8(errMsg), cookie);
}

void CRIMCallbacker_Qt::createGrpSuccess(const char *data, const char *cookie)
{
	CRIMGrpInfo info = CRIM::toStruct<CRIMGrpInfo>(data);
	CRIMMgrLogDebug("createGrpSuccess...gid:%s, groupName:%s", info.groupID.toUtf8().constData(), info.groupName.toUtf8().constData());
	emit CRIMManager_Qt::instance()->s_createGrpSuccess(info, cookie);
}

void CRIMCallbacker_Qt::createGrpFailure(int errCode, const char *errMsg, const char *cookie)
{
	CRIMMgrLogWarn("createGrpFailure...errCode:%d, errMsg:%s", errCode, errMsg); 
	emit CRIMManager_Qt::instance()->s_createGrpFailed(errCode, QString::fromUtf8(errMsg), cookie);
}

void CRIMCallbacker_Qt::joinGrpSuccess(const char *data, const char *cookie)
{
	CRIMMgrLogDebug("joinGrpSuccess."); 
	emit CRIMManager_Qt::instance()->s_joinGrpSuccess(QString::fromUtf8(data), cookie);
}

void CRIMCallbacker_Qt::joinGrpFailure(int errCode, const char *errMsg, const char *cookie)
{
	CRIMMgrLogWarn("joinGrpFailure...errCode:%d, errMsg:%s", errCode, errMsg); 
	emit CRIMManager_Qt::instance()->s_joinGrpFailed(errCode, QString::fromUtf8(errMsg), cookie);
}

void CRIMCallbacker_Qt::quitGrpSuccess(const char *data, const char *cookie)
{
	CRIMMgrLogDebug("quitGrpSuccess."); 
	emit CRIMManager_Qt::instance()->s_quitGrpSuccess(QString::fromUtf8(data), cookie);
}

void CRIMCallbacker_Qt::quitGrpFailure(int errCode, const char *errMsg, const char *cookie)
{
	CRIMMgrLogWarn("quitGrpFailure...errCode:%d, errMsg:%s", errCode, errMsg); 
	emit CRIMManager_Qt::instance()->s_quitGrpFailed(errCode, QString::fromUtf8(errMsg), cookie);
}

void CRIMCallbacker_Qt::dismissGrpSuccess(const char *data, const char *cookie)
{
	CRIMMgrLogDebug("dismissGrpSuccess."); 
	emit CRIMManager_Qt::instance()->s_dismissGrpSuccess(QString::fromUtf8(data), cookie);
}

void CRIMCallbacker_Qt::dismissGrpFailure(int errCode, const char *errMsg, const char *cookie)
{
	CRIMMgrLogWarn("dismissGrpFailure...errCode:%d, errMsg:%s", errCode, errMsg); 
	emit CRIMManager_Qt::instance()->s_dismissGrpFailed(errCode, QString::fromUtf8(errMsg), cookie);
}

void CRIMCallbacker_Qt::setGrpInfoSuccess(const char *data, const char *cookie)
{
	CRIMMgrLogDebug("setGrpInfoSuccess."); 
	emit CRIMManager_Qt::instance()->s_setGrpInfoSuccess(QString::fromUtf8(data), cookie);
}

void CRIMCallbacker_Qt::setGrpInfoFailure(int errCode, const char *errMsg, const char *cookie)
{
	CRIMMgrLogWarn("setGrpInfoFailure...errCode:%d, errMsg:%s", errCode, errMsg); 
	emit CRIMManager_Qt::instance()->s_setGrpInfoFailed(errCode, QString::fromUtf8(errMsg), cookie);
}

void CRIMCallbacker_Qt::getJoinedGrpListSuccess(const char *data, const char *cookie)
{
	QList<CRIMGrpInfo> infos = CRIM::toStructList<CRIMGrpInfo>(data);
	CRIMMgrLogDebug("getJoinedGrpListSuccess...count:%d", infos.size());
	emit CRIMManager_Qt::instance()->m_getJoinedGrpListSuccess(infos, cookie);
}

void CRIMCallbacker_Qt::getJoinedGrpListFailure(int errCode, const char *errMsg, const char *cookie)
{
	CRIMMgrLogWarn("getJoinedGrpListFailure...errCode:%d, errMsg:%s", errCode, errMsg); 
	emit CRIMManager_Qt::instance()->m_getJoinedGrpListFailed(errCode, QString::fromUtf8(errMsg), cookie);
}

void CRIMCallbacker_Qt::getSpecifiedGrpsInfoSuccess(const char *data, const char *cookie)
{
	QList<CRIMGrpInfo> infos = CRIM::toStructList<CRIMGrpInfo>(data);
	CRIMMgrLogDebug("getSpecifiedGrpsInfoSuccess...count:%d", infos.size());
	emit CRIMManager_Qt::instance()->m_getSpecifiedGrpsInfoSuccess(infos, cookie);
}

void CRIMCallbacker_Qt::getSpecifiedGrpsInfoFailure(int errCode, const char *errMsg, const char *cookie)
{
	CRIMMgrLogWarn("getSpecifiedGrpsInfoFailure...errCode:%d, errMsg:%s", errCode, errMsg); 
	emit CRIMManager_Qt::instance()->m_getSpecifiedGrpsInfoFailed(errCode, QString::fromUtf8(errMsg), cookie);
}

void CRIMCallbacker_Qt::searchGrpsSuccess(const char *data, const char *cookie)
{
	//CRIMMgrLogDebug("searchGrpsSuccess.");
	//emit CRIMManager_Qt::instance()->s_searchGrpsSuccess(QString::fromUtf8(data), cookie);
}

void CRIMCallbacker_Qt::searchGrpsFailure(int errCode, const char *errMsg, const char *cookie)
{
	//CRIMMgrLogWarn("searchGrpsFailure...errCode:%d, errMsg:%s", errCode, errMsg);
	//emit CRIMManager_Qt::instance()->s_searchGrpsFailed(errCode, QString::fromUtf8(errMsg), cookie);
}

void CRIMCallbacker_Qt::changeGrpMuteSuccess(const char *data, const char *cookie)
{
	CRIMMgrLogDebug("changeGrpMuteSuccess."); 
	emit CRIMManager_Qt::instance()->s_changeGrpMuteSuccess(QString::fromUtf8(data), cookie);
}

void CRIMCallbacker_Qt::changeGrpMuteFailure(int errCode, const char *errMsg, const char *cookie)
{
	CRIMMgrLogWarn("changeGrpMuteFailure...errCode:%d, errMsg:%s", errCode, errMsg); 
	emit CRIMManager_Qt::instance()->s_changeGrpMuteFailed(errCode, QString::fromUtf8(errMsg), cookie);
}

void CRIMCallbacker_Qt::changeGrpMemberMuteSuccess(const char *data, const char *cookie)
{
	CRIMMgrLogDebug("changeGrpMemberMuteSuccess."); 
	emit CRIMManager_Qt::instance()->s_changeGrpMemberMuteSuccess(QString::fromUtf8(data), cookie);
}

void CRIMCallbacker_Qt::changeGrpMemberMuteFailure(int errCode, const char *errMsg, const char *cookie)
{
	CRIMMgrLogWarn("changeGrpMemberMuteFailure...errCode:%d, errMsg:%s", errCode, errMsg); 
	emit CRIMManager_Qt::instance()->s_changeGrpMemberMuteFailed(errCode, QString::fromUtf8(errMsg), cookie);
}

void CRIMCallbacker_Qt::setGrpMemberRoleLevelSuccess(const char *data, const char *cookie)
{
	CRIMMgrLogDebug("setGrpMemberRoleLevelSuccess."); 
	emit CRIMManager_Qt::instance()->s_setGrpMemberRoleLevelSuccess(QString::fromUtf8(data), cookie);
}

void CRIMCallbacker_Qt::setGrpMemberRoleLevelFailure(int errCode, const char *errMsg, const char *cookie)
{
	CRIMMgrLogWarn("setGrpMemberRoleLevelFailure...errCode:%d, errMsg:%s", errCode, errMsg); 
	emit CRIMManager_Qt::instance()->s_setGrpMemberRoleLevelFailed(errCode, QString::fromUtf8(errMsg), cookie);
}

void CRIMCallbacker_Qt::setGrpLookMemberInfoSuccess(const char *data, const char *cookie)
{
	CRIMMgrLogDebug("setGrpLookMemberInfoSuccess."); 
	emit CRIMManager_Qt::instance()->s_setGrpLookMemberInfoSuccess(QString::fromUtf8(data), cookie);
}

void CRIMCallbacker_Qt::setGrpLookMemberInfoFailure(int errCode, const char *errMsg, const char *cookie)
{
	CRIMMgrLogWarn("setGrpLookMemberInfoFailure...errCode:%d, errMsg:%s", errCode, errMsg); 
	emit CRIMManager_Qt::instance()->s_setGrpLookMemberInfoFailed(errCode, QString::fromUtf8(errMsg), cookie);
}

void CRIMCallbacker_Qt::setGrpApplyMemberFriendSuccess(const char *data, const char *cookie)
{
	CRIMMgrLogDebug("setGrpApplyMemberFriendSuccess."); 
	emit CRIMManager_Qt::instance()->s_setGrpApplyMemberFriendSuccess(QString::fromUtf8(data), cookie);
}

void CRIMCallbacker_Qt::setGrpApplyMemberFriendFailure(int errCode, const char *errMsg, const char *cookie)
{
	CRIMMgrLogWarn("setGrpApplyMemberFriendFailure...errCode:%d, errMsg:%s", errCode, errMsg); 
	emit CRIMManager_Qt::instance()->s_setGrpApplyMemberFriendFailed(errCode, QString::fromUtf8(errMsg), cookie);
}

void CRIMCallbacker_Qt::getGrpMemberListSuccess(const char *data, const char *cookie)
{
	QList<CRIMGrpMemberInfo> infos = CRIM::toStructList<CRIMGrpMemberInfo>(data);
	CRIMMgrLogDebug("getGrpMemberListSuccess...count:%d", infos.size()); 
	emit CRIMManager_Qt::instance()->m_getGrpMemberListSuccess(infos, cookie);
}

void CRIMCallbacker_Qt::getGrpMemberListFailure(int errCode, const char *errMsg, const char *cookie)
{
	CRIMMgrLogWarn("getGrpMemberListFailure...errCode:%d, errMsg:%s", errCode, errMsg); 
	emit CRIMManager_Qt::instance()->m_getGrpMemberListFailed(errCode, QString::fromUtf8(errMsg), cookie);
}

void CRIMCallbacker_Qt::getGrpMemberListByJoinTimeFilterSuccess(const char *data, const char *cookie)
{
// 	QList<CRIMGroupMemberInfo> infos = CRIM::toStructList<CRIMGroupMemberInfo>(data);
// 	CRIMMgrLogDebug("getGrpMemberListByJoinTimeFilterSuccess...count:%d", infos.size()); 
// 	emit CRIMManager_Qt::instance()->m_getGrpMemberListByJoinTimeFilterSuccess(infos, cookie);
}

void CRIMCallbacker_Qt::getGrpMemberListByJoinTimeFilterFailure(int errCode, const char *errMsg, const char *cookie)
{
// 	CRIMMgrLogWarn("getGrpMemberListByJoinTimeFilterFailure...errCode:%d, errMsg:%s", errCode, errMsg); 
// 	emit CRIMManager_Qt::instance()->m_getGrpMemberListByJoinTimeFilterFailed(errCode, QString::fromUtf8(errMsg), cookie);
}

void CRIMCallbacker_Qt::setGrpMemberInfoSuccess(const char *data, const char *cookie)
{
	CRIMMgrLogDebug("setGrpMemberInfoSuccess."); 
	emit CRIMManager_Qt::instance()->s_setGrpMemberInfoSuccess(QString::fromUtf8(data), cookie);
}

void CRIMCallbacker_Qt::setGrpMemberInfoFailure(int errCode, const char *errMsg, const char *cookie)
{
	CRIMMgrLogWarn("setGrpMemberInfoFailure...errCode:%d, errMsg:%s", errCode, errMsg); 
	emit CRIMManager_Qt::instance()->s_setGrpMemberInfoFailed(errCode, QString::fromUtf8(errMsg), cookie);
}

void CRIMCallbacker_Qt::getSpecifiedGrpMembersInfoSuccess(const char *data, const char *cookie)
{
	// 	QList<GroupMemberInfo> infos = CRIM::toStructList<CRIMGroupMemberInfo>(data);
	// 	CRIMMgrLogDebug("getGroupMembersInfoSuccess...count:%d", infos.size());
	// 	emit CRIMManager_Qt::instance()->s_getGroupMembersInfoSuccess(infos, cookie);
}

void CRIMCallbacker_Qt::getSpecifiedGrpMembersInfoFailure(int errCode, const char *errMsg, const char *cookie)
{
	//  CRIMMgrLogWarn("getGroupMembersInfoFailure...errCode:%d, errMsg:%s", errCode, errMsg);
	// 	emit CRIMManager_Qt::instance()->s_getGroupMembersInfoFailed(errCode, QString::fromUtf8(errMsg), cookie);
}

void CRIMCallbacker_Qt::setGrpMemberNicknameSuccess(const char *data, const char *cookie)
{
	CRIMMgrLogDebug("setGrpMemberNicknameSuccess."); 
	emit CRIMManager_Qt::instance()->s_setGrpMemberNicknameSuccess(QString::fromUtf8(data), cookie);
}

void CRIMCallbacker_Qt::setGrpMemberNicknameFailure(int errCode, const char *errMsg, const char *cookie)
{
	CRIMMgrLogWarn("setGrpMemberNicknameFailure...errCode:%d, errMsg:%s", errCode, errMsg); 
	emit CRIMManager_Qt::instance()->s_setGrpMemberNicknameFailed(errCode, QString::fromUtf8(errMsg), cookie);
}

void CRIMCallbacker_Qt::getGrpMemberOwnerAndAdminSuccess(const char *data, const char *cookie)
{
	CRIMMgrLogDebug("getGrpMemberOwnerAndAdminSuccess."); 
	emit CRIMManager_Qt::instance()->s_getGrpMemberOwnerAndAdminSuccess(QString::fromUtf8(data), cookie);
}

void CRIMCallbacker_Qt::getGrpMemberOwnerAndAdminFailure(int errCode, const char *errMsg, const char *cookie)
{
	CRIMMgrLogWarn("getGrpMemberOwnerAndAdminFailure...errCode:%d, errMsg:%s", errCode, errMsg); 
	emit CRIMManager_Qt::instance()->s_getGrpMemberOwnerAndAdminFailed(errCode, QString::fromUtf8(errMsg), cookie);
}

void CRIMCallbacker_Qt::setGrpVerificationSuccess(const char *data, const char *cookie)
{
	CRIMMgrLogDebug("setGrpVerificationSuccess."); 
	emit CRIMManager_Qt::instance()->s_setGrpVerificationSuccess(QString::fromUtf8(data), cookie);
}

void CRIMCallbacker_Qt::setGrpVerificationFailure(int errCode, const char *errMsg, const char *cookie)
{
	CRIMMgrLogWarn("setGrpVerificationFailure...errCode:%d, errMsg:%s", errCode, errMsg); 
	emit CRIMManager_Qt::instance()->s_setGrpVerificationFailed(errCode, QString::fromUtf8(errMsg), cookie);
}

void CRIMCallbacker_Qt::inviteUserToGrpSuccess(const char *data, const char *cookie)
{
	QList<CRIMSimpleResultInfo> infos = CRIM::toStructList<CRIMSimpleResultInfo>(data);
	CRIMMgrLogDebug("inviteUserToGrpSuccess...count:%d", infos.size());
	emit CRIMManager_Qt::instance()->s_inviteUserToGrpSuccess(infos, cookie);
}

void CRIMCallbacker_Qt::inviteUserToGrpFailure(int errCode, const char *errMsg, const char *cookie)
{
	CRIMMgrLogWarn("inviteUserToGrpFailure...errCode:%d, errMsg:%s", errCode, errMsg);
	emit CRIMManager_Qt::instance()->s_inviteUserToGrpFailed(errCode, QString::fromUtf8(errMsg), cookie);
}

void CRIMCallbacker_Qt::kickGrpMemberSuccess(const char *data, const char *cookie)
{
	QList<CRIMSimpleResultInfo> infos = CRIM::toStructList<CRIMSimpleResultInfo>(data);
	CRIMMgrLogDebug("kickGrpMemberSuccess...count:%s", infos.size());
	emit CRIMManager_Qt::instance()->s_kickGrpMemberSuccess(infos, cookie);
}

void CRIMCallbacker_Qt::kickGrpMemberFailure(int errCode, const char *errMsg, const char *cookie)
{
	CRIMMgrLogWarn("kickGrpMemberFailure...errCode:%d, errMsg:%s", errCode, errMsg); 
	emit CRIMManager_Qt::instance()->s_kickGrpMemberFailed(errCode, QString::fromUtf8(errMsg), cookie);
}

void CRIMCallbacker_Qt::transferGrpOwnerSuccess(const char *data, const char *cookie)
{
	CRIMMgrLogDebug("transferGrpOwnerSuccess."); 
	emit CRIMManager_Qt::instance()->s_transferGrpOwnerSuccess(QString::fromUtf8(data), cookie);
}

void CRIMCallbacker_Qt::transferGrpOwnerFailure(int errCode, const char *errMsg, const char *cookie)
{
	CRIMMgrLogWarn("transferGrpOwnerFailure...errCode:%d, errMsg:%s", errCode, errMsg); 
	emit CRIMManager_Qt::instance()->s_transferGrpOwnerFailed(errCode, QString::fromUtf8(errMsg), cookie);
}

void CRIMCallbacker_Qt::getGrpReqListAsRecipientSuccess(const char *data, const char *cookie)
{
	QList<CRIMGrpReqInfo> lst = CRIM::toStructList<CRIMGrpReqInfo>(data);
	CRIMMgrLogDebug("getGrpReqListAsRecipientSuccess...count:%s", lst.count());
	emit CRIMManager_Qt::instance()->s_getGrpReqListAsRecipientSuccess(lst, cookie);
}

void CRIMCallbacker_Qt::getGrpReqListAsRecipientFailure(int errCode, const char *errMsg, const char *cookie)
{
	CRIMMgrLogWarn("getGrpReqListAsRecipientFailure...errCode:%d, errMsg:%s", errCode, errMsg); 
	emit CRIMManager_Qt::instance()->s_getGrpReqListAsRecipientFailed(errCode, QString::fromUtf8(errMsg), cookie);
}

void CRIMCallbacker_Qt::getGrpReqListAsApplicantSuccess(const char *data, const char *cookie)
{
	QList<CRIMGrpReqInfo> lst = CRIM::toStructList<CRIMGrpReqInfo>(data);
	CRIMMgrLogDebug("getGrpReqListAsApplicantSuccess...count:%s", lst.size());
	emit CRIMManager_Qt::instance()->s_getGrpReqListAsApplicantSuccess(lst, cookie);
}

void CRIMCallbacker_Qt::getGrpReqListAsApplicantFailure(int errCode, const char *errMsg, const char *cookie)
{
	CRIMMgrLogWarn("getGrpReqListAsApplicantFailure...errCode:%d, errMsg:%s", errCode, errMsg); 
	emit CRIMManager_Qt::instance()->s_getGrpReqListAsApplicantFailed(errCode, QString::fromUtf8(errMsg), cookie);
}

void CRIMCallbacker_Qt::acceptGrpReqSuccess(const char *data, const char *cookie)
{
	CRIMMgrLogDebug("acceptGrpReqSuccess."); 
	emit CRIMManager_Qt::instance()->s_acceptGrpReqSuccess(QString::fromUtf8(data), cookie);
}

void CRIMCallbacker_Qt::acceptGrpReqFailure(int errCode, const char *errMsg, const char *cookie)
{
	CRIMMgrLogWarn("acceptGrpReqFailure...errCode:%d, errMsg:%s", errCode, errMsg); 
	emit CRIMManager_Qt::instance()->s_acceptGrpReqFailed(errCode, QString::fromUtf8(errMsg), cookie);
}

void CRIMCallbacker_Qt::refuseGrpReqSuccess(const char *data, const char *cookie)
{
	CRIMMgrLogDebug("refuseGrpReqSuccess."); 
	emit CRIMManager_Qt::instance()->s_refuseGrpReqSuccess(QString::fromUtf8(data), cookie);
}

void CRIMCallbacker_Qt::refuseGrpReqFailure(int errCode, const char *errMsg, const char *cookie)
{
	CRIMMgrLogWarn("refuseGrpReqFailure...errCode:%d, errMsg:%s", errCode, errMsg); 
	emit CRIMManager_Qt::instance()->s_refuseGrpReqFailed(errCode, QString::fromUtf8(errMsg), cookie);
}

void CRIMCallbacker_Qt::isJoinGrpSuccess(const char *data, const char *cookie)
{
	CRIMMgrLogDebug("isJoinGrpSuccess."); 
	emit CRIMManager_Qt::instance()->s_isJoinGrpSuccess(QString::fromUtf8(data), cookie);
}

void CRIMCallbacker_Qt::isJoinGrpFailure(int errCode, const char *errMsg, const char *cookie)
{
	CRIMMgrLogWarn("isJoinGrpFailure...errCode:%d, errMsg:%s", errCode, errMsg); 
	emit CRIMManager_Qt::instance()->s_isJoinGrpFailed(errCode, QString::fromUtf8(errMsg), cookie);
}

void CRIMCallbacker_Qt::searchGrpMembersSuccess(const char *data, const char *cookie)
{
	CRIMMgrLogDebug("searchGrpMembersSuccess."); 
	emit CRIMManager_Qt::instance()->s_searchGrpMembersSuccess(QString::fromUtf8(data), cookie);
}

void CRIMCallbacker_Qt::searchGrpMembersFailure(int errCode, const char *errMsg, const char *cookie)
{
	CRIMMgrLogWarn("searchGrpMembersFailure...errCode:%d, errMsg:%s", errCode, errMsg); 
	emit CRIMManager_Qt::instance()->s_searchGrpMembersFailed(errCode, QString::fromUtf8(errMsg), cookie);
}

/**********************************回调**********************************/
void CRIMCallbacker_Qt::notifyConnecting()
{
	CRIMMgrLogDebug("Callback:OnConnecting");
	emit CRIMManager_Qt::instance()->s_connecting();
}

void CRIMCallbacker_Qt::notifyConnectSuccess()
{
	CRIMMgrLogDebug("Callback:OnConnectSuccess");
	emit CRIMManager_Qt::instance()->s_connectSuccess();
}

void CRIMCallbacker_Qt::notifyConnectFailed(int errCode, const char *errMsg, const char *cookie)
{
	CRIMMgrLogDebug("Callback:OnConnectFailed...errCode:%d, errMsg:%s", errCode, errMsg);
	emit CRIMManager_Qt::instance()->m_connectFailed(errCode);
}

void CRIMCallbacker_Qt::notifyKickedOffline()
{
	CRIMMgrLogDebug("Callback:OnKickedOffline");
	emit CRIMManager_Qt::instance()->m_kickedOffline(0);
}

void CRIMCallbacker_Qt::notifyUserTokenExpired()
{
	CRIMMgrLogDebug("OnUserTokenExpired...");
	emit CRIMManager_Qt::instance()->s_userTokenExpired();
}

void CRIMCallbacker_Qt::notifyUserTokenInvalid(const char *data)
{
	CRIMMgrLogDebug("Callback:notifyUserTokenInvalid...");
	emit CRIMManager_Qt::instance()->s_userTokenInvalid(data);
}

void CRIMCallbacker_Qt::notifySyncServerStart(int ret)
{
	CRIMMgrLogDebug("Callback:notifySyncServerStart...");
	emit CRIMManager_Qt::instance()->s_syncServerStart();
}

void CRIMCallbacker_Qt::notifySyncServerFinish(int ret)
{
	CRIMMgrLogDebug("Callback:notifySyncServerFinish...");
	emit CRIMManager_Qt::instance()->m_syncServerFinish();
}

void CRIMCallbacker_Qt::notifySyncServerFailed(int ret)
{
	CRIMMgrLogDebug("Callback:notifySyncServerFailed...err:%d", ret);
	emit CRIMManager_Qt::instance()->s_syncServerFailed(ret);
}

void CRIMCallbacker_Qt::notifySyncServerProgress(int ret)
{
	CRIMMgrLogDebug("Callback:notifySyncServerProgress...progress:%d", ret);
	emit CRIMManager_Qt::instance()->s_syncServerProgress(ret);
}

void CRIMCallbacker_Qt::notifySelfInfoUpdated(const char *data)
{
	CRIMMgrLogDebug("Callback:OnSelfInfoUpdated..."); 
	CRIMUserInfo old = CRIMManager_Qt::instance()->getSelfUserInfo();
	CRIMUserInfo info = CRIM::toStruct<CRIMUserInfo>(data);
	emit CRIMManager_Qt::instance()->s_selfInfoUpdated(old, info);
}

void CRIMCallbacker_Qt::notifyUserStatusChanged(const char *data)
{
	CRIMUserStatusInfo info = CRIM::toStruct<CRIMUserStatusInfo>(data);
	CRIMMgrLogDebug("Callback:notifyUserStatusChanged...data:%s", data);

	emit CRIMManager_Qt::instance()->s_userStatusChanged(info);
}

void CRIMCallbacker_Qt::notifyUserCommandAdd(const char *data)
{
	CRIMMgrLogDebug("Callback:notifyUserCommandAdd...");
}
void CRIMCallbacker_Qt::notifyUserCommandDelete(const char *data)
{
	CRIMMgrLogDebug("Callback:notifyUserCommandDelete...");
}
void CRIMCallbacker_Qt::notifyUserCommandUpdate(const char *data)
{
	CRIMMgrLogDebug("Callback:notifyUserCommandUpdate...");
}

void CRIMCallbacker_Qt::notifyRecvNewMsgs(const char *data)
{
	CRIMMgrLogDebug("Callback:notifyRecvNewMsgs...");
	int receiverCount = CRIMManager_Qt::instance()->receivers(SIGNAL(s_recvNewMsgs(const QList<CRIMMessage> &)));
	receiverCount += CRIMManager_Qt::instance()->receivers(SIGNAL(s_notifyCallIn(const CRIMInvitationInfo &)));
	//有信号槽连接时才进行数据处理，优化速度
	if (receiverCount > 0)
	{
		QList<CRIMMessage> msgs = CRIM::toStructList<CRIMMessage>(data);
		for (auto &msg : msgs)
		{
			CRIMMgrLogDebug("Callback:notifyRecvNewMsgs...clientMsgID:%s, sendID:%s, groupID:%s", msg.clientMsgID.toUtf8().constData(), msg.sendID.toUtf8().constData(), msg.groupID.toUtf8().constData());
			//处理呼叫类型的消息
			if (msg.contentType == CRIM_MsgType_Custom && msg.customElem.isInviteType())
			{
				//呼叫类型的消息自动已读
				QString id = (msg.sessionType == CRIM_Conversation_1v1) ? msg.sendID : msg.groupID;
				QString conversationID = CRIMManager_Qt::instance()->getConversationIDBySessionType(id, msg.sessionType);
				CRIMManager_Qt::instance()->markMsgAsReadByMsgID(conversationID, QStringList(msg.clientMsgID));
			
				CRIMInvitationInfo inviteInfo = *(msg.customElem.inviteInfo);
				switch (msg.customElem.customType)
				{
				case CRCUSTOMMSG_NewInvitation:
					if (msg.sendID == CRIMManager_Qt::instance()->getUserID()) //自已其它设备的消息
					{
						CRIMMgrLogDebug("notifyCallByOtherDevice...invitationMsgId:%s", inviteInfo.invitationMsgId.toUtf8().constData());
						break;
					}
					CRIMMgrLogDebug("notifyCallIn...invitationMsgId:%s", inviteInfo.invitationMsgId.toUtf8().constData());
					emit CRIMManager_Qt::instance()->s_notifyCallIn(inviteInfo);
					break;
				case CRCUSTOMMSG_InviteeAccept:
					if (msg.sendID == CRIMManager_Qt::instance()->getUserID()) //自已其它设备的消息
					{
						CRIMMgrLogDebug("notifyCallAcceptedByOtherDevice...invitationMsgId:%s", inviteInfo.invitationMsgId.toUtf8().constData());
						emit CRIMManager_Qt::instance()->s_notifyCallAcceptedByOtherDevice(inviteInfo);
						break;
					}
					CRIMMgrLogDebug("notifyCallAccepted...invitationMsgId:%s", inviteInfo.invitationMsgId.toUtf8().constData());
					emit CRIMManager_Qt::instance()->s_notifyCallAccepted(inviteInfo);
					break;
				case CRCUSTOMMSG_InviteeReject:
					if (msg.sendID == CRIMManager_Qt::instance()->getUserID()) //自已其它设备的消息
					{
						CRIMMgrLogDebug("notifyCallRejectedByOtherDevice...invitationMsgId:%s", inviteInfo.invitationMsgId.toUtf8().constData());
						emit CRIMManager_Qt::instance()->s_notifyCallRejectedByOtherDevice(inviteInfo);
						break;
					}
					CRIMMgrLogDebug("notifyCallRejected...invitationMsgId:%s, opType:%d, opDesc:%s", inviteInfo.invitationMsgId.toUtf8().constData(), inviteInfo.opType, inviteInfo.opDesc.toUtf8().constData());
					emit CRIMManager_Qt::instance()->s_notifyCallRejected(inviteInfo);
					break;
				case CRCUSTOMMSG_InvitationCancel:
					CRIMMgrLogDebug("notifyCallRejected...notifyCallCancelled:%s, opType:%d, opDesc:%s", inviteInfo.invitationMsgId.toUtf8().constData(), inviteInfo.opType, inviteInfo.opDesc.toUtf8().constData());
					emit CRIMManager_Qt::instance()->s_notifyCallCancelled(inviteInfo);
					break;
				case CRCUSTOMMSG_InvitationHangUp:
					CRIMMgrLogDebug("notifyCallRejected...notifyCallHungUp:%s, opType:%d, opDesc:%s", inviteInfo.invitationMsgId.toUtf8().constData(), inviteInfo.opType, inviteInfo.opDesc.toUtf8().constData());
					emit CRIMManager_Qt::instance()->s_notifyCallHungUp(inviteInfo);
					break;
				default:
					break;
				}
			}
		}
		//通知收到新消息
		emit CRIMManager_Qt::instance()->s_recvNewMsgs(msgs);
	}
}

void CRIMCallbacker_Qt::notifyRecvMsgRevoked(const char *data)
{
	CRIMRevokedInfo info = CRIM::toStruct<CRIMRevokedInfo>(data);
	CRIMMgrLogDebug("Callback:notifyNewRecvMsgRevoked...clientMsgID:%s", info.clientMsgID.toUtf8().constData()); 
	emit CRIMManager_Qt::instance()->s_recvMsgRevoked(info);
}

void CRIMCallbacker_Qt::notifyMsgDeleted(const char *data)
{
	int receiverCount = CRIMManager_Qt::instance()->receivers(SIGNAL(s_msgDeleted(const CRIMMessage &)));
	//有信号槽连接时才进行数据处理，优化速度
	if (receiverCount > 0)
	{
		CRIMMessage msg = CRIM::toStruct<CRIMMessage>(data);
		CRIMMgrLogDebug("Callback:notifyMsgDeleted...clientMsgID:%s", msg.clientMsgID.toUtf8().constData()); 
		emit CRIMManager_Qt::instance()->s_msgDeleted(msg);
	}
}

void CRIMCallbacker_Qt::notifyRecv1v1ReadReceipt(const char *data)
{
	CRIMMgrLogDebug("Callback:notifyRecv1v1ReadReceipt...data:%s", data);
	QList<CRIMReadReceiptInfo> infos = CRIM::toStructList<CRIMReadReceiptInfo>(data);
	emit CRIMManager_Qt::instance()->s_recv1v1ReadReceipt(infos);
}


void CRIMCallbacker_Qt::notifyFriendAdded(const char *data)
{
	CRIMFriendInfo info = CRIM::toStruct<CRIMFriendInfo>(data);
	CRIMMgrLogDebug("Callback:notifyFriendAdded...uid:%s", info.userID.toUtf8().constData()); 
	emit CRIMManager_Qt::instance()->m_friendAdded(info);
}

void CRIMCallbacker_Qt::notifyFriendDeleted(const char *data)
{
	CRIMFriendInfo info = CRIM::toStruct<CRIMFriendInfo>(data);
	CRIMMgrLogDebug("Callback:notifyFriendDeleted...uid:%s", info.userID.toUtf8().constData());
	emit CRIMManager_Qt::instance()->m_friendDeleted(info);
}

void CRIMCallbacker_Qt::notifyFriendInfoChanged(const char *data)
{
	CRIMFriendInfo info = CRIM::toStruct<CRIMFriendInfo>(data);
	CRIMMgrLogDebug("Callback:notifyFriendInfoChanged...uid:%s", info.userID.toUtf8().constData());
	emit CRIMManager_Qt::instance()->m_friendInfoChanged(info);
}

void CRIMCallbacker_Qt::notifyFriendReqAdded(const char *data)
{
	CRIMFriendReqInfo info = CRIM::toStruct<CRIMFriendReqInfo>(data);
	CRIMMgrLogDebug("Callback:notifyFriendReqAdded...fromUID:%s, toUID:%s", info.fromUserID.toUtf8().constData(), info.toUserID.toUtf8().constData());
	emit CRIMManager_Qt::instance()->s_friendReqAdded(info);
}

void CRIMCallbacker_Qt::notifyFriendReqDeleted(const char *data)
{
	CRIMFriendReqInfo info = CRIM::toStruct<CRIMFriendReqInfo>(data);
	CRIMMgrLogDebug("Callback:notifyFriendReqDeleted...fromUID:%s, toUID:%s", info.fromUserID.toUtf8().constData(), info.toUserID.toUtf8().constData());
	emit CRIMManager_Qt::instance()->s_friendReqDeleted(info);
}

void CRIMCallbacker_Qt::notifyFriendReqAccepted(const char *data)
{
	CRIMFriendReqInfo info = CRIM::toStruct<CRIMFriendReqInfo>(data);
	CRIMMgrLogDebug("Callback:notifyFriendReqAccepted...fromUID:%s, toUID:%s", info.fromUserID.toUtf8().constData(), info.toUserID.toUtf8().constData());
	emit CRIMManager_Qt::instance()->s_friendReqAccepted(info);
}

void CRIMCallbacker_Qt::notifyFriendReqRejected(const char *data)
{
	CRIMFriendReqInfo info = CRIM::toStruct<CRIMFriendReqInfo>(data);
	CRIMMgrLogDebug("Callback:notifyFriendReqRejected...fromUID:%s, toUID:%s", info.fromUserID.toUtf8().constData(), info.toUserID.toUtf8().constData());
	emit CRIMManager_Qt::instance()->s_friendReqRejected(info);
}

void CRIMCallbacker_Qt::notifyBlackListAdded(const char *data)
{
	CRIMBlackListInfo info = CRIM::toStruct<CRIMBlackListInfo>(data);
	CRIMMgrLogDebug("Callback:notifyBlackListAdded...uid:%s", info.userID.toUtf8().constData());
	emit CRIMManager_Qt::instance()->s_blackListAdded(info);
}

void CRIMCallbacker_Qt::notifyBlackListDeleted(const char *data)
{
	CRIMBlackListInfo info = CRIM::toStruct<CRIMBlackListInfo>(data);
	CRIMMgrLogDebug("Callback:notifyBlackListDeleted...uid:%s", info.userID.toUtf8().constData());
	emit CRIMManager_Qt::instance()->s_blackListDeleted(info);
}

void CRIMCallbacker_Qt::notifyNewConversation(const char *data)
{
	QList<CRIMConversationInfo> infos = CRIM::toStructList<CRIMConversationInfo>(data);
	CRIMMgrLogDebug("Callback:notifyNewConversation...count:%d", infos.size());
	emit CRIMManager_Qt::instance()->m_newConversation(infos);
}

void CRIMCallbacker_Qt::notifyConversationChanged(const char *data)
{
	QList<CRIMConversationInfo> infos = CRIM::toStructList<CRIMConversationInfo>(data);
	CRIMMgrLogDebug("Callback:notifyConversationChanged...count:%d", infos.size());
	emit CRIMManager_Qt::instance()->s_conversationChanged(infos);
}

void CRIMCallbacker_Qt::notifyTotalUnreadMsgCountChanged(int count)
{
	CRIMMgrLogDebug("Callback:notifyTotalUnreadMsgCountChanged...count:%d", count); 
	emit CRIMManager_Qt::instance()->s_totalUnreadMsgCountChanged(count);
}

void CRIMCallbacker_Qt::notifyGrpInfoChanged(const char *data)
{
	CRIMGrpInfo info = CRIM::toStruct<CRIMGrpInfo>(data);
	CRIMMgrLogDebug("Callback:notifyGrpInfoChanged...gid:%s", info.groupID.toUtf8().constData());
	emit CRIMManager_Qt::instance()->m_grpInfoChanged(info);
}

void CRIMCallbacker_Qt::notifyJoinedGrpDismissed(const char *data)
{
	CRIMGrpInfo info = CRIM::toStruct<CRIMGrpInfo>(data);
	CRIMMgrLogDebug("Callback:notifyJoinedGrpDismissed...gid:%s", info.groupID.toUtf8().constData());
	emit CRIMManager_Qt::instance()->m_joinedGrpDismissed(info);
}

void CRIMCallbacker_Qt::notifyJoinedGrpAdded(const char *data)
{
	CRIMGrpInfo info = CRIM::toStruct<CRIMGrpInfo>(data);
	CRIMMgrLogDebug("Callback:notifyJoinedGrpAdded...gid:%s", info.groupID.toUtf8().constData());
	emit CRIMManager_Qt::instance()->m_joinedGrpAdded(info);
}

void CRIMCallbacker_Qt::notifyJoinedGrpDeleted(const char *data)
{
	CRIMGrpInfo info = CRIM::toStruct<CRIMGrpInfo>(data);
	CRIMMgrLogDebug("Callback:notifyJoinedGrpDeleted...gid:%s", info.groupID.toUtf8().constData());
	emit CRIMManager_Qt::instance()->m_joinedGrpDeleted(info);
}

void CRIMCallbacker_Qt::notifyGrpMemberInfoChanged(const char *data)
{
	CRIMGrpMemberInfo info = CRIM::toStruct<CRIMGrpMemberInfo>(data);
	CRIMMgrLogDebug("Callback:notifyGrpMemberInfoChanged...gid:%s, uid:%d", info.groupID.toUtf8().constData(), info.userID.toUtf8().constData());
	emit CRIMManager_Qt::instance()->m_grpMemberInfoChanged(info);
}

void CRIMCallbacker_Qt::notifyGrpMemberAdded(const char *data)
{
	CRIMGrpMemberInfo info = CRIM::toStruct<CRIMGrpMemberInfo>(data);
	CRIMMgrLogDebug("Callback:notifyGrpMemberAdded...gid:%s, uid:%d", info.groupID.toUtf8().constData(), info.userID.toUtf8().constData());
	emit CRIMManager_Qt::instance()->m_grpMemberAdded(info);
}

void CRIMCallbacker_Qt::notifyGrpMemberDeleted(const char *data)
{
	CRIMGrpMemberInfo info = CRIM::toStruct<CRIMGrpMemberInfo>(data);
	CRIMMgrLogDebug("Callback:notifyGrpMemberDeleted...gid:%s, uid:%d", info.groupID.toUtf8().constData(), info.userID.toUtf8().constData());
	emit CRIMManager_Qt::instance()->m_grpMemberDeleted(info);
}

void CRIMCallbacker_Qt::notifyGrpReqAdded(const char *data)
{
	CRIMGrpReqInfo info = CRIM::toStruct<CRIMGrpReqInfo>(data);
	CRIMMgrLogDebug("Callback:notifyGrpReqAdded...gid:%s, uid:%d", info.groupID.toUtf8().constData(), info.userID.toUtf8().constData());
	emit CRIMManager_Qt::instance()->s_grpReqAdded(info);
}

void CRIMCallbacker_Qt::notifyGrpReqDeleted(const char *data)
{
	CRIMGrpReqInfo info = CRIM::toStruct<CRIMGrpReqInfo>(data);
	CRIMMgrLogDebug("Callback:notifyGrpReqDeleted...gid:%s, uid:%d", info.groupID.toUtf8().constData(), info.userID.toUtf8().constData());
	emit CRIMManager_Qt::instance()->s_grpReqDeleted(info);
}

void CRIMCallbacker_Qt::notifyGrpReqAccepted(const char *data)
{
	CRIMGrpReqInfo info = CRIM::toStruct<CRIMGrpReqInfo>(data);
	CRIMMgrLogDebug("Callback:notifyGrpReqAccepted...gid:%s, uid:%d", info.groupID.toUtf8().constData(), info.userID.toUtf8().constData());
	emit CRIMManager_Qt::instance()->s_grpReqAccepted(info);
}

void CRIMCallbacker_Qt::notifyGrpReqRejected(const char *data)
{
	CRIMGrpReqInfo info = CRIM::toStruct<CRIMGrpReqInfo>(data);
	CRIMMgrLogDebug("Callback:notifyGrpReqRejected...gid:%s, uid:%d", info.groupID.toUtf8().constData(), info.userID.toUtf8().constData());
	emit CRIMManager_Qt::instance()->s_grpReqRejected(info);
}
