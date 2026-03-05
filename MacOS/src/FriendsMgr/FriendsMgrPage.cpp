#include "stdafx.h"
#include "FriendsMgrPage.h"
#include "FriendItemWidget.h"
#include "AddFriendDlg.h"
#include "MainDialog.h"
#include "QSuspendedScrollBar.h"
#include "UserInfoPage.h"

FriendsMgrPage::FriendsMgrPage(QWidget *parent)
	: QFrame(parent)
{						   
	ui.setupUi(this);
	connect(ui.searchEdt, &CRLineEdit::textChanged, MainDialog::instance(), &MainDialog::slot_searchTextChanged);

	connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_syncServerFinish, this, &FriendsMgrPage::slot_syncServerFinish);
	connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_friendAdded, this, &FriendsMgrPage::slot_friendAdded);
	connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_friendDeleted, this, &FriendsMgrPage::slot_friendDeleted);
	connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_friendInfoChanged, this, &FriendsMgrPage::slot_friendAdded);

	connect(g_atMgr.getActionById(ACT_SearchFor_AddFriend), &QAction::triggered, this, &FriendsMgrPage::slot_addFriendTriggered);
	connect(g_atMgr.getActionById(ACT_NEW_Friend), &QAction::triggered, this, &FriendsMgrPage::slot_newFriendTriggered);
	connect(g_atMgr.getActionById(ACT_Del_Friend), &QAction::triggered, this, &FriendsMgrPage::slot_delFriendTriggered);
	connect(g_atMgr.getActionById(ACT_SHOW_FriendInfo, QString()), &QAction::triggered, this, &FriendsMgrPage::slot_showFriendInfoTriggered);
	connect(g_atMgr.getActionById(ACT_CFG_Friend, QString()), &QAction::triggered, this, &FriendsMgrPage::slot_cfgFriendTriggered);
	connect(ui.friendsList, &QListWidget::itemClicked, this, &FriendsMgrPage::slot_itemClicked);
	connect(ui.friendsList, &QListWidget::currentItemChanged, this, &FriendsMgrPage::slot_currentItemChanged);
	connect(ui.newMsgPage, &NewFriendMsgPage::s_firendReqMsgChanged, this, &FriendsMgrPage::slot_firendReqMsgChanged);

	//右则滚动条浮动显示
	ui.friendsList->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
	ui.friendsList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	QSuspendedScrollBar *suspendedScrollBar = new QSuspendedScrollBar(Qt::Vertical, ui.friendsList);

	initList();
}

FriendsMgrPage::~FriendsMgrPage()
{

}

void FriendsMgrPage::initList()
{
	CRIMFriendInfo tmp;
	//添加好友
	tmp.userID = g_strAddFriend;
	addFriendItem(tmp);
	//新的好友
	tmp.userID = g_strNewFriendMsg;
	addFriendItem(tmp);

	ui.defaultPage->show();
	ui.detailPage->hide();
	ui.newMsgPage->hide();

	slot_syncServerFinish();
}


void FriendsMgrPage::slot_syncServerFinish()
{
	for (int i = ui.friendsList->count() - 1; i > 1; i--)
	{
		QListWidgetItem *pItem = ui.friendsList->takeItem(i);
		delete pItem;
	}

	//好在列表
	const QHash<QString, CRIMFriendInfo>& friendList = CRIMManager_Qt::instance()->getFriendList();
	for (auto it = friendList.begin(); it != friendList.end(); it++)
	{
		const CRIMFriendInfo &info = it.value();
		addFriendItem(info);
	}
	ui.friendsList->sortItems();
}

FriendListItem* FriendsMgrPage::getItemByUserID(const QString& userID)
{
	for (int i = 0; i < ui.friendsList->count(); i++)
	{
		FriendListItem* pItem = static_cast<FriendListItem*>(ui.friendsList->item(i));
		if (pItem->getUserID() == userID)
			return pItem;
	}
	return nullptr;
}

int FriendsMgrPage::getRowByUserID(const QString& userID)
{
	for (int i = 0; i < ui.friendsList->count(); i++)
	{
		FriendListItem* pItem = static_cast<FriendListItem*>(ui.friendsList->item(i));
		if (pItem->getUserID() == userID)
			return i;
	}
	return -1;
}



void FriendsMgrPage::slot_friendAdded(const CRIMFriendInfo &info)
{
	FriendListItem* pItem = getItemByUserID(info.userID);
	if (pItem == nullptr)
	{
		addFriendItem(info);
	}
	else
	{
		updateFriendItem(pItem, info);
	}
	ui.friendsList->sortItems();
}

void FriendsMgrPage::slot_friendDeleted(const CRIMFriendInfo &info)
{
	int row = getRowByUserID(info.userID);
	if (row<0)
		return;

	QListWidgetItem *pItem = ui.friendsList->takeItem(row);
	delete pItem;
}

void FriendsMgrPage::addFriendItem(const CRIMFriendInfo &info)
{
	//列表item
	FriendListItem* pItem = new FriendListItem(ui.friendsList);
	ui.friendsList->addItem(pItem);
	FriendItemWidget *pItemWidget = new FriendItemWidget(pItem);
	ui.friendsList->setItemWidget(pItem, pItemWidget);

	pItem->setItemDat(&info);
	pItemWidget->updateUI();
}

