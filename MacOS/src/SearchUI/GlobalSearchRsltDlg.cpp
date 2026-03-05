#include "stdafx.h"
#include "GlobalSearchRsltDlg.h"
#include "ui_GlobalSearchRsltDlg.h"
#include "QGifLabel.h"
#include "FriendsMgr/FriendsMgrPage.h"
#include "MessageMgr/MessageMgrPage.h"
#include "MainDialog.h"
#include "GlobalSearch_defs.h"
#include "GlobalIMSearchDlg.h"


//////////////////////////////////////////////////////////////////////////
GlobalSearchRsltDlg::GlobalSearchRsltDlg(QLineEdit *edit, QWidget* parent)
	: QWidget(parent)
{
	ui = new Ui::GlobalSearchRsltDlg();
	ui->setupUi(this);
	this->setCursor(Qt::ArrowCursor);

	_searchEdit = edit;
	ui->listWidget->setFocusProxy(_searchEdit);
	_searchEdit->installEventFilter(this);

	_delayResearchTimer.setSingleShot(true);
	_delayResearchTimer.setInterval(500);
	connect(&_delayResearchTimer, &QTimer::timeout, this, &GlobalSearchRsltDlg::slot_research);

	ui->nulDesc->hide();
	ui->listWidget->setItemDelegate(new GlobalSearchRsltItemDelegate(ui->listWidget));
	initListWidget();
	connect(ui->listWidget, &QListWidget::itemClicked, this, &GlobalSearchRsltDlg::slot_itemClicked);

	connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_searchLocalMsgsSuccess, this, &GlobalSearchRsltDlg::slot_searchLocalMsgsSuccess);
	connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_searchLocalMsgsFailed, this, &GlobalSearchRsltDlg::slot_searchLocalMsgsFailed);
}

GlobalSearchRsltDlg::~GlobalSearchRsltDlg()
{
	_searchEdit->clear();
	ui->listWidget->setFocusProxy(NULL);
	delete ui;
}

void GlobalSearchRsltDlg::paintEvent(QPaintEvent *event)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}


void GlobalSearchRsltDlg::initListWidget()
{
	_friendsTitle = new GlobalSearchRsltItem(ui->listWidget, GlobalSearchRsltItem::FriendsTitle);
	_friendsTitle->setSizeHint(QSize(0, ITEMTitleHI));
	_friendsTitle->setFlags(0);
	_friendsTitle->_row1 = tr("好友");
	_friendsTitle->setText(_friendsTitle->_row1);
	_friendsTitle->setHidden(true);
	ui->listWidget->addItem(_friendsTitle);

	_friendsExpand = new GlobalSearchExpandItem(ui->listWidget, GlobalSearchRsltItem::FriendsExpand);
	_friendsExpand->setSizeHint(QSize(0, ITEMTitleHI));
	_friendsExpand->setHidden(true);
	ui->listWidget->addItem(_friendsExpand);

	_grpsTitle = new GlobalSearchRsltItem(ui->listWidget, GlobalSearchRsltItem::GroupsTitle);
	_grpsTitle->setSizeHint(QSize(0, ITEMTitleHI + 2));
	_grpsTitle->setFlags(0);
	_grpsTitle->_row1 = tr("群组");
	_grpsTitle->setText(_grpsTitle->_row1);
	_grpsTitle->setHidden(true);
	ui->listWidget->addItem(_grpsTitle);

	_grpsExpand = new GlobalSearchExpandItem(ui->listWidget, GlobalSearchRsltItem::GroupsExpand);
	_grpsExpand->setSizeHint(QSize(0, ITEMTitleHI));
	_grpsExpand->setHidden(true);
	ui->listWidget->addItem(_grpsExpand);

	_msgTitle = new GlobalSearchRsltItem(ui->listWidget, GlobalSearchRsltItem::MsgTitle);
	_msgTitle->setSizeHint(QSize(0, ITEMTitleHI + 2));
	_msgTitle->setFlags(0);
	_msgTitle->_row1 = tr("聊天记录");
	_msgTitle->setText(_msgTitle->_row1);
	_msgTitle->setHidden(true);
	ui->listWidget->addItem(_msgTitle);

	_msgLoading = new GlobalSearchRsltItem(ui->listWidget, GlobalSearchRsltItem::MsgLoading);
	_msgLoading->setSizeHint(QSize(0, ITEMHI));
	_msgLoading->setHidden(true);
	ui->listWidget->addItem(_msgLoading);
	QGifLabel* loading = new QGifLabel(ui->listWidget);
	loading->setObjectName("MsgLoadingLabel");
	ui->listWidget->setItemWidget(_msgLoading, loading);

	_msgExpand = new GlobalSearchExpandItem(ui->listWidget, GlobalSearchRsltItem::MsgExpand);
	_msgExpand->setSizeHint(QSize(0, ITEMTitleHI));
	_msgExpand->setHidden(true);
	ui->listWidget->addItem(_msgExpand);
}

