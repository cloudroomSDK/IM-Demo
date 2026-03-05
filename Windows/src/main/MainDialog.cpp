#include "stdafx.h"
#include "MainDialog.h"
#include "LoginDlg.h"
#include "AboutDlg.h"
#include "App.h"
#include "UserInfoPage.h"
#include "GlobalSearchRsltDlg.h"
#include "MeetingMainDialog.h"
#include "UserStatusMgr.h"
#include "ImCallMgr.h"

MainDialog* MainDialog::s_pMainDialog = nullptr;
MainDialog* MainDialog::instance()
{
	if (s_pMainDialog == nullptr)
	{
		s_pMainDialog = new MainDialog(NULL);
	}
	return s_pMainDialog;
}

MainDialog::MainDialog(QWidget *parent)
	: QNoNCDialog(parent)
{
	s_pMainDialog = this;
	ImIconMgr::instance()->init(App::instance()->getAcntDatPath());

 	QWidget *pClient = new QWidget(this);
	ui.setupUi(pClient);

	this->cfgCloseType(false);
	this->setClientWidget(pClient);
	this->setWindowTitle(App::instance()->getUsrLoginAcnt());

	//消息未读提示
	m_lbMsgUnreadCount = new QClickedLabel(ui.messageBtn);
	m_lbMsgUnreadCount->setAlignment(Qt::AlignCenter);
	m_lbMsgUnreadCount->setObjectName("lbMsgUnreadCount");
	m_lbMsgUnreadCount->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	addSubWidgetFor(ui.messageBtn, m_lbMsgUnreadCount, Qt::AlignRight | Qt::AlignTop);
	connect(m_lbMsgUnreadCount, &QClickedLabel::doubleClick, this, &MainDialog::slot_showNextMsgSession);

	//新好友提示器
	QWidget* newContactTip = addSubWidgetFor(ui.friendsBtn, "NewDatRedTip", Qt::AlignRight | Qt::AlignTop);
	newContactTip->hide();
	connect(ui.friendsPage, &FriendsMgrPage::s_firendReqMsgChanged, newContactTip, &QWidget::setVisible);
	connect(ui.myselBtn, &QAbstractButton::clicked, this, &MainDialog::slot_myselBtnClicked);
	connect(ui.messageBtn, &QAbstractButton::clicked, this, &MainDialog::slot_messageBtnClicked);
	connect(ui.friendsBtn, &QAbstractButton::clicked, this, &MainDialog::slot_friendsBtnClicked);
	connect(ui.groupBtn, &QAbstractButton::clicked, this, &MainDialog::slot_groupBtnClicked);
	connect(ui.moreBtn, &QAbstractButton::clicked, this, &MainDialog::slot_moreBtnClicked);
	connect(g_atMgr.getActionById(ACT_START_Chat1v1, QString()), &QAction::triggered, this, &MainDialog::slot_start_chat1v1_triggered);
	connect(g_atMgr.getActionById(ACT_START_ChatGrp, QString()), &QAction::triggered, this, &MainDialog::slot_start_chatGrp_triggered);
	connect(App::instance()->getTrayIcon(), &CSystemTrayIcon::s_showNextMsgSession, this, &MainDialog::slot_showNextMsgSession);
	connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_getTotalUnreadMsgCountSuccess, this, &MainDialog::slot_getTotalUnreadMsgCountSuccess);
	connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_totalUnreadMsgCountChanged, this, &MainDialog::slot_totalUnreadMsgCountChanged);

	m_reszHandler->setEnableActions(QWidgetResizeHandlerI::ALL_ACTIONS);
	ImIconMgr::instance()->bindIcon(ui.myselBtn, IMIconInfo(CRIMManager_Qt::instance()->getUserID()));
	this->resize(1200, 800);
	
	//呼叫模块
	m_callMgr = new ImCallMgr(this);
}

MainDialog::~MainDialog()
{
	s_pMainDialog = nullptr;
	ImIconMgr::instance()->uninit();
}


FriendsMgrPage* MainDialog::getFriendsMgrPage()
{
	return ui.friendsPage;
}

GroupsMgrPage* MainDialog::getGroupsMgrPage()
{
	return ui.groupPage;
}

void MainDialog::showOnLoginSuccess()
{
	//获取未读总数
	slot_totalUnreadMsgCountChanged(0);
	CRIMManager_Qt::instance()->getTotalUnreadMsgCount();
	//进入消息页
	ui.messageBtn->setChecked(true);	

	this->show();
}

void MainDialog::slot_myselBtnClicked()
{
	QMap<QString, QStringList> findParams;
	CRIMManager_Qt::instance()->findMsgList(findParams);

	UserInfoPage *pDlg = new UserInfoPage(this);
	pDlg->setAttribute(Qt::WA_DeleteOnClose);
	pDlg->show(CRIMManager_Qt::instance()->getUserID(), QString(), QCursor::pos());
}


void MainDialog::slot_messageBtnClicked()
{
	ui.mainUIStacked->setCurrentWidget(ui.msgPage);
}

void MainDialog::slot_friendsBtnClicked()
{
	ui.mainUIStacked->setCurrentWidget(ui.friendsPage);
}

void MainDialog::slot_groupBtnClicked()
{
	ui.mainUIStacked->setCurrentWidget(ui.groupPage);
}

