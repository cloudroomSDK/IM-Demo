#include "stdafx.h"
#include "SessionListWidget.h"
#include "SessionItemWidget.h"
#include "ConversationBaseWidget.h"
#include "MessageMgrPage.h"
#include "QSmoothScrollbar.h"
#include "QSuspendedScrollBar.h"

SessionListWidget::SessionListWidget(QWidget *parent)
	: QListWidget(parent)
{
	m_currentItem = NULL;
	m_mgrPage = NULL;
	connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_syncServerFinish, this, &SessionListWidget::slot_syncServerFinish);
	connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_getAllConversationListSuccess, this, &SessionListWidget::slot_getAllConversationListSuccess);
	connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_getAllConversationListFailed, this, &SessionListWidget::slot_getAllConversationListFailed);
	connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_getOneConversationSuccess, this, &SessionListWidget::slot_getOneConversationSuccess);
	connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_getOneConversationFailed, this, &SessionListWidget::slot_getOneConversationFailed);
	connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_deleteConversationAndDeleteAllMsgSuccess, this, &SessionListWidget::slot_deleteConversationAndDeleteAllMsgSuccess);
	connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_newConversation, this, &SessionListWidget::slot_newConversation);
	connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_conversationChanged, this, &SessionListWidget::slot_conversationChanged);
	connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_friendInfoChanged2, this, &SessionListWidget::slot_friendInfoChanged2);
	connect(this, &QListWidget::itemClicked, this, &SessionListWidget::slot_itemClicked);

	this->setSortingEnabled(true);
	this->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
	this->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
	this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	//支持平滑滚动
	this->setVerticalScrollBar(new QSmoothScrollbar(parent));
	//支持浮动显示
	QSuspendedScrollBar *suspendedScrollBar = new QSuspendedScrollBar(Qt::Vertical, this);
	slot_syncServerFinish();
}

SessionListWidget::~SessionListWidget()
{
}
//登录成功
void SessionListWidget::slot_syncServerFinish()
{
	MainLogDebug("getAllConversationList...");
	//获取会话记录
	CRIMManager_Qt::instance()->getAllConversationList();
}

QList<const CRIMConversationInfo*> SessionListWidget::getConversationList(bool bOnlyUnread)
{
	QList<const CRIMConversationInfo*> infoList;
	for (auto &item : m_itemsForId.values())
	{
		const CRIMConversationInfo &info = item->getInfo();
		//过滤未读计数为0的数据
		if (info.unreadCount == 0 && bOnlyUnread)
		{
			continue;
		}
		infoList.append(&info);
	}
	return infoList;
}

const CRIMConversationInfo* SessionListWidget::getConversationInfo(const QString &conversationID)
{
	auto pos = m_itemsForId.find(conversationID);
	if (pos == m_itemsForId.end())
		return NULL;

	return &(pos.value()->getInfo());
}


void SessionListWidget::switchConversation(const QString &id, CRIMConversationType type)
{
	CRIMManager_Qt::instance()->getOneConversation(id, type);
}

void SessionListWidget::switchConversation(const QString &conversationId)
{
	if (!m_itemsForId.contains(conversationId))
	{
		return;
	}
	setCurrentConversation(conversationId);
}

void SessionListWidget::jumpNextUnreadConversation()
{
	int startIndex = 0;
	if (m_currentItem!=nullptr)
	{
		startIndex = row(m_currentItem);
	}

	//在当前项后找
	for (int i = startIndex; i < this->count(); i++)
	{
		SessionListItem* pItem = static_cast<SessionListItem*>(this->item(i));
		if (pItem != NULL && pItem->getInfo().unreadCount > 0)
		{
			scrollToItem(pItem, QListWidget::PositionAtTop);
			switchConversation(pItem->getInfo().conversationID);
			return;
		}
	}

	//在当前项前找
	for (int i = 0; i < startIndex; i++)
	{
		SessionListItem* pItem = static_cast<SessionListItem*>(this->item(i));
		if (pItem != NULL && pItem->getInfo().unreadCount > 0)
		{
			scrollToItem(pItem, QListWidget::PositionAtTop);
			switchConversation(pItem->getInfo().conversationID);
			return;
		}
	}

}

void SessionListWidget::clearMsg(const QString &conversationId)
{
	auto pos = m_itemsForId.find(conversationId);
	if (pos == m_itemsForId.end())
		return;
	SessionListItem *pItem = pos.value();
	if (pItem == nullptr)
		return;

	pItem->clearMsg();
}

