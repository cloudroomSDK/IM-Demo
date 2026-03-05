#include "stdafx.h"
#include "GroupMemberList.h"
#include "CRIM_Manager.h"
#include "QSmoothScrollbar.h"
#include "QSuspendedScrollBar.h"
#include "ImGroupMenuCreator.h"

GroupHostItemWidget::GroupHostItemWidget(QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);
}

void GroupHostItemWidget::resizeEvent(QResizeEvent *event)
{
	QWidget::resizeEvent(event);
	//qDebug("GroupHostItemWidget size:%dx%d", this->width(), this->height());
}


GroupMemberList::GroupMemberList(QWidget *parent)
	: QListWidget(parent)
{
	connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_grpMemberInfoChanged, this, &GroupMemberList::slot_grpMemberInfoChanged);
	connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_grpMemberAdded, this, &GroupMemberList::slot_grpMemberAdded);
	connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_grpMemberDeleted, this, &GroupMemberList::slot_grpMemberDeleted);
	connect(this, &QListWidget::itemDoubleClicked, this, &GroupMemberList::slot_itemDoubleClicked);

	this->setIconSize(QSize(30, 30));
	this->setUniformItemSizes(true);
	//this->setContextMenuPolicy(Qt::CustomContextMenu); //自定义菜单
	this->setSortingEnabled(true);
	this->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
	this->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
	this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	//支持平滑滚动
	this->setVerticalScrollBar(new QSmoothScrollbar(parent));
	//支持浮动显示
	QSuspendedScrollBar *suspendedScrollBar = new QSuspendedScrollBar(Qt::Vertical, this);
}

GroupMemberList::~GroupMemberList()
{

}

void GroupMemberList::setGroupInfo(const CRIMGrpInfo_Ex *info)
{
	if (info == NULL)
	{
		return;
	}
	
	m_groupID = info->groupID;
	m_itemsForUserId.clear();
	m_managerIds.clear();
	this->clear();
	emit s_memberChanged(0, "", "");
	for (auto it = info->_members.begin(); it != info->_members.end(); it++)
	{
		addMember(it.value());
	}
}

void GroupMemberList::clearGroupInfo()
{
	m_groupID.clear();
	m_itemsForUserId.clear();
	m_managerIds.clear();
	this->clear();
}

QString GroupMemberList::getUserIDByNicknmae(const QString &nickName)
{
	for (auto it = m_itemsForUserId.begin(); it != m_itemsForUserId.end(); it++)
	{
		GrpMemberListItem* pItem = it.value();
		if (pItem->m_nickName == nickName)
		{
			return pItem->m_userID;
		}
	}

	return "";
}

void GroupMemberList::searchMember(const QString &str)
{
	for (auto it = m_itemsForUserId.begin(); it != m_itemsForUserId.end(); it++)
	{
		GrpMemberListItem* pItem = it.value();
		if (str.isEmpty() || pItem->m_nickName.contains(str, Qt::CaseInsensitive))
		{
			pItem->setHidden(false);
		}
		else
		{
			pItem->setHidden(true);
		}
	}
}

void GroupMemberList::addMember(const CRIMGrpMemberInfo &info)
{
	if (info.userID.isEmpty())
	{
		return;
	}
	if (m_itemsForUserId.contains(info.userID))
	{
		updateMember(info.userID);
		return;
	}
	GrpMemberListItem* pItem = new GrpMemberListItem();
	pItem->updateMemberInfo(info);
	ImIconMgr::instance()->bindIcon(pItem, IMIconInfo(info.userID));
	this->addItem(pItem);
 	m_itemsForUserId[info.userID] = pItem;
 	if (pItem->isGroupManager())
 	{
		m_managerIds.insert(info.userID);
 		updateHostWidget(pItem);
 	}
	
	emit s_memberChanged(1, info.userID, info.nickname);
}

