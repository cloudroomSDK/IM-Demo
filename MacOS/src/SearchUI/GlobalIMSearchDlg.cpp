#include "stdafx.h"
#include "GlobalIMSearchDlg.h"
#include "ui_GlobalIMSearchDlg.h"
#include "GlobalSearch_defs.h"
#include "MessageMgr/MessageMgrPage.h"
#include "MainDialog.h"

GlobalIMSearchDlg* GlobalIMSearchDlg::_ins = NULL;

GlobalIMSearchDlg* GlobalIMSearchDlg::instance()
{
	if (_ins == NULL)
	{
		_ins = new GlobalIMSearchDlg(MainDialog::instance());
	}
	return _ins;
}


GlobalIMSearchDlg::GlobalIMSearchDlg(QWidget *parent) : QNoNCDialog(parent)
{
	StandardTitleBar *pTitleBar = new StandardTitleBar(this, StandardTitleBar::TBTN_CLOSE | StandardTitleBar::TBTN_MAX);
	this->setTitleBar(pTitleBar);

	QWidget *pClient = new QWidget(this);
	ui = new Ui::GlobalIMSearchDlg();
	ui->setupUi(pClient);
	this->setClientWidget(pClient);
	this->setAttribute(Qt::WA_DeleteOnClose);

	ui->stackedWidget->setCurrentWidget(ui->nullPage);
	ui->msgList->setType(IMMsgWidget::MSGS_SEARCH);
	ui->msgHis->setType(IMMsgWidget::MSGS_HIS);
	connect(g_atMgr.getActionById(ACT_IM_VIEWCONTEXT), &QAction::triggered, this, &GlobalIMSearchDlg::slot_imViewContext);
	connect(ui->enterButton, &QPushButton::clicked, this, &GlobalIMSearchDlg::slot_enterButtonClicked);
	connect(ui->hisBackBtn, &QPushButton::clicked, this, &GlobalIMSearchDlg::slot_backBtnClicked);

	ui->searchEdit->setLeftIconVisible(true);
	ui->listWidget->setItemDelegate(new GlobalSearchRsltItemDelegate(ui->listWidget));
	connect(ui->listWidget, &QListWidget::itemSelectionChanged, this, &GlobalIMSearchDlg::slot_itemSelectionChanged);

	_delayResearchTimer.setSingleShot(true);
	_delayResearchTimer.setInterval(500);
	connect(&_delayResearchTimer, &QTimer::timeout, this, &GlobalIMSearchDlg::slot_research);
	connect(ui->searchEdit, &QLineEdit::textChanged, this, &GlobalIMSearchDlg::slot_textChanged);
	connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_searchLocalMsgsSuccess, this, &GlobalIMSearchDlg::slot_searchLocalMsgsSuccess);
	connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_searchLocalMsgsFailed, this, &GlobalIMSearchDlg::slot_searchLocalMsgsFailed);
	this->setMinimumSize(750, 600);
}

GlobalIMSearchDlg::~GlobalIMSearchDlg()
{
	_ins = NULL;
	delete ui;
}

void GlobalIMSearchDlg::setContext(const QString &searchTxt, int searchTime, const CRIMSearchResultInfo &info, const QString &conversationID)
{
	_searchText = searchTxt;
	_searchTime = searchTime;
	ui->searchEdit->setText(searchTxt);
	fillDatToList(info, conversationID);
}

void GlobalIMSearchDlg::fillDatToList(const CRIMSearchResultInfo &rslt, const QString &curConversationID)
{
	ui->listWidget->clear();

	for (auto &dat : rslt.items)
	{
		const CRIMConversationInfo* pConv = MessageMgrPage::instance()->getConversationInfo(dat.conversationID);
		if (pConv == NULL)
			continue;

		GlobalSearchConvsationItem *pItem = new GlobalSearchConvsationItem(ui->listWidget);
		pItem->setSizeHint(QSize(0, ITEMHI));
		pItem->_ID = dat.conversationID;
		pItem->_row1 = pConv->showName();
		pItem->_row2 = tr("%1条相关聊天记录").arg(dat.messageCount);
		pItem->_rslt = dat;
		ui->listWidget->addItem(pItem);

		if (curConversationID.length() > 0 && curConversationID == dat.conversationID)
		{
			ui->listWidget->setItemSelected(pItem, true);
		}
	}
}