void SessionListWidget::clearAllUnreadCount(const QStringList &conversationIds)
{
	//清除所有未读计数
	if (conversationIds.size() == 0)
	{
		for (auto &item : m_itemsForId.values())
		{
			item->clearUnreadCount();
		}
		return;
	}

	for (auto &id : conversationIds)
	{
		auto findIt = m_itemsForId.find(id);
		if (findIt == m_itemsForId.end())
		{
			continue;
		}

		SessionListItem *item = findIt.value();
		if (item != NULL)
		{
			item->clearUnreadCount();
		}
	}
}

void SessionListWidget::slot_widgetItemChanged(QWidget *pWidget, QListWidgetItem *item)
{
	if (pWidget == NULL)
		return;

	SessionItemWidget* pItemWidget = static_cast<SessionItemWidget*>(pWidget);
	if (item == NULL)
	{
		pItemWidget->clear();
	}
	else
	{
		SessionListItem* pItem = static_cast<SessionListItem*>(item);
		pItemWidget->setInfo(pItem->getInfo());
	}
}

//新增一个会话
void SessionListWidget::addConversation(const CRIMConversationInfo &info)
{
	//过滤无效的类型
	if (info.conversationType == CRIM_Conversation_Undefine)
	{
		return;
	}
	if (m_itemsForId.contains(info.conversationID))
	{
		this->updateConversation(info);
		return;
	}
	//列表item
	SessionListItem* pItem = new SessionListItem(info, this);
	this->addItem((QListWidgetItem*)pItem);
	SessionItemWidget *pItemWidget = new SessionItemWidget;
	connect(pItemWidget, &SessionItemWidget::s_delConversation, this, &SessionListWidget::slot_delOneConversation);
	this->setItemWidget(pItem, pItemWidget);
	slot_widgetItemChanged(pItemWidget, pItem);
	m_itemsForId[info.conversationID] = pItem;
}

//更新一个会话
void SessionListWidget::updateConversation(const CRIMConversationInfo &info)
{
	auto findIt = m_itemsForId.find(info.conversationID);
	if (findIt == m_itemsForId.end())
	{
		return;
	}
	SessionListItem *pItem = findIt.value();
	pItem->updateInfo(info);
	slot_widgetItemChanged(itemWidget(pItem), pItem);
}

//删除一个会话
void SessionListWidget::removeConversation(const QString &conversationID)
{
	auto findIt = m_itemsForId.find(conversationID);
	if (findIt == m_itemsForId.end())
	{
		return;
	}
	SessionListItem *item = findIt.value();
	//删除列表中元素
	int rowIndex = this->row(item);
	delete this->takeItem(rowIndex);
	if (m_currentItem == item)
	{
		clearSelection();
	}
	m_itemsForId.erase(findIt);

	auto sendIt = m_sendTextForId.find(conversationID);
	if (sendIt != m_sendTextForId.end())
	{
		m_sendTextForId.erase(sendIt);
	}
}

void SessionListWidget::clearSelection()
{
	if (m_currentItem != NULL)
	{
		m_currentItem = NULL;
		this->setCurrentItem(NULL);
	}
	emit s_setCurrentConversation();
}

//清空
void SessionListWidget::clearAll()
{
	clearSelection();
	m_itemsForId.clear();
	m_sendTextForId.clear();
	this->clear();
}

void SessionListWidget::setCurrentConversation(const QString &conversationID)
{
	auto findIt = m_itemsForId.find(conversationID);
	if (findIt == m_itemsForId.end())
	{
		return;
	}
	SessionListItem *pItem = findIt.value();
	slot_itemClicked(pItem);
}

// 删除会话, 可刷新会话列表
void SessionListWidget::slot_delOneConversation(const QString &conversationId)
{
	//删除sdk缓存
	CRIMManager_Qt::instance()->deleteConversationAndDeleteAllMsg(conversationId);

	//删除ui内容
	slot_deleteConversationAndDeleteAllMsgSuccess(conversationId);
	
}
// 删除会话成功（可能从其他地方删除
void SessionListWidget::slot_deleteConversationAndDeleteAllMsgSuccess(const QString &conversationID)
{
	//删除ui内容
	removeConversation(conversationID);
	//通知外部会话列表改变
	emit MessageMgrPage::instance()->s_conversationListChanged();
}

void SessionListWidget::slot_getAllConversationListSuccess(const QList<CRIMConversationInfo> &conversationInfos, const QString &cookie)
{
	MainLogDebug("getAllConversationList...success! (count:%d)", conversationInfos.size());

	clearAll();
	for (auto &info : conversationInfos)
	{
		addConversation(info);
	}
	//通知外部会话列表改变
	emit MessageMgrPage::instance()->s_conversationListChanged();
}

