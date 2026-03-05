#ifndef CRIM_DATACONV_H
#define CRIM_DATACONV_H
#include "CRIM_Defines.h"
namespace CRIM
{
	void Struct_Conv(const CRIMOfflinePushInfo &src, QVariantMap &dst);
	void Struct_Conv(const QVariantMap &src, CRIMOfflinePushInfo &dst);
	void Struct_Conv(const CRIMPictureInfo &src, QVariantMap &dst);
	void Struct_Conv(const QVariantMap &src, CRIMPictureInfo &dst);
	void Struct_Conv(const CRIMPictureElem &src, QVariantMap &dst);
	void Struct_Conv(const QVariantMap &src, CRIMPictureElem &dst);
    void Struct_Conv(const CRIMSoundElem &src, QVariantMap &dst);
    void Struct_Conv(const QVariantMap &src, CRIMSoundElem &dst);
    void Struct_Conv(const CRIMVideoElem &src, QVariantMap &dst);
    void Struct_Conv(const QVariantMap &src, CRIMVideoElem &dst);
    void Struct_Conv(const CRIMFileElem &src, QVariantMap &dst);
    void Struct_Conv(const QVariantMap &src, CRIMFileElem &dst);
    void Struct_Conv(const CRIMAtInfo &src, QVariantMap &dst);
    void Struct_Conv(const QVariantMap &src, CRIMAtInfo &dst);
    void Struct_Conv(const CRIMAtElem &src, QVariantMap &dst);
    void Struct_Conv(const QVariantMap &src, CRIMAtElem &dst);
    void Struct_Conv(const CRIMFaceElem &src, QVariantMap &dst);
    void Struct_Conv(const QVariantMap &src, CRIMFaceElem &dst);
    void Struct_Conv(const CRIMLocationElem &src, QVariantMap &dst);
    void Struct_Conv(const QVariantMap &src, CRIMLocationElem &dst);
    void Struct_Conv(const CRIMCustomElem &src, QVariantMap &dst);
    void Struct_Conv(const QVariantMap &src, CRIMCustomElem &dst);
    void Struct_Conv(const CRIMMergeElem &src, QVariantMap &dst);
    void Struct_Conv(const QVariantMap &src, CRIMMergeElem &dst);
    void Struct_Conv(const CRIMQuoteElem &src, QVariantMap &dst);
    void Struct_Conv(const QVariantMap &src, CRIMQuoteElem &dst);
    void Struct_Conv(const CRIMNotificationElem &src, QVariantMap &dst);
    void Struct_Conv(const QVariantMap &src, CRIMNotificationElem &dst);
    void Struct_Conv(const CRIMGroupHasReadInfo &src, QVariantMap &dst);
    void Struct_Conv(const QVariantMap &src, CRIMGroupHasReadInfo &dst);
    void Struct_Conv(const CRIMAttachedInfoElem &src, QVariantMap &dst);
    void Struct_Conv(const QVariantMap &src, CRIMAttachedInfoElem &dst);
	void Struct_Conv(const CRIMTextElem &src, QVariantMap &dst);
	void Struct_Conv(const QVariantMap &src, CRIMTextElem &dst);
	void Struct_Conv(const CRIMCardElem &src, QVariantMap &dst);
	void Struct_Conv(const QVariantMap &src, CRIMCardElem &dst);
	void Struct_Conv(const CRIMTypingElem &src, QVariantMap &dst);
	void Struct_Conv(const QVariantMap &src, CRIMTypingElem &dst);
	void Struct_Conv(const CRIMMessage &src, QVariantMap &dst);
    void Struct_Conv(const QVariantMap &src, CRIMMessage &dst);
    void Struct_Conv(const CRIMReadReceiptInfo &src, QVariantMap &dst);
    void Struct_Conv(const QVariantMap &src, CRIMReadReceiptInfo &dst);
    void Struct_Conv(const CRIMPublicUserInfo &src, QVariantMap &dst);
    void Struct_Conv(const QVariantMap &src, CRIMPublicUserInfo &dst);
    void Struct_Conv(const CRIMFriendInfo &src, QVariantMap &dst);
    void Struct_Conv(const QVariantMap &src, CRIMFriendInfo &dst);
    void Struct_Conv(const CRIMBlackListInfo &src, QVariantMap &dst);
    void Struct_Conv(const QVariantMap &src, CRIMBlackListInfo &dst);
	void Struct_Conv(const CRIMUserStatusInfo &src, QVariantMap &dst);
	void Struct_Conv(const QVariantMap &src, CRIMUserStatusInfo &dst);
	void Struct_Conv(const CRIMUserInfo &src, QVariantMap &dst);
	void Struct_Conv(const QVariantMap &src, CRIMUserInfo &dst);
    void Struct_Conv(const CRIMFriendReqInfo &src, QVariantMap &dst);
    void Struct_Conv(const QVariantMap &src, CRIMFriendReqInfo &dst);
    void Struct_Conv(const CRIMGrpBaseInfo &src, QVariantMap &dst);
    void Struct_Conv(const QVariantMap &src, CRIMGrpBaseInfo &dst);
    void Struct_Conv(const CRIMGrpCreateInfo &src, QVariantMap &dst);
    void Struct_Conv(const QVariantMap &src, CRIMGrpCreateInfo &dst);
    void Struct_Conv(const CRIMGrpInfo &src, QVariantMap &dst);
    void Struct_Conv(const QVariantMap &src, CRIMGrpInfo &dst);
    void Struct_Conv(const CRIMGrpMemberInfo &src, QVariantMap &dst);
    void Struct_Conv(const QVariantMap &src, CRIMGrpMemberInfo &dst);
    void Struct_Conv(const CRIMGrpReqInfo &src, QVariantMap &dst);
    void Struct_Conv(const QVariantMap &src, CRIMGrpReqInfo &dst);
    void Struct_Conv(const CRIMSimpleResultInfo &src, QVariantMap &dst);
    void Struct_Conv(const QVariantMap &src, CRIMSimpleResultInfo &dst);
    void Struct_Conv(const CRIMConversationInfo &src, QVariantMap &dst);
    void Struct_Conv(const QVariantMap &src, CRIMConversationInfo &dst);
	void Struct_Conv(const CRIMNotDisturbInfo &src, QVariantMap &dst);
	void Struct_Conv(const QVariantMap &src, CRIMNotDisturbInfo &dst);
	void Struct_Conv(const CRIMSearchResultItem &src, QVariantMap &dst);
    void Struct_Conv(const QVariantMap &src, CRIMSearchResultItem &dst);
    void Struct_Conv(const CRIMSearchResultInfo &src, QVariantMap &dst);
    void Struct_Conv(const QVariantMap &src, CRIMSearchResultInfo &dst);
	void Struct_Conv(const CRIMAdvancedMsg &src, QVariantMap &dst);
	void Struct_Conv(const QVariantMap &src, CRIMAdvancedMsg &dst);
	void Struct_Conv(const CRIMRevokedInfo &src, QVariantMap &dst);
	void Struct_Conv(const QVariantMap &src, CRIMRevokedInfo &dst);
	void Struct_Conv(const CRIMInvitationInfo &src, QVariantMap &dst);
	void Struct_Conv(const QVariantMap &src, CRIMInvitationInfo &dst);
	void Struct_Conv(const CRIMInvitationResultInfo &src, QVariantMap &dst);
	void Struct_Conv(const QVariantMap &src, CRIMInvitationResultInfo &dst);
	void Struct_Conv(const CRIMInvitationInfo &src, QVariantMap &dst);
	void Struct_Conv(const QVariantMap &src, CRIMInvitationInfo &dst);
	void Struct_Conv(const CRIMSearchResultItem &src, QVariantMap &dst);
	void Struct_Conv(const CRIMGetFriendReqListAsRecipientReq &src, QVariantMap &dst);
	void Struct_Conv(const CRIMGetFriendReqListAsApplicantReq &src, QVariantMap &dst);
	void Struct_Conv(const CRIMGetGroupReqListAsRecipientReq &src, QVariantMap &dst);
	void Struct_Conv(const CRIMGetGroupReqListAsApplicantReq &src, QVariantMap &dst);
	void Struct_Conv(const CRIMUpdateFriendsReq &src, QVariantMap &dst);

	template<typename T>
    QByteArray toJson(const T &obj)
    {
        QVariantMap varMap;
        Struct_Conv(obj, varMap);
        return QJsonDocument::fromVariant(varMap).toJson(QJsonDocument::Compact);
    }
    template<typename T>
    T toStruct(const QByteArray &json)
    {
        QVariant var = QJsonDocument::fromJson(json).toVariant();
        T obj;
        Struct_Conv(var.toMap(), obj);
        return obj;
    }
    template<typename T>
    QByteArray toJsonList(const QList<T> &objList)
    {
        QVariantList varList;
        for (auto &obj : objList)
        {
            QVariantMap varMap;
            Struct_Conv(obj, varMap);
            varList.append(varMap);
        }
        return QJsonDocument::fromVariant(varList).toJson(QJsonDocument::Compact);
    }
    template<typename T>
    QList<T> toStructList(const QByteArray &json)
    {
        QVariantList varList = QJsonDocument::fromJson(json).toVariant().toList();
        QList<T> objList;
        for (auto &var : varList)
        {
            T obj;
            Struct_Conv(var.toMap(), obj);
            objList.append(obj);
        }
        return objList;
    }
}
#endif//CRIM_DATACONV_H