void GroupMemberList::removeMember(const CRIMGrpMemberInfo &info)
{
	auto fItem = m_itemsForUserId.find(info.userID);
	if (fItem == m_itemsForUserId.end())
	{
		return;
	}

	GrpMemberListItem* pItem = fItem.value();
	QString name = pItem->m_nickName;

	int iRow = this->row(pItem);
	this->takeItem(iRow);
	delete pItem;
	pItem = NULL;

	m_itemsForUserId.remove(info.userID);
	m_managerIds.remove(info.userID);

	emit s_memberChanged(2, info.userID, name);
}

void GroupMemberList::updateMember(const QString &userId)
{
	auto fItem = m_itemsForUserId.find(userId);
	if ( fItem==m_itemsForUserId.end() )
	{
		return;
	}

	GrpMemberListItem* pItem = fItem.value();
	QString oldName = pItem->m_nickName;
	CRIMGroupMemberRole oldRole = pItem->m_roleLevel;

	const CRIMGrpMemberInfo* info = CRIMManager_Qt::instance()->getGrpMemberInfo(m_groupID, userId);
	pItem->updateMemberInfo(*info);
	updateHostWidget(pItem);

	if (pItem->isGroupManager())
	{
		m_managerIds.insert(userId);
	}
	else
	{
		m_managerIds.remove(userId);
	}

	//信息更新时，需要执行排序
	this->model()->sort(0);
	emit s_memberChanged(3, userId, info->nickname, oldName);
	if (userId == CRIMManager_Qt::instance()->getUserID() && oldRole!=info->roleLevel)
	{
		emit s_actorChanged();
	}
}

void GroupMemberList::updateHostWidget(GrpMemberListItem* pItem)
{
	if (pItem == NULL)
		return;

	QWidget *pWidget = this->itemWidget(pItem);
	if (pItem->isGroupManager())
	{
		pItem->setText("");
		GroupHostItemWidget *pItemWidget = static_cast<GroupHostItemWidget*>(pWidget);
		if (pItemWidget == NULL)
		{
			pItemWidget = new GroupHostItemWidget;
			this->setItemWidget(pItem, pItemWidget);
		}
		pItemWidget->ui.lbName->setText(pItem->m_nickName);
		pItemWidget->ui.lbMemberRole->setToolTip(pItem->m_roleLevel == CRIM_GMR_Owner ? tr("群主"): tr("管理员"));
		pItemWidget->ui.lbMemberRole->setObjectName(pItem->m_roleLevel == CRIM_GMR_Owner ? "lbMemberRole_owner" : "lbMemberRole_host");
		WidgetStyleUpdate(pItemWidget->ui.lbMemberRole);
	}
	else
	{
		if (pWidget != NULL)
		{
			this->setItemWidget(pItem, NULL);
			pItem->setText(pItem->m_nickName);
		}
	}
}

void GroupMemberList::slot_grpMemberInfoChanged(const CRIMGrpMemberInfo &info)
{
	if (info.groupID != m_groupID)
	{
		return;
	}
	updateMember(info.userID);
}

void GroupMemberList::slot_grpMemberAdded(const CRIMGrpMemberInfo &info)
{
	if (info.groupID != m_groupID)
	{
		return;
	}
	addMember(info);
}

void GroupMemberList::slot_grpMemberDeleted(const CRIMGrpMemberInfo &info)
{
	if (info.groupID != m_groupID)
	{
		return;
	}
	removeMember(info);
}

void GroupMemberList::contextMenuEvent(QContextMenuEvent * event)
{
	GrpMemberListItem *pItem = ToMemberListItem(this->itemAt(event->pos()));
	if (pItem == NULL)
		return;

	QMenu *menu = new QMenu(this);
	menu->setAttribute(Qt::WA_DeleteOnClose, true);
	IMGroupMenuCreator::FillGroupMemberLstMenu(menu, m_groupID, pItem->m_userID);
	menu->exec(event->globalPos());
}

void GroupMemberList::slot_itemDoubleClicked(QListWidgetItem *item)
{
	GrpMemberListItem *pItem = ToMemberListItem(item);
	if (pItem == NULL)
		return;

	g_atMgr.getActionById(ACT_START_Chat1v1, pItem->m_userID)->trigger();
}