bool GlobalSearchRsltDlg::eventFilter(QObject *watched, QEvent *event)
{
	if (event->type() == QEvent::KeyPress)
	{
		QKeyEvent *keyEv = static_cast<QKeyEvent*>(event);
		if (keyEv->key() == Qt::Key_Up)
		{
			selectNextItem(false);
			return true;
		}
		else if (keyEv->key() == Qt::Key_Down)
		{
			selectNextItem(true);
			return true;
		}
		else if (keyEv->key() == Qt::Key_Return)
		{
			QListWidgetItem *pItem = ui->listWidget->selectedItems().value(0);
			if (pItem != NULL)
			{
				slot_itemClicked(pItem);
			}
			return true;
		}
		else if (keyEv->key() == Qt::Key_Escape)
		{
			_searchEdit->clear();
			return true;
		}

	}

	return QWidget::eventFilter(watched, event);
}

void GlobalSearchRsltDlg::selectNextItem(bool bDown)
{
	int curSelRow = -1;
	auto selItems = ui->listWidget->selectedItems();
	if (selItems.size() > 0)
	{
		curSelRow = ui->listWidget->row(selItems.front());
	}

	if (bDown)
	{
		int nCount = ui->listWidget->count();
		for (int i = curSelRow + 1; i < nCount; i++)
		{
			GlobalSearchRsltItem* pItem = static_cast<GlobalSearchRsltItem*>(ui->listWidget->item(i));
			if (!pItem->isHidden() && (pItem->isDatItem() || pItem->isExpandItem()))
			{
				ui->listWidget->setItemSelected(pItem, true);
				ui->listWidget->scrollToItem(pItem);
				break;
			}
		}
	}
	else
	{
		for (int i = curSelRow - 1; i >= 0; i--)
		{
			GlobalSearchRsltItem* pItem = static_cast<GlobalSearchRsltItem*>(ui->listWidget->item(i));
			if (!pItem->isHidden() && (pItem->isDatItem() || pItem->isExpandItem()))
			{
				ui->listWidget->setItemSelected(pItem, true);
				ui->listWidget->scrollToItem(pItem);
				break;
			}
		}
	}

}


QString GlobalSearchRsltDlg::HighlighterText(const QString &str)
{
	static QString prefix("<span style=\"color:#2384C8;\">");
	static QString endfix("</span>");
	int pos = str.indexOf(_searchText, 0, Qt::CaseInsensitive);
	if (pos < 0)
	{
		return str;
	}
	int endPos = pos + _searchText.length();

	QString rslt(str);
	rslt.insert(endPos, endfix);
	rslt.insert(pos, prefix);
	return rslt;
}


void GlobalSearchRsltDlg::setSearchText(const QString &str)
{
	_searchText = str;
	_imSearchTime = int(QDateTime::currentMSecsSinceEpoch() / 1000);
	_delayResearchTimer.start();
}

