#include "stdafx.h"
#include "GroupsMgrPage.h"
#include "DlgNewGroup.h"
#include "GroupItemWidget.h"
#include "MainDialog.h"
#include "QSuspendedScrollBar.h"
#include "UserInfoPage.h"

GroupsMgrPage::GroupsMgrPage(QWidget *parent)
	: QFrame(parent)
{
	ui.setupUi(this);
	connect(ui.searchEdt, &CRLineEdit::textChanged, MainDialog::instance(), &MainDialog::slot_searchTextChanged);
	connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_syncServerFinish, this, &GroupsMgrPage::slot_syncServerFinish);
	connect(g_atMgr.getActionById(ACT_NEW_GrpChat), &QAction::triggered, this, &GroupsMgrPage::slot_newGroupClicked);
	connect(g_atMgr.getActionById(ACT_CFG_Grp), &QAction::triggered, this, &GroupsMgrPage::slot_cfgGrp);
	connect(g_atMgr.getActionById(GMEM_ACT_SHOW_GrpMemberInfo, QString(), QString()), &QAction::triggered, this, &GroupsMgrPage::slot_showGrpMemberInfo);
	connect(ui.groupList, &QListWidget::itemClicked, this, &GroupsMgrPage::slot_itemClicked);
	connect(ui.groupList, &QListWidget::currentItemChanged, this, &GroupsMgrPage::slot_currentItemChanged);

	connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_joinedGrpAdded, this, &GroupsMgrPage::slot_joinedGrpAdded);
	connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_grpInfoChanged, this, &GroupsMgrPage::slot_grpInfoChanged);
	connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_joinedGrpDeleted, this, &GroupsMgrPage::slot_joinedGrpDeleted);
	connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_joinedGrpDismissed, this, &GroupsMgrPage::slot_joinedGrpDeleted);

	//右则滚动条浮动显示
	ui.groupList->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
	ui.groupList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	QSuspendedScrollBar *suspendedScrollBar = new QSuspendedScrollBar(Qt::Vertical, ui.groupList);

	slot_syncServerFinish();
}

GroupsMgrPage::~GroupsMgrPage()
{

}

void GroupsMgrPage::slot_syncServerFinish()
{
	ui.defaultPage->show();
	ui.detailPage->hide();

	ui.groupList->setUpdatesEnabled(false);
	ui.groupList->clear();

	//新建群
	CRIMGrpInfo_Ex tmpGInfo;
	tmpGInfo.groupID = g_createNewGrp;
	addGroupItem(&tmpGInfo);

	//群列表
	const QMap<QString, CRIMGrpInfo_Ex>& groupList = CRIMManager_Qt::instance()->getJoinedGrpList();
	for (auto it = groupList.begin(); it != groupList.end(); it++)
	{
		const CRIMGrpInfo_Ex &info = it.value();
		addGroupItem(&info);
	}
	ui.groupList->sortItems();
	ui.groupList->setUpdatesEnabled(true);
}

void GroupsMgrPage::addGroupItem(const CRIMGrpInfo_Ex *info)
{
	//列表item
	GroupListItem* pItem = new GroupListItem(info->groupID, ui.groupList);
	ui.groupList->addItem((QListWidgetItem*)pItem);
	GroupItemWidget *pItemWidget = new GroupItemWidget(pItem);
	ui.groupList->setItemWidget(pItem, pItemWidget);
}

void GroupsMgrPage::slot_itemClicked(QListWidgetItem *item)
{
	GroupListItem* pItem = static_cast<GroupListItem*>(item);
	if (pItem->getGrpID() == g_createNewGrp)
	{
		slot_newGroupClicked();
	}
}

void GroupsMgrPage::slot_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
	GroupListItem* pItem = static_cast<GroupListItem*>(current);
	if (pItem != nullptr && pItem->getGrpID() == g_createNewGrp)
	{
		pItem = nullptr;
	}

	bool bDetail = (pItem != nullptr);
	ui.detailPage->setVisible(bDetail);
	ui.defaultPage->setVisible(!bDetail);
	if (bDetail)
	{
		ui.detailPage->setGroupInfo(pItem->getGrpID());
	}
}


void GroupsMgrPage::slot_newGroupClicked()
{
	QSet<QString> initUserIDs;
	QAction *pAction = dynamic_cast<QAction*>(sender());
	if (pAction != nullptr)
	{
		QString exUserID = pAction->property(EXT_ID).toString();
		if (!exUserID.isEmpty())
		{
			initUserIDs.insert(exUserID);
		}
	}

	DlgNewGroup *dlgNewGroup = new DlgNewGroup(initUserIDs, this);
	dlgNewGroup->setAttribute(Qt::WA_DeleteOnClose);
	dlgNewGroup->exec();
}

