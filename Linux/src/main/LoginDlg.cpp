#include "stdafx.h"
#include "LoginDlg.h"
#include "ui_LoginDlg.h"
#include "MainDialog.h"
#include "UserStatusMgr.h"


class LoginDlg* LoginDlg::s_pLogin = nullptr;

LoginDlg::LoginDlg(QWidget *parent)  : QNoNCDialog(parent)
{
	s_pLogin = this;

    unsigned int sysBtnStyle = 0;
#ifdef MAC
    m_bShowTitle = true;
    unifiedNativeTitle(true);
#else
    sysBtnStyle = TitleBar::TBTN_CLOSE;
#endif

    StandardTitleBar *pTitleBar = new StandardTitleBar(this, sysBtnStyle);
    pTitleBar->setObjectName("loginDlgTitleBar");
	pTitleBar->setRightBtnsAlign(Qt::AlignTop);
    this->setTitleBar(pTitleBar);
    
    m_pSettingBtn = new QToolButton(pTitleBar);
    m_pSettingBtn->setObjectName("btnSetting");
	m_pSettingBtn->setFocusPolicy(Qt::NoFocus);
	m_pSettingBtn->setToolTip(tr("设置"));
	connect(m_pSettingBtn, SIGNAL(clicked(bool)), this, SLOT(slot_btnLoginSettingClicked()));
	pTitleBar->addWidgetAfterTitle(m_pSettingBtn);

	m_pBackBtn = new QToolButton(pTitleBar);
	m_pBackBtn->setText(tr("登录设置"));
	m_pBackBtn->setObjectName("btnBack");
	m_pBackBtn->setFocusPolicy(Qt::NoFocus);
	m_pBackBtn->setToolTip(tr("返回"));
	m_pBackBtn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	m_pBackBtn->hide();
	connect(m_pBackBtn, SIGNAL(clicked(bool)), this, SLOT(slot_btnBackClicked()));
	pTitleBar->addWidgetBeforeIcon(m_pBackBtn);

    ui = new Ui::LoginDlg;
    QWidget *pClient = new QWidget(this);
    ui->setupUi(pClient);
    this->cfgCloseType(false);
    this->setClientWidget(pClient);

	App::instance()->getTrayIcon()->show();

	connect(ui->loginPage, &LoginPage::s_loginSuccess, this, &LoginDlg::slot_loginSuccess);
	connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_kickedOffline, this, &LoginDlg::slot_kickedOffline);
	connect(ui->settingPage, &LoginSettingPage::s_cfgChanged, ui->loginPage, &LoginPage::slot_cfgChanged);
	ui->stackedWidget->setCurrentWidget(ui->loginPage);
}

LoginDlg::~LoginDlg()
{
    delete ui;
    ui = nullptr;
    s_pLogin = nullptr;
}

void LoginDlg::slot_btnLoginSettingClicked()
{
	m_pSettingBtn->hide();
	m_pBackBtn->show();
	ui->stackedWidget->setCurrentWidget(ui->settingPage);
}

void LoginDlg::slot_btnBackClicked()
{
	if (!ui->settingPage->save())
		return;

	m_pBackBtn->hide();
	m_pSettingBtn->show();
	ui->stackedWidget->setCurrentWidget(ui->loginPage);
}

void LoginDlg::slot_loginSuccess()
{
	this->hide();
	MainDialog::instance()->showOnLoginSuccess();
	App::instance()->getTrayIcon()->setToolTip(App::instance()->getUsrLoginAcnt());
	UserStatusMgr::instance()->setLoingState(true);
}

void LoginDlg::signOut()
{
	logout();

	App::CfgInfo appCfg = App::instance()->getCfgInfo();
	appCfg.clearLoginInfo();
	App::instance()->setCfgInfo(appCfg);

	ShowAndActiveWindow(this);
}

void LoginDlg::logout()
{
	if (MainDialog::hasInstance())
	{
		CloseWindowAndAllSubs(MainDialog::instance());
		MainDialog::instance()->deleteLater();
	}
	ui->loginPage->logout();
	App::instance()->getTrayIcon()->stopFlashTray();
	App::instance()->getTrayIcon()->setToolTip(App::instance()->getAppName());
	UserStatusMgr::instance()->setLoingState(false);
}

void LoginDlg::closeEvent(QCloseEvent *e)
{
	QNoNCDialog::closeEvent(e);
	App::instance()->slot_exitApp();
}

//被踢下线
void LoginDlg::slot_kickedOffline()
{
	signOut();
	CRMsgBox::information(this, tr("提示"), tr("您已被踢下线"));
}