void MainDialog::slot_moreBtnClicked()
{
	QMenu *moreMenu = new QMenu(this);
	moreMenu->setObjectName("mainMoreMenu");

// 	QAction *settingAction = moreMenu->addAction(tr("常规设置"));
// 	connect(settingAction, &QAction::triggered, this, &MainDialog::slot_settingActionTrigered);

	QAction *aboutLab = moreMenu->addAction(tr("关于"));
	connect(aboutLab, &QAction::triggered, this, &MainDialog::slot_aboutActionTrigered);
	QAction *signOutAction = moreMenu->addAction(tr("登出"));
	connect(signOutAction, &QAction::triggered, this, &MainDialog::slot_signOutAccount);

	moreMenu->addSeparator();
	QAction *quitAction = moreMenu->addAction(tr("退出"));
	connect(quitAction, &QAction::triggered, App::instance(), &App::slot_exitApp);

	moreMenu->setAttribute(Qt::WA_DeleteOnClose);

	//位置调整
	QPoint menuPos(ui.moreBtn->width(), ui.moreBtn->height());
	menuPos = ui.moreBtn->mapToGlobal(menuPos);
	moreMenu->show();
	menuPos.setY(menuPos.y() - moreMenu->height());
	moreMenu->move(menuPos);
}

void MainDialog::slot_settingActionTrigered()
{
}

void MainDialog::slot_aboutActionTrigered()
{
	AboutDlg *pDlg = new AboutDlg(this);
	pDlg->setAttribute(Qt::WA_DeleteOnClose);
	pDlg->show();
}

void MainDialog::slot_signOutAccount()
{
	LoginDlg::instance()->signOut();
}

void MainDialog::slot_start_chat1v1_triggered()
{
	QAction *pAct = dynamic_cast<QAction*>(sender());
	if (pAct == NULL)
		return;

	QString userID = pAct->property(EXT_ID).toString();
	//如果是自己，不进入私聊
	if (userID == CRIMManager_Qt::instance()->getUserID())
	{
		return;
	}
	switch2Conversation(userID, CRIM_Conversation_1v1);
}

void MainDialog::slot_start_chatGrp_triggered()
{
	QAction *pAct = dynamic_cast<QAction*>(sender());
	if (pAct == NULL)
		return;

	QString grpID = pAct->property(EXT_ID).toString();
	switch2Conversation(grpID, CRIM_Conversation_Group);
}

void MainDialog::slot_showNextMsgSession()
{
	ui.msgPage->jumpNextUnreadConversation();
	ShowAndActiveWindow(this);
}


void MainDialog::switch2Conversation(const QString &uId, CRIMConversationType type)
{
	if (ui.msgPage != ui.mainUIStacked->currentWidget())
	{
		ui.messageBtn->click();
	}

	ui.msgPage->switchConversation(uId, type);
}

void MainDialog::slot_searchTextChanged(const QString &txt)
{
	if (txt.isEmpty())
	{
		m_searchRsltGuard.reset();
		return;
	}

	CRLineEdit *pLineEdit = dynamic_cast<CRLineEdit*>(sender());
	if (pLineEdit == nullptr)
	{
		return;
	}

	if (m_searchRsltGuard.isNull())
	{
		GlobalSearchRsltDlg* pDlg = new GlobalSearchRsltDlg(pLineEdit, this);
		m_searchRsltGuard.reset(pDlg);
	}
	m_searchRsltGuard->setSearchText(txt);
	m_searchRsltGuard->setFixedWidth(400);
	m_searchRsltGuard->setFixedHeight(this->height() - 150);
	m_searchRsltGuard->show();
	slot_updateSearchRsltDlgPos();
	//QTimer::singleShot(10, pDlg, SLOT(show()));
}


void MainDialog::slot_updateSearchRsltDlgPos()
{
	if (m_searchRsltGuard.isNull())
		return;

	QWidget *pEdit = m_searchRsltGuard->curEdit();
	if (pEdit == nullptr)
		return;

	int leftPos = -5; //qproperty-layoutMargin:14 - border-width:9
	QPoint scrnPos = pEdit->mapTo(this, QPoint(leftPos, pEdit->height()));
	m_searchRsltGuard->move(scrnPos);
	m_searchRsltGuard->raise();
}

void MainDialog::resizeEvent(QResizeEvent *e)
{
	QNoNCDialog::resizeEvent(e);
	QTimer::singleShot(10, this, SLOT(slot_updateSearchRsltDlgPos()));
}

void MainDialog::slot_getTotalUnreadMsgCountSuccess(int count, const QString &cookie)
{
	slot_totalUnreadMsgCountChanged(count);
}

void MainDialog::slot_totalUnreadMsgCountChanged(int count)
{
	//未读总数处理
	QString strCount = count > 99 ? QString("99+") : QString::number(count);
	m_lbMsgUnreadCount->setText(strCount);
	m_lbMsgUnreadCount->setVisible(count > 0);
#ifdef MAC
	//设置角标
	if (count == 0)
	{
		strCount = "";
	}
    //setAppBadge(strCount);
#endif

	if (count > 0)
	{
		App::instance()->getTrayIcon()->startFlashTray();
		this->startFlashMainDlg();
	}
	else
	{
		App::instance()->getTrayIcon()->stopFlashTray();
		this->stopFlashMainDlg();
	}
}

void MainDialog::startFlashMainDlg()
{
	//如果主窗口处理激活状态，不再窗口闪烁；
	if (QApplication::activeWindow() != NULL)
		return;

#ifdef WIN32
	FLASHWINFO flInfo;
	flInfo.cbSize = sizeof(flInfo);
	flInfo.hwnd = (HWND)winId();
	flInfo.dwFlags = FLASHW_TIMERNOFG | FLASHW_TRAY;
	flInfo.uCount = 3;
	flInfo.dwTimeout = 0;
	::FlashWindowEx(&flInfo);
#endif
}

void MainDialog::stopFlashMainDlg()
{
#ifdef WIN32
	FLASHWINFO flInfo;
	flInfo.cbSize = sizeof(flInfo);
	flInfo.hwnd = (HWND)winId();
	flInfo.dwFlags = FLASHW_STOP;
	::FlashWindowEx(&flInfo);
#endif
}