void FriendsMgrPage::updateFriendItem(FriendListItem* pItem, const CRIMFriendInfo &info)
{
	pItem->setItemDat(&info);

	FriendItemWidget *pItemWidget = static_cast<FriendItemWidget*>(ui.friendsList->itemWidget(pItem));
	if (pItemWidget != nullptr)
	{
		pItemWidget->updateUI();
	}
}

void FriendsMgrPage::slot_firendReqMsgChanged(bool hasFriendReqMsg)
{
	emit s_firendReqMsgChanged(hasFriendReqMsg);

	FriendListItem* pItem = getItemByUserID(g_strNewFriendMsg);
	if (pItem == nullptr)
		return;
	FriendItemWidget *pItemWidget = static_cast<FriendItemWidget*>(ui.friendsList->itemWidget(pItem));
	if (pItemWidget == nullptr)
		return;
	pItemWidget->setNewDatRedTipVisible(hasFriendReqMsg);
}

void FriendsMgrPage::slot_itemClicked(QListWidgetItem *item)
{
	FriendListItem* pItem = static_cast<FriendListItem*>(item);
	if (pItem->getUserID() == g_strAddFriend)
	{
		ui.defaultPage->show();
		ui.newMsgPage->hide();
		ui.detailPage->hide();

		slot_addFriendTriggered();
		return;
	}
}

void FriendsMgrPage::slot_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
	if (current == nullptr)
	{
		ui.defaultPage->show();
		ui.newMsgPage->hide();
		ui.detailPage->hide();
		return;
	}

	FriendListItem* pItem = static_cast<FriendListItem*>(current);
	if (pItem->getUserID() == g_strAddFriend)
	{
		ui.defaultPage->show();
		ui.newMsgPage->hide();
		ui.detailPage->hide();
		return;
	}
	if (pItem->getUserID() == g_strNewFriendMsg)
	{
		slot_newFriendTriggered();
		return;
	}

	ui.detailPage->setFriendInfo(pItem->getUserID());
	ui.defaultPage->hide();
	ui.newMsgPage->hide();
	ui.detailPage->show();

}

void FriendsMgrPage::slot_newFriendTriggered()
{
	ui.defaultPage->hide();
	ui.detailPage->hide();
	ui.newMsgPage->show();
}

void FriendsMgrPage::slot_addFriendTriggered()
{
	AddFriendDlg *dlgAddFriend = new AddFriendDlg(MainDialog::instance());
	dlgAddFriend->setAttribute(Qt::WA_DeleteOnClose);
	dlgAddFriend->exec();
}


void FriendsMgrPage::slot_delFriendTriggered()
{
	QAction *pAction = dynamic_cast<QAction*>(sender());
	if (pAction == nullptr)
		return;
	QString userID = pAction->property(EXT_ID).toString();
	if (userID.isEmpty())
		return;

	CRIMManager_Qt::instance()->deleteFriend(userID);
}

void FriendsMgrPage::slot_showFriendInfoTriggered()
{
	QAction *pAction = dynamic_cast<QAction*>(sender());
	if (pAction == nullptr)
		return;
	QString usrID = pAction->property(EXT_ID).toString();
	if (usrID.isEmpty())
		return;

	UserInfoPage *pDlg = new UserInfoPage(MainDialog::instance());
	pDlg->setAttribute(Qt::WA_DeleteOnClose);
	pDlg->show(usrID, QString(), QCursor::pos());
}

void FriendsMgrPage::slot_cfgFriendTriggered()
{
	QAction *pAction = dynamic_cast<QAction*>(sender());
	if (pAction == nullptr)
		return;
	QString usrID = pAction->property(EXT_ID).toString();
	if (usrID.isEmpty())
		return;

	QNoNCDialog *pDlg = new QNoNCDialog(MainDialog::instance());
	pDlg->setAttribute(Qt::WA_DeleteOnClose);
	pDlg->setObjectName("FriendDetailDlg");
	StandardTitleBar *pTitleBar = new StandardTitleBar(pDlg, StandardTitleBar::TBTN_CLOSE);
	pDlg->setTitleBar(pTitleBar);
	FriendDetailWidget *pDetail = new FriendDetailWidget(pDlg);
	pDetail->setFriendInfo(usrID);
	pDetail->setHideChatBtn(true);
	pDlg->setClientWidget(pDetail);
	connect(pDetail, &FriendDetailWidget::s_clearMsgClicked, pDlg, &QNoNCDialog::close);
	connect(pDetail, &FriendDetailWidget::s_delFriendClicked, pDlg, &QNoNCDialog::close);
	connect(pDetail, &FriendDetailWidget::s_videoCallClicked, pDlg, &QNoNCDialog::close);
	connect(pDetail, &FriendDetailWidget::s_audioCallClicked, pDlg, &QNoNCDialog::close);
	pDlg->exec();
}

QList<FriendsMgrPage::FriendSearchDat> FriendsMgrPage::searchFriends(const QString &str)
{
	QList<FriendsMgrPage::FriendSearchDat> rslt;
	const QHash<QString, CRIMFriendInfo>& fList = CRIMManager_Qt::instance()->getFriendList();
	for (auto &it : fList)
	{
		FriendsMgrPage::FriendSearchDat dat;
		if (it.nickname.contains(str, Qt::CaseInsensitive))
		{
			dat._frd = &it;
			dat._matchType = CSMT_NAME;
		}
		else if (it.remark.contains(str, Qt::CaseInsensitive))
		{
			dat._frd = &it;
			dat._matchType = CSMT_REMARK;
		}
		else
		{
			continue;
		}
		rslt.push_back(dat);
	}

	return rslt;
}
