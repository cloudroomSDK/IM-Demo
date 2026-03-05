#include "stdafx.h"
#include "CRIM_Defines.h"

CRIMLogLevel g_imLogLevel = CRIM_ERRORLEVEL;
QString getErrDsc(CRIM_ERR_DEF err)
{
	switch (err)
	{

	case CRIM_ParseToken:				return QObject::tr("Token异常");

	case CRIM_ThirdPartyCredential:		return QObject::tr("第三方验证失败");

	case CRIM_InBlackList:				return QObject::tr("在黑名单中");
	case CRIM_NotFriend:				return QObject::tr("不是好友");

	case CRIM_TokenExpired:				return QObject::tr("Token过期");
	case CRIM_TokenInvalid:				return QObject::tr("Token无效");
	case CRIM_TokenMalformed:			return QObject::tr("Token格式不对");
	case CRIM_TokenNotValidYet:			return QObject::tr("Token未激活");
	case CRIM_TokenUnknown:				return QObject::tr("无法处理此Token");
	case CRIM_TokenKicked:				return QObject::tr("用户被踢下线");

	case CRIM_Access:					return QObject::tr("没有权限");
	case CRIM_DB:						return QObject::tr("访问数据库失败");
	case CRIM_Args:						return QObject::tr("参数错误");
	case CRIM_Api:						return QObject::tr("接口错误");
	case CRIM_Data:						return QObject::tr("数据错误");
	case CRIM_Login:					return QObject::tr("只能登录一次");
	case CRIM_Config:					return QObject::tr("配置错误");
	case CRIM_ThirdParty:				return QObject::tr("第三方异常");

	case CRIM_WsRecvConnDiff:			return QObject::tr("接收超时，不同的连接");
	case CRIM_WsRecvConnSame:			return QObject::tr("接收超时，相同的连接");
	case CRIM_WsRecvCode:				return QObject::tr("接收码错误");
	case CRIM_WsSendTimeout:			return QObject::tr("发送超时");
	case CRIM_ResourceLoadNotComplete:	return QObject::tr("资源加载未完成");
	case CRIM_NotSupportFunction:		return QObject::tr("不支持的功能");

	case CRIM_HttpTimeout:				return QObject::tr("请求超时");
	case CRIM_HttpRspDatErr:			return QObject::tr("响应数据错误");
	case CRIM_HttpSSLErr:				return QObject::tr("ssl通信失败");
	case CRIM_HttpCmdFailed:
	case CRIM_HttpErr:					return QObject::tr("请求失败");

	default: return QObject::tr("未定义错误(%1)").arg(QString::number(int(err)));
	}
}

bool CRIMAtElem::isAtSelf() const
{
	if (atUserList.count() <= 0)
	{
		return false;
	}
	return atUserList.contains(CRIMManager_Qt::instance()->getUserID());
}

bool CRIMAtElem::isAtAll() const 
{
	if (atUserList.count() <= 0)
	{
		return false;
	}
	QString atUser = atUserList.first();
	return !atUser.isEmpty() && CRIMManager_Qt::instance()->getAtAllTag() == atUser;
}

bool CRIMGrpMemberInfo::isOwner() const
{
	return roleLevel == CRIM_GMR_Owner;
}
bool CRIMGrpMemberInfo::isManager() const
{
	return roleLevel == CRIM_GMR_Admin || roleLevel == CRIM_GMR_Owner;
}

QString CRIMConversationInfo::showName() const
{
	if (conversationType == CRIM_Conversation_1v1)
	{
		const CRIMFriendInfo* pInfo = CRIMManager_Qt::instance()->getFriendInfo(userID);
		if (pInfo != NULL)
		{
			return pInfo->getDisplayName();
		}
	}
	return orgName;
}

bool CRIMCustomElem::isInviteType() const
{
	return customType >= CRCUSTOMMSG_NewInvitation && customType <= CRCUSTOMMSG_InvitationHangUp;
}

void CRIMInvitationInfo::makeDuration()
{
	int acpTs = (acceptTime > 0) ? acceptTime : initiateTime;
	int dur = (CRIMManager_Qt::instance()->getCurrentSvrTime() / 1000) - acpTs;
	duration = qMax(dur, 0);
}