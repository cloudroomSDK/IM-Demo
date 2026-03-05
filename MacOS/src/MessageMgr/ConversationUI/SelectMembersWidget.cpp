#include "stdafx.h"
#include "SelectMembersWidget.h"

#define PATHID_ROOT QString("-1") //path路径根节点
#define PATHID_FRIEND QString("-2") //path路径好友节点

class CMItem_SMW : public ListWidgetItemWithIMIcon
{
public:
	CMItem_SMW(QListWidget *parent = Q_NULLPTR) : ListWidgetItemWithIMIcon(parent) {}
	~CMItem_SMW() {}

	QString		m_usrID;
	QByteArray	m_gbkName;
	
	void setGbkName(const QString &name)
	{
		m_gbkName = QTextCodec::codecForName("gbk")->fromUnicode(name.toLower());
	}
	virtual bool operator<(const QListWidgetItem &other) const
	{
		const CMItem_SMW* item2 = static_cast<const CMItem_SMW*>(&other);
		return m_gbkName < item2->m_gbkName;
	}
};

SelectMembersWidget::SelectMembersWidget(QWidget *parent)
	: QFrame(parent)
{
	ui.setupUi(this);
	
	ui.listWidget->setIconSize(QSize(29, 29));
	ui.selListWidget->setIconSize(QSize(29, 29));
	ui.selListWidget->setSortingEnabled(true);

	connect(ui.listWidget, &QListWidget::itemClicked, this, &SelectMembersWidget::slot_itemClicked);
	updateConnnection(false);
}

SelectMembersWidget::~SelectMembersWidget()
{

}

void SelectMembersWidget::updateConnnection(bool bDisconnect)
{
	if (bDisconnect)
	{
		disconnect(ui.listWidget, &QListWidget::itemChanged, this, &SelectMembersWidget::slot_itemChanged);
		disconnect(ui.checkAll, &QCheckBox::toggled, this, &SelectMembersWidget::slot_checkAllToggled);
	}
	else
	{
		connect(ui.listWidget, &QListWidget::itemChanged, this, &SelectMembersWidget::slot_itemChanged);
		connect(ui.checkAll, &QCheckBox::toggled, this, &SelectMembersWidget::slot_checkAllToggled);
	}
}

void SelectMembersWidget::init(const SelectCfg &cfg)
{
	//加载整个通讯录
	m_cfg = cfg;
	if (m_cfg.groupID.isEmpty())
	{
		fillFriendDat();
	}
	else
	{
		fillGroupMemberDat(m_cfg.groupID);
	}
}

void SelectMembersWidget::fillFriendDat(const QStringList &containsIds)
{
	ui.listWidget->clear();
	const QHash<QString, CRIMFriendInfo>& frineds = CRIMManager_Qt::instance()->getFriendList();
	for (auto it = frineds.begin(); it != frineds.end(); it++)
	{
		QString id = it.key();
		CRIMFriendInfo v = it.value();
		if (!containsIds.isEmpty() && !containsIds.contains(it.key()))
		{
			continue;
		}
		addUserToList(&it.value());
	}
	ui.listWidget->sortItems();
	updateCheckStates();
}

void SelectMembersWidget::fillGroupMemberDat(const QString &gId, const QStringList &containsIds)
{
	ui.listWidget->clear();
	const QMap<QString, CRIMGrpMemberInfo>& gMembers = CRIMManager_Qt::instance()->getGrpMembersInfo(gId);
	for (auto it = gMembers.begin(); it != gMembers.end(); it++)
	{
		if (!containsIds.isEmpty() && !containsIds.contains(it.key()))
		{
			continue;
		}
		addGroupUserToList(&it.value());
	}
	ui.listWidget->sortItems();
	updateCheckStates();
}

bool SelectMembersWidget::isSelected(const QString &userId)
{
	if (m_cfg.selectedIds.contains(userId))
		return true;
	return m_selItems.contains(userId);
}