void GlobalIMSearchDlg::slot_textChanged(const QString &txt)
{
	if (txt == _searchText)
		return;

	ui->listWidget->clear();
	_searchText = txt;
	_searchTime = int(QDateTime::currentMSecsSinceEpoch() / 1000);
	_delayResearchTimer.start();
}

void GlobalIMSearchDlg::slot_research()
{
	ui->listWidget->clear();

	QStringList keyList; keyList.push_back(_searchText);

	CRIMManager_Qt::instance()->searchLocalMsgs(QString(), keyList, 0, QStringList(), QList<CRIMMsgType>(), _searchTime, 0, 0, 0xFFFF,  _searchText.toUtf8());
}


void GlobalIMSearchDlg::slot_searchLocalMsgsSuccess(const CRIMSearchResultInfo &rslt, const QString &cookie)
{
	if (cookie != _searchText.toUtf8())
	{
		return;
	}

	CRIMMgrLogDebug("searchLocalMessage success, count:%d", rslt.totalCount);
	fillDatToList(rslt, QString());
}

void GlobalIMSearchDlg::slot_searchLocalMsgsFailed(int code, const QString &error, const QString &cookie)
{
	CRIMMgrLogWarn("searchLocalMessage failed(%d,%s), searchText:%s", code, error.toUtf8().constData(), cookie.constData());
}


void GlobalIMSearchDlg::slot_itemSelectionChanged()
{
	GlobalSearchConvsationItem *pItem = static_cast<GlobalSearchConvsationItem*>(ui->listWidget->selectedItems().value(0));
	if (pItem != NULL)
	{
		const CRIMConversationInfo* pConv = MessageMgrPage::instance()->getConversationInfo(pItem->_ID);
		if (pConv != NULL)
		{
			ui->convsationName->setText(pConv->showName());
			ui->stackedWidget->setCurrentWidget(ui->msgListPage);
			ui->msgList->init(pItem, _searchText, _searchTime);
		}
		return;
	}

	ui->stackedWidget->setCurrentWidget(ui->nullPage);
}

void GlobalIMSearchDlg::slot_imViewContext()
{
	QAction *act = static_cast<QAction*>(sender());
	if (act == NULL)
	{
		return;
	}

	UIMsgInfo msgInfo = act->property(EXT_PARAMS).value<UIMsgInfo>();
	if (msgInfo.clientMsgID.isEmpty())
	{
		return;
	}

	GlobalSearchConvsationItem *pItem = static_cast<GlobalSearchConvsationItem*>(ui->listWidget->selectedItems().value(0));
	if (pItem == NULL)
	{
		return;
	}
	const CRIMConversationInfo* pConv = MessageMgrPage::instance()->getConversationInfo(pItem->_ID);
	if (pConv == NULL)
	{
		return;
	}

	ui->stackedWidget->setCurrentWidget(ui->msgHisPage);
	ui->msgHis->setConversationInfo(*pConv);
	ui->msgHis->loadHistroyMsgs(msgInfo, _searchText);
}

void GlobalIMSearchDlg::slot_backBtnClicked()
{
	ui->stackedWidget->setCurrentWidget(ui->msgListPage);
}

void GlobalIMSearchDlg::slot_enterButtonClicked()
{
	GlobalSearchConvsationItem *pItem = static_cast<GlobalSearchConvsationItem*>(ui->listWidget->selectedItems().value(0));
	if (pItem == NULL)
	{
		return;
	}
	const CRIMConversationInfo* pConv = MessageMgrPage::instance()->getConversationInfo(pItem->_ID);
	if (pConv == NULL)
	{
		return;
	}
	if (pConv->conversationType == CRIM_Conversation_1v1)
	{
		g_atMgr.getActionById(ACT_START_Chat1v1, pConv->userID)->trigger();
	}
	else
	{
		g_atMgr.getActionById(ACT_START_ChatGrp, pConv->groupID)->trigger();
	}
	this->close();
}
