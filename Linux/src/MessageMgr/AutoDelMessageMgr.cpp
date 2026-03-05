#include "stdafx.h"
#include "AutoDelMessageMgr.h"

AutoDelMessageMgr* AutoDelMessageMgr::getInstance()
{
	static AutoDelMessageMgr ins;
	return &ins;
}

AutoDelMessageMgr::AutoDelMessageMgr() : QObject()
{
	connect(&m_delTimer, &QTimer::timeout, this, &AutoDelMessageMgr::slot_timeout);
}

AutoDelMessageMgr::~AutoDelMessageMgr()
{

}

void AutoDelMessageMgr::burnMsg(const QList<DelInfo> &infos)
{
	for (auto &info : infos)
	{
		burnMsg(info);
	}
}

void AutoDelMessageMgr::burnMsg(const DelInfo &info)
{
	QList<DelInfo> &infoList = m_delInfoMap[info.conversationID];
	for (auto &tmpInfo : infoList)
	{
		//已经存在不添加
		if (tmpInfo.msgId == info.msgId)
		{
			return;
		}
	}
	//立即删除
	if (info.duration <= 0)
	{
		//删除本地消息+服务器消息
		CRIMManager_Qt::instance()->deleteMsg(info.conversationID, info.msgId);
		return;
	}
	infoList.append(info);
	if (!m_delTimer.isActive())
	{
		m_delTimer.start(1000);
	}
}

void AutoDelMessageMgr::cancelByConversationId(const QString &conversationID)
{
	if (!m_delInfoMap.contains(conversationID))
	{
		return;
	}
	m_delInfoMap.remove(conversationID);
}

void AutoDelMessageMgr::cancelByMsgId(const QString &conversationID, const QString &msgId)
{
	if (!m_delInfoMap.contains(conversationID))
	{
		return;
	}
	QList<DelInfo> &infos = m_delInfoMap[conversationID];
	for (auto it = infos.begin(); it != infos.end(); it++)
	{
		DelInfo &info = *it;
		if (info.msgId == msgId)
		{
			infos.erase(it);
			break;
		}
	}
}

QList<AutoDelMessageMgr::DelInfo> AutoDelMessageMgr::getInfosByConversation(const QString &conversationID)
{
	return m_delInfoMap.value(conversationID);
}

void AutoDelMessageMgr::slot_timeout()
{
	for (auto it = m_delInfoMap.begin(); it != m_delInfoMap.end();)
	{
		QList<DelInfo> &infos = it.value();
		for (auto itInfo = infos.begin(); itInfo != infos.end(); )
		{
			DelInfo &info = *itInfo;
			info.duration--;
			if (info.duration <= 0)
			{
				//删除本地消息+服务器消息
				CRIMManager_Qt::instance()->deleteMsg(info.conversationID, info.msgId);
				itInfo = infos.erase(itInfo);
			}
			else
			{
				itInfo++;
			}
		}
		if (infos.size() <=0)
		{
			it = m_delInfoMap.erase(it);
		}
		else
		{
			it++;
		}
	}
	//通知进度，用于界面刷新倒计时
	emit s_msgsDurationProgress();
	if (m_delInfoMap.size() <= 0 && m_delTimer.isActive())
	{
		m_delTimer.stop();
	}
}