bool SelectMembersWidget::isSelected(const QSet<QString> &userIds)
{
	bool bSel = userIds.size() > 0;
	for (auto &id : userIds)
	{
		if (!m_selItems.contains(id))
		{
			bSel = false;
			break;
		}
	}
	return bSel;
}

void SelectMembersWidget::addUserToList(const CRIMFriendInfo *pInfo)
{
	CMItem_SMW *pItem = new CMItem_SMW(/*ui.listWidget*/);
	pItem->m_usrID = pInfo->userID;
	pItem->setGbkName(pInfo->getDisplayName());
	bool bDisableOpr = m_cfg.disableIds.contains(pInfo->userID) || m_cfg.selectedIds.contains(pInfo->userID);
	pItem->setFlags(bDisableOpr ? Qt::ItemIsSelectable : (Qt::ItemIsSelectable | Qt::ItemIsEnabled));
	ImIconMgr::instance()->bindIcon(pItem, IMIconInfo(pInfo->userID, IMICON_USR, ui.listWidget->iconSize()));
	pItem->setText(pInfo->getDisplayName());
	pItem->setToolTip(pItem->text());
	ui.listWidget->addItem(pItem);

	pItem->setCheckState(isSelected(pInfo->userID) ? Qt::Checked : Qt::Unchecked);
}

void SelectMembersWidget::addGroupUserToList(const CRIMGrpMemberInfo *pInfo)
{
	CMItem_SMW *pItem = new CMItem_SMW(/*ui.listWidget*/);
	pItem->m_usrID = pInfo->userID;
	pItem->setGbkName(pInfo->nickname);
	pItem->setCheckState(isSelected(pInfo->userID) ? Qt::Checked : Qt::Unchecked);
	bool bDisableOpr = m_cfg.disableIds.contains(pInfo->userID) || m_cfg.selectedIds.contains(pInfo->userID);
	pItem->setFlags(bDisableOpr ? Qt::ItemIsSelectable : (Qt::ItemIsSelectable | Qt::ItemIsEnabled));
	ImIconMgr::instance()->bindIcon(pItem, IMIconInfo(pInfo->userID, IMICON_USR, ui.listWidget->iconSize()));
	pItem->setText(pInfo->nickname);
	pItem->setToolTip(pItem->text());
	ui.listWidget->addItem(pItem);
}

void SelectMembersWidget::slot_itemClicked(QListWidgetItem *item)
{
  	CMItem_SMW *pItem = static_cast<CMItem_SMW*>(item);
	//禁用了item
	if ((pItem->flags() & Qt::ItemIsEnabled) == 0)
	{
		return;
	}
	if (!pItem->data(Qt::CheckStateRole).isNull())
	{
		Qt::CheckState s = pItem->checkState();
        pItem->setCheckState(s == Qt::Checked ? Qt::Unchecked : Qt::Checked);
	}
}

void SelectMembersWidget::slot_checkAllToggled(bool checked)
{
	for (int i = 0; i < ui.listWidget->count(); i++)
	{
		QListWidgetItem *pItem = ui.listWidget->item(i);
		//禁用了item
		if ((pItem->flags() & Qt::ItemIsEnabled) == 0)
		{
			continue;
		}
		pItem->setCheckState(checked ? Qt::Checked : Qt::Unchecked);
	}
}

void SelectMembersWidget::slot_itemChanged(QListWidgetItem *item)
{
	CMItem_SMW *pItem = static_cast<CMItem_SMW*>(item);
	QSet<QString> users;
	users.insert(pItem->m_usrID);
	if (Qt::Checked == pItem->checkState())		//选中时
	{
		addUserToSelList(users);
	}
	else if (Qt::Unchecked == pItem->checkState())
	{
		removeUserFromSelList(users);
	}
}

