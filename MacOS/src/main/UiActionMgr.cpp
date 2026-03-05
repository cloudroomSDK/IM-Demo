#include "stdafx.h"
#include "UiActionMgr.h"

ActionMgr_Mgr* ActionMgr_Mgr::instance()
{
    static ActionMgr_Mgr acMgr;
    return &acMgr;
}

ActionMgr_Mgr::ActionMgr_Mgr()
{
	init();
}

struct ACTION_DATA
{
	int id;
	QString text;
	//QString iconName;
	QString toolTip; 
};

void ActionMgr_Mgr::init()
{
	//创建通用action
	for(int i=ACTION_ID_BEGIN; i<ACTION_ID_BUTT; i++)
	{
		QAction *pAction = new QAction(this);
		pAction->setData(i);
		pAction->setProperty(EXT_ID, "");
		pAction->setProperty(EXT_PARAMS, QVariant());
		m_Actions[i] = pAction;
	}
	//创建与群成员相关的action
	for (int i = GMEM_ACTION_ID_BEGIN; i < GMEM_ACT_ACTION_ID_BUTT; i++)
	{
		QAction *pAction = new QAction(this);
		pAction->setData(i);
		pAction->setProperty(EXT_GID, "");
		pAction->setProperty(EXT_UID, "");
		m_Actions[i] = pAction;
	}
	//配置Action基本数据
	static ACTION_DATA s_ActionData[] = 
	{ 
		//////////////////////////////////////////////////////////////////////////
		//actionID							actionText				tooltip
		{ ACT_START_Chat1v1,				tr("聊天"), "" },
		{ ACT_START_ChatGrp,				tr("聊天"), "" },
		{ ACT_START_AudioChat1v1,			tr("语音呼叫"), "" },
		{ ACT_START_VideoChat1v1,			tr("视频呼叫"), "" },

		{ ACT_NEW_GrpChat,					tr("新的群聊"), "" },

		{ ACT_SearchFor_AddFriend,			tr("添加好友"), "" },
		{ ACT_NEW_Friend,					tr("新的好友"), "" },
		{ ACT_Del_Friend,					tr("删除好友"), "" },
		{ ACT_SHOW_FriendInfo,				tr("查看用户资料"), "" },
		{ ACT_IM_VIEWCONTEXT,				tr("查看上下文"), "" },

		//////////////////////////////////////////////////////////////////////////
		{ GMEM_ACT_AT_USER,						tr("@"), "" },
		{ GMEM_ACT_TRANS_OWNER,					tr("转让群主"), "" },
		{ GMEM_ACT_SET_HOST,					tr("设为管理员"), "" },
		{ GMEM_ACT_UNSET_HOST,					tr("收回管理员"), "" },
		{ GMEM_ACT_KICK,						tr("请出群"), "" },
		{ GMEM_ACT_SHOW_GrpMemberInfo,			tr("查看用户资料"), "" },

		//////////////////////////////////////////////////////////////////////////

	};
	int nSize = sizeof(s_ActionData)/sizeof(ACTION_DATA);
	for(int i=0; i<nSize; ++i)
	{
		ACTION_DATA &data= s_ActionData[i];
		resetActionState(m_Actions[data.id], &data);		
	}
}

void ActionMgr_Mgr::resetActionState(QAction *pAction, void *actionData)
{
	ACTION_DATA *pData = (ACTION_DATA*)actionData;

	pAction->setText(pData->text);
	pData->toolTip.isEmpty()? pAction->setToolTip(pData->text):pAction->setToolTip(pData->toolTip);
}

QAction* ActionMgr_Mgr::getActionById(ACTION_ID acId, const QString& id, const QVariant &var)
{
	if ( acId<ACTION_ID_BEGIN || acId>=ACTION_ID_BUTT )
		return NULL;

	QAction *pAction = m_Actions[acId];
	assert(pAction!=NULL);

	//如果获取Action绑定到新成员,需要更新
	udpateAction(pAction, acId, id, var);
	return pAction;
}

QAction* ActionMgr_Mgr::getActionById(GMEM_ACTION_ID acId, const QString& gid, const QString& uid)
{
	if (acId < GMEM_ACTION_ID_BEGIN || acId >= GMEM_ACT_ACTION_ID_BUTT)
		return NULL;

	QAction *pAction = m_Actions[acId];
	assert(pAction != NULL);

	//如果获取Action绑定到新成员,需要更新
	udpateAction(pAction, acId, gid, uid);
	return pAction;
}

void ActionMgr_Mgr::ReSetAllActionState()
{
	for(int i=ACTION_ID_BEGIN; i<ACTION_ID_BUTT; i++)
	{
		QAction *pAction = m_Actions[i];
		assert(pAction!=NULL);

		QString bindID = pAction->property(EXT_ID).toString();
		QVariant params = pAction->property(EXT_PARAMS);
		udpateAction(pAction, i, bindID, params);
	}
	for (int i = GMEM_ACTION_ID_BEGIN; i < GMEM_ACT_ACTION_ID_BUTT; i++)
	{
		QAction *pAction = m_Actions[i];
		assert(pAction != NULL);

		QString bindGID = pAction->property(EXT_GID).toString();
		QString bindUID = pAction->property(EXT_UID).toString();
		udpateAction(pAction, i, bindGID, bindUID);
	}
}

void ActionMgr_Mgr::udpateAction(QAction *pAction, int acId, const QString& uid, const QVariant &var)
{
	pAction->setProperty(EXT_ID, uid);
	pAction->setProperty(EXT_PARAMS, var);
	if (uid.isEmpty())
	{
		return;
	}

	switch (acId)
	{
	case ACT_START_Chat1v1:
	{	
		const CRIMFriendInfo* friendInfo = CRIMManager_Qt::instance()->getFriendInfo(uid);
		//同一企业或好友显示按钮
		bool bVisible = friendInfo != NULL;
		setActionV_E(pAction, bVisible, bVisible);
	}
		break;
	case ACT_START_ChatGrp:
	case ACT_SHOW_FriendInfo:
		setActionV_E(pAction, true, true);
		break;

	default:
		MainLogDebug("---UndealAction: %d, %s", acId, pAction->text().toUtf8().constData());
		break;
	}
}

void ActionMgr_Mgr::udpateAction(QAction *pAction, int acId, const QString& gid, const QString& uid)
{
	pAction->setProperty(EXT_GID, gid);
	pAction->setProperty(EXT_UID, uid);
	if (gid.isEmpty() || uid.isEmpty())
	{
		return;
	}

	switch (acId)
	{
	case GMEM_ACT_AT_USER:
	{
		const CRIMGrpMemberInfo* info = CRIMManager_Qt::instance()->getGrpMemberInfo(gid, uid);
		if (info == NULL)
		{
			setActionV_E(pAction, false, false);
			break;
		}
		pAction->setText(QString("@%1").arg(info->nickname));
	}
	case GMEM_ACT_TRANS_OWNER:
	case GMEM_ACT_SET_HOST:
	case GMEM_ACT_UNSET_HOST:
	case GMEM_ACT_KICK:
	case GMEM_ACT_SHOW_GrpMemberInfo:
		setActionV_E(pAction, true, true);
		break;

	default:
		MainLogDebug("---UndealAction: %d, %s", acId, pAction->text().toUtf8().constData());
		break;
	}
}


void ActionMgr_Mgr::setActionV_E(QAction *pAction, bool bVisiable, bool bEnable)
{
	pAction->setEnabled(bEnable);
	pAction->setVisible(bVisiable);
}