void SessionListWidget::slot_getAllConversationListFailed(int code, const QString &error, const QString &cookie)
{
	MainLogDebug("getAllConversationList...failed! (%d,%s)", code, error.toUtf8().constData());
	CRMsgBox::information(this, tr("提示"), tr("获取会话列表失败"));
}

void SessionListWidget::slot_getOneConversationSuccess(const CRIMConversationInfo &info, const QString &cookie)
{
	//如果没有找到，创建一个临时的会话，并且切换到该会话
	if (!m_itemsForId.contains(info.conversationID))
	{
		//修改时间最后一条消息发送时间，保证该会话在最上面一个
		CRIMConversationInfo newInfo = info;
		newInfo.latestMsgSendTime = QDateTime::currentMSecsSinceEpoch();
		addConversation(newInfo);
		//通知外部会话列表改变
		emit MessageMgrPage::instance()->s_conversationListChanged();
	}

	setCurrentConversation(info.conversationID);
}

void SessionListWidget::slot_getOneConversationFailed(int code, const QString &error, const QString &cookie)
{
	CRMsgBox::information(this, tr("提示"), tr("进入会话失败"));
}

// 新增会话, 可刷新会话列表
void SessionListWidget::slot_newConversation(const QList<CRIMConversationInfo> &list)
{
	for (auto &info : list)
	{
		addConversation(info);
	}
	//通知外部会话列表改变
	emit MessageMgrPage::instance()->s_conversationListChanged();
}

void SessionListWidget::slot_friendInfoChanged2(const CRIMFriendInfo &oldInfo, const CRIMFriendInfo &newInfo)
{
	//只需对昵称进行处理
	if (oldInfo.getDisplayName() == newInfo.getDisplayName())
		return;

	SessionListItem *pItem = getConversationItemBySourceId(oldInfo.userID, CRIM_Conversation_1v1);
	if (pItem == NULL)
		return;

	slot_widgetItemChanged(itemWidget(pItem), pItem);
}

// 会话列表有数据更新, 可刷新会话列表
void SessionListWidget::slot_conversationChanged(const QList<CRIMConversationInfo> &list)
{
	for (auto &info : list)
	{
		updateConversation(info);
	}
	//信息更新时，需要执行排序
	this->model()->sort(0);
	//通知外部会话列表改变
	emit MessageMgrPage::instance()->s_conversationListChanged();
}

void SessionListWidget::slot_itemClicked(QListWidgetItem *item)
{
	if (m_currentItem == item)
	{
		return;
	}

	SessionListItem* pItem = static_cast<SessionListItem*>(item);
	SessionListItem* pPreviousItem = static_cast<SessionListItem*>(m_currentItem);
	QString conId = pItem->getInfo().conversationID;

	//进入新的页面时，需要将分割器位置同步
	QList<int> splitterSizes;
	if (pPreviousItem != NULL)
	{
		QString prevConId = pPreviousItem->getInfo().conversationID;
		m_sendTextForId[prevConId] = pPreviousItem->getTextFragment();
		splitterSizes = pPreviousItem->getSplitterSize();
		pPreviousItem->leaveConversation();
		
	}
	//进入会话
	if (m_mgrPage != NULL)
	{
		pItem->enterConversation(m_mgrPage->getConversationWidget(pItem->getInfo().conversationType));
	}
	//同步会话状态
	if (m_sendTextForId.contains(conId))
	{
		pItem->setTextFragment(m_sendTextForId[conId]);
	}
	if (splitterSizes.size() > 0)
	{ 
		pItem->setSplitterSize(splitterSizes);
	}
	item->setSelected(true);
	m_currentItem = item;

	emit s_setCurrentConversation(pItem->getInfo());
}

const CRIMConversationInfo* SessionListWidget::getConversationInfoBySourceId(const QString &id, CRIMConversationType type)
{
	SessionListItem *pItem = getConversationItemBySourceId(id, type);
	if (pItem == NULL)
		return NULL;

	return &(pItem->getInfo());
}

SessionListItem* SessionListWidget::getConversationItemBySourceId(const QString &id, CRIMConversationType type)
{
	for (auto it = m_itemsForId.begin(); it != m_itemsForId.end(); it++)
	{
		SessionListItem *item = it.value();
		if (type == CRIM_Conversation_1v1 && item->getInfo().userID == id)
		{
			return item;
		}
		else if (type == CRIM_Conversation_Group && item->getInfo().groupID == id)
		{
			return item;
		}
	}
	return NULL;
}