void GlobalSearchRsltDlg::slot_research()
{
	ui->listWidget->setUpdatesEnabled(false);
	clearSearchRslt();

	//好友
	{
		QList<FriendsMgrPage::FriendSearchDat> frds = MainDialog::instance()->getFriendsMgrPage()->searchFriends(_searchText);
		_friendsTitle->setHidden(frds.size() <= 0);
		_friendsExpand->setHidden(frds.size() <= DefaultItemCount);
		_friendsExpand->_totalCount = frds.size();
		for (auto &dat : frds)
		{
			GlobalSearchRsltItem *pItem = new GlobalSearchRsltItem(ui->listWidget, GlobalSearchRsltItem::FriendsItem);
			pItem->setSizeHint(QSize(0, ITEMHI));
			pItem->_ID = dat._frd->userID;
			pItem->_row1 = HighlighterText(dat._frd->nickname);
			if (dat._matchType == FriendsMgrPage::CSMT_REMARK)
			{
				pItem->_matchIndex = dat._frd->remark.indexOf(_searchText, Qt::CaseInsensitive);
				pItem->_matchStrLen = dat._frd->remark.length();
				pItem->_row2 = tr("备注：") + HighlighterText(dat._frd->remark);
			}
			ui->listWidget->addItem(pItem);
		}
	}

	//群组
	{
		QList<GroupsMgrPage::GroupSearchDat> grps = MainDialog::instance()->getGroupsMgrPage()->searchGroups(_searchText);
		_grpsTitle->setHidden(grps.size() <= 0);
		_grpsExpand->setHidden(grps.size() <= DefaultItemCount);
		_grpsExpand->_totalCount = grps.size();
		for (auto &dat : grps)
		{
			GlobalSearchRsltItem *pItem = new GlobalSearchRsltItem(ui->listWidget, GlobalSearchRsltItem::GroupsItem);
			pItem->setSizeHint(QSize(0, ITEMHI));
			pItem->_ID = dat._grp->groupID;
			pItem->_row1 = HighlighterText(dat._grp->groupName);
			if (dat._meb != NULL)
			{
				pItem->_row2 = tr("包含：") + HighlighterText(dat._meb->nickname);
			}
			ui->listWidget->addItem(pItem);
		}
	}

	//聊天记录，(正在搜索时，不发起搜索请求）
	if (_msgLoading->isHidden())
	{
		searchIMHistory();
	}

	ui->listWidget->sortItems();
	itemExpandStateChanged(_friendsExpand);
	itemExpandStateChanged(_grpsExpand);

	ui->listWidget->setUpdatesEnabled(true);
}


void GlobalSearchRsltDlg::searchIMHistory()
{
	_msgTitle->setHidden(false);
	_msgLoading->setHidden(false);

	_imSearchRslt.totalCount = 0;
	_imSearchRslt.items.clear();

	QStringList keyList; keyList.push_back(_searchText);
	CRIMManager_Qt::instance()->searchLocalMsgs(QString(), keyList, 0, QStringList(), QList<CRIMMsgType>(), _imSearchTime, 0, 0, 0xFFFF, _searchText.toUtf8());
}


void GlobalSearchRsltDlg::slot_searchLocalMsgsSuccess(const CRIMSearchResultInfo &rslt, const QString &cookie)
{
	if (cookie != _searchText.toUtf8())
	{
		searchIMHistory();
		return;
	}

	CRIMMgrLogDebug("searchLocalMessage success, count:%d", rslt.totalCount);

	_imSearchRslt = rslt;
	_msgTitle->setHidden(rslt.totalCount <= 0);
	_msgLoading->setHidden(true);
	_msgExpand->_totalCount = _imSearchRslt.items.size();
	_msgExpand->setHidden(_msgExpand->_totalCount <= DefaultItemCount);

	for (auto &dat : _imSearchRslt.items)
	{
		const CRIMConversationInfo* pConv = MessageMgrPage::instance()->getConversationInfo(dat.conversationID);
		if ( pConv==NULL )
			continue;

		GlobalSearchExpandItem *pItem = new GlobalSearchExpandItem(ui->listWidget, GlobalSearchRsltItem::MsgItem);
		pItem->setSizeHint(QSize(0, ITEMHI));
		pItem->_ID = dat.conversationID;
		pItem->_row1 = pConv->showName();
		pItem->_row2 = tr("%1条相关聊天记录").arg(dat.messageCount);
		ui->listWidget->addItem(pItem);
	}
	ui->listWidget->sortItems();
	itemExpandStateChanged(_msgExpand);

	bool bEmpty = (_friendsExpand->_totalCount + _grpsExpand->_totalCount + _msgExpand->_totalCount) <= 0;
	ui->nulDesc->setVisible(bEmpty);
}