void SelectMembersWidget::addUserToSelList(const QSet<QString> &users)
{
	bool bMaxCount = false;
	for (auto &id : users)
	{
		if (m_selItems.contains(id))
		{
			continue;
		}
		if (m_cfg.disableIds.contains(id))
		{
			continue;
		}
		//超过了最大选择限制
		if (m_selItems.size() >= m_cfg.maxSelect)
		{
			bMaxCount = true;
			break;
		}
		CMItem_SMW *pItem = new CMItem_SMW;
		pItem->m_usrID = id;
		SelRsltWidget_SMW *w = new SelRsltWidget_SMW(id, ui.selListWidget);
		connect(w, &SelRsltWidget_SMW::s_delUsr, this, &SelectMembersWidget::slot_delSelUser);
		QString faceUrl;
		QString nickName;
		if (!m_cfg.groupID.isEmpty())
		{
			const CRIMGrpMemberInfo* pInfo = CRIMManager_Qt::instance()->getGrpMemberInfo(m_cfg.groupID, id);
			nickName = pInfo->getDisplayName();
			faceUrl = pInfo->faceURL;
		}
		else
		{
			const CRIMFriendInfo *pInfo = CRIMManager_Qt::instance()->getFriendInfo(id);
			nickName = pInfo->getDisplayName();
			faceUrl = pInfo->faceURL;
		}
		ImIconMgr::instance()->bindIcon(w->ui.lbFace, IMIconInfo(id, IMICON_USR, ui.listWidget->iconSize()));
		w->ui.lbName->setText(nickName);
		w->ui.lbName->setToolTip(w->ui.lbName->text());
		pItem->setGbkName(nickName);
		if (m_cfg.selectedIds.contains(id))
		{
			w->ui.btnDel->hide();
		}

		ui.selListWidget->addItem(pItem);
		ui.selListWidget->setItemWidget(pItem, w);
		m_selItems[id] = pItem;
	}
	ui.lbSelCount->setText(QString("(%1/%2)").arg(m_selItems.size()).arg(m_cfg.maxSelect));
	updateCheckStates();
	if (bMaxCount)
	{
		CRMsgBox::information(this, tr("提示"), tr("已超过最大数量限制"));
	}
}

void SelectMembersWidget::slot_delSelUser(const QString &users)
{
	removeUserFromSelList(QSet<QString>() << users);
}

void SelectMembersWidget::removeUserFromSelList(const QSet<QString> &users)
{
	for (auto &id : users)
	{
		auto findIt = m_selItems.find(id);
		if (findIt == m_selItems.end())
		{
			continue;
		}

		QListWidgetItem *pItem = findIt.value();
		int row = ui.selListWidget->row(pItem);
		delete ui.selListWidget->takeItem(row);
		m_selItems.erase(findIt);
	}
	ui.lbSelCount->setText(QString("(%1/%2)").arg(m_selItems.size()).arg(m_cfg.maxSelect));
	updateCheckStates();
}

void SelectMembersWidget::updateCheckStates()
{
	updateConnnection(true);

	bool bCheckAll = ui.listWidget->count() > 0;
	int disableItemCount = 0;
	for (int i = 0; i < ui.listWidget->count(); i++)
	{
		CMItem_SMW *pItem = static_cast<CMItem_SMW*>(ui.listWidget->item(i));
		//禁用了item
		if ((pItem->flags() & Qt::ItemIsEnabled) == 0)
		{
			disableItemCount++;
			continue;
		}
		Qt::CheckState state = m_selItems.contains(pItem->m_usrID) ? Qt::Checked : Qt::Unchecked;
		pItem->setCheckState(state);
		if (state == Qt::Unchecked)
		{
			bCheckAll = false;
		}
	}
	//如果全部禁用
	if (disableItemCount == ui.listWidget->count())
	{
		bCheckAll = false;
	}
	ui.checkAll->setChecked(bCheckAll);
	updateConnnection(false);
}