GroupListItem* GroupsMgrPage::getItemByGrpID(const QString &grpID)
{
	for (int i = 0; i < ui.groupList->count(); i++)
	{
		QListWidgetItem *pItem = ui.groupList->item(i);
		GroupListItem* grpItem = static_cast<GroupListItem*>(pItem);
		if (grpItem->getGrpID() == grpID)
		{
			return grpItem;
		}
	}

	return nullptr;
}

void GroupsMgrPage::slot_joinedGrpAdded(const CRIMGrpInfo &info)
{
	const CRIMGrpInfo_Ex *grpInfo = CRIMManager_Qt::instance()->getJoinedGrpInfo(info.groupID);
	if (grpInfo == nullptr)
		return;
	addGroupItem(grpInfo);
	ui.groupList->sortItems();
}


void GroupsMgrPage::slot_grpInfoChanged(const CRIMGrpInfo &info)
{
	GroupListItem* pItem = getItemByGrpID(info.groupID);
	if (pItem == nullptr)
		return;

	QWidget *pItemWidget = ui.groupList->itemWidget(pItem);
	GroupItemWidget *grpItemWidget = static_cast<GroupItemWidget*>(pItemWidget);
	if (grpItemWidget == nullptr)
		return;

	grpItemWidget->updateUI();
}

void GroupsMgrPage::slot_joinedGrpDeleted(const CRIMGrpInfo &info)
{
	GroupListItem* pItem = getItemByGrpID(info.groupID);
	if (pItem == nullptr)
		return;

	ui.groupList->removeItemWidget(pItem);
	ui.groupList->takeItem(ui.groupList->row(pItem));
	delete pItem;
}

void GroupsMgrPage::slot_showGrpMemberInfo()
{
	QAction *pAction = dynamic_cast<QAction*>(sender());
	if (pAction == nullptr)
		return;
	QString usrID = pAction->property(EXT_UID).toString();
	if (usrID.isEmpty())
		return;
	QString grpID = pAction->property(EXT_GID).toString();

	UserInfoPage *pDlg = new UserInfoPage(MainDialog::instance());
	pDlg->setAttribute(Qt::WA_DeleteOnClose);
	pDlg->show(usrID, grpID, QCursor::pos());
}

void GroupsMgrPage::slot_cfgGrp()
{
	QAction *pAction = dynamic_cast<QAction*>(sender());
	if (pAction == nullptr)
		return;
	QString grpID = pAction->property(EXT_ID).toString();
	if (grpID.isEmpty())
		return;

	QNoNCDialog *pDlg = new QNoNCDialog(MainDialog::instance());
	pDlg->setAttribute(Qt::WA_DeleteOnClose);
	pDlg->setObjectName("GroupDetailDlg");
	StandardTitleBar *pTitleBar = new StandardTitleBar(pDlg, StandardTitleBar::TBTN_CLOSE);
	pDlg->setTitleBar(pTitleBar);
	GroupDetailWidget *pDetail = new GroupDetailWidget(pDlg);
	pDetail->ui.btnGroupChat->hide();
	pDetail->setGroupInfo(grpID);
	pDlg->setClientWidget(pDetail);
	connect(pDetail, &GroupDetailWidget::s_btnClearGroupClicked, pDlg, &QNoNCDialog::close);
	connect(pDetail, &GroupDetailWidget::s_btnDismissGroupClicked, pDlg, &QNoNCDialog::close);
	connect(pDetail, &GroupDetailWidget::s_btnQuitGroupClicked, pDlg, &QNoNCDialog::close);

	pDlg->exec();
}


QList<GroupsMgrPage::GroupSearchDat> GroupsMgrPage::searchGroups(const QString &str)
{
	QList<GroupSearchDat> rslt;
	const QMap<QString, CRIMGrpInfo_Ex>& grps = CRIMManager_Qt::instance()->getJoinedGrpList();
	for (auto &grp : grps)
	{
		if (grp.groupName.contains(str, Qt::CaseInsensitive))
		{
			GroupSearchDat dat;
			dat._grp = &grp;
			dat._meb = NULL;
			rslt.push_back(dat);
			continue;
		}

		for (auto &mb : grp._members)
		{
			if (mb.nickname.contains(str, Qt::CaseInsensitive))
			{
				GroupSearchDat dat;
				dat._grp = &grp;
				dat._meb = &mb;
				rslt.push_back(dat);
				break;
			}
		}
	}

	return rslt;
}