void GlobalSearchRsltDlg::slot_searchLocalMsgsFailed(int code, const QString &error, const QString &cookie)
{
	if (cookie != _searchText.toUtf8())
	{
		searchIMHistory();
		return;
	}

	CRIMMgrLogWarn("searchLocalMessage failed(%d,%s), searchText:%s", code, error.toUtf8().constData(), cookie.constData());
	CRIMSearchResultInfo rslt;
	slot_searchLocalMsgsSuccess(rslt, cookie);
}

void GlobalSearchRsltDlg::clearSearchRslt()
{
	for (int i = ui->listWidget->count()-1; i>=0; i--)
	{
		GlobalSearchRsltItem* pItem = static_cast<GlobalSearchRsltItem*>(ui->listWidget->item(i));
		if (!pItem->isFixedItem())
		{
			ui->listWidget->takeItem(i);
			delete pItem;
		}
	}
}

void GlobalSearchRsltDlg::slot_itemClicked(QListWidgetItem *p)
{
	GlobalSearchRsltItem* pItem = static_cast<GlobalSearchRsltItem*>(p);
	if (pItem->isExpandItem())
	{
		GlobalSearchExpandItem *pExpand = static_cast<GlobalSearchExpandItem*>(pItem);
		if (pItem->_itemType == GlobalSearchRsltItem::MsgExpand)
		{
			showImSearchDlg(QString());
		}
		else
		{
			pExpand->_bExpand = !pExpand->_bExpand;
			itemExpandStateChanged(pExpand);
		}
		return;
	}

	if (pItem->isDatItem())
	{
		GlobalSearchRsltItem *pDat = static_cast<GlobalSearchRsltItem*>(pItem);
		if (pItem->_itemType == GlobalSearchRsltItem::MsgItem)
		{
			showImSearchDlg(pDat->_ID);
		}
		else if (pDat->_itemType == GlobalSearchRsltItem::FriendsItem)
		{
			g_atMgr.getActionById(ACT_START_Chat1v1, pDat->_ID)->trigger();
			_searchEdit->clear();
		}
		else if (pDat->_itemType == GlobalSearchRsltItem::GroupsItem)
		{
			g_atMgr.getActionById(ACT_START_ChatGrp, pDat->_ID)->trigger();
			_searchEdit->clear();
		}
		return;
	}

}

void GlobalSearchRsltDlg::itemExpandStateChanged(GlobalSearchExpandItem *pExpand)
{
	if (pExpand->_bExpand)
	{
		pExpand->_row1 = tr("收起");
	}
	else
	{
		pExpand->_row1 = tr("全部显示(%1)").arg(pExpand->_totalCount);
	}
	pExpand->setHidden((pExpand->_totalCount <= DefaultItemCount));

	//找到本类型数据范围
	int startRow = -1;
	if (pExpand->_itemType == GlobalSearchRsltItem::FriendsExpand)
	{
		startRow = ui->listWidget->row(_friendsTitle) + 1;
	}
	else if (pExpand->_itemType == GlobalSearchRsltItem::GroupsExpand)
	{
		startRow = ui->listWidget->row(_grpsTitle) + 1;
	}
	else
	{
		startRow = ui->listWidget->row(_msgTitle) + 1;
	}
	int endRow = ui->listWidget->row(pExpand);

	//显示隐藏处理
	for (int i = startRow + DefaultItemCount; i < endRow; i++)
	{
		ui->listWidget->item(i)->setHidden(!pExpand->_bExpand);
	}
}


void GlobalSearchRsltDlg::showImSearchDlg(const QString &conversationID)
{
	GlobalIMSearchDlg* pDlg = GlobalIMSearchDlg::instance();
	pDlg->setContext(_searchText, _imSearchTime, _imSearchRslt, conversationID);
	pDlg->show();

	_searchEdit->clear();
}
