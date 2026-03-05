#include "stdafx.h"
#include "LoginPage.h"
#include "DemoSvrWebAPI.h"
#include "Version.h"

LoginPage::LoginPage(QWidget *parent)
	: QFrame(parent)
{
	ui.setupUi(this);

	m_bIMSvrLogined = false;
	connect(ui.authCodeLoginBtn, &QPushButton::clicked, this, &LoginPage::slot_authCodeLoginBtnClicked);
	connect(ui.pswdLoginBtn, &QPushButton::clicked, this, &LoginPage::slot_pswdLoginBtnClicked);
	connect(ui.getAuthCodeBtn, &QPushButton::clicked, this, &LoginPage::slot_getAuthCodeBtnClicked);
	connect(ui.swtichPwdPageBtn, &QPushButton::clicked, this, &LoginPage::slot_swtichPwdPageBtnClicked);
	connect(ui.swtichAuthCodePageBtn, &QPushButton::clicked, this, &LoginPage::slot_swtichAuthCodePageBtnClicked);
	connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_loginSuccess, this, &LoginPage::slot_IMSDKLoginSuccess);
	connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_loginFailed, this, &LoginPage::slot_IMSDKLoginFailed);

	slot_cfgChanged();
	const App::CfgInfo &appCfg = App::instance()->getCfgInfo();
	ui.phoneNumber->setText(appCfg.lastLoginNumber);

	//sdk版本号
	QString demoVer = STR_VERSION;
	QString strSDKVer = CRIMSDK::getVersion();
	QString info = QString("Demo:%1    SDK:%2").arg(demoVer).arg(strSDKVer);
	ui.label_version->setText(info);

	connect(DemoSvrWebAPI::instance(), &DemoSvrWebAPI::s_loginFailed, this, &LoginPage::slot_DemoSvrLoginFailed);
	connect(DemoSvrWebAPI::instance(), &DemoSvrWebAPI::s_loginSuccess, this, &LoginPage::slot_DemoSvrLoginSuccess);

	QTimer::singleShot(10, this, SLOT(slot_checkAutoLogin()));
}

LoginPage::~LoginPage()
{
}

void LoginPage::slot_cfgChanged()
{
	const App::CfgInfo &appCfg = App::instance()->getCfgInfo();
	DemoSvrWebAPI::instance()->onSvrChanged();
}


void LoginPage::slot_getAuthCodeBtnClicked()
{
	QString strPhone = ui.phoneNumber->text();
	if (!isChinaMobile(strPhone))
	{
		CRMsgBox::information(this, tr("错误"), tr("请输入有效的手机号码!"));
		ui.phoneNumber->setFocus();
		return;
	}
	DemoSvrWebAPI::instance()->requestAuthCode(strPhone);

	ui.getAuthCodeBtn->setDisabled(true);
	ui.getAuthCodeBtn->setProperty("leftSecond", 60);
	slot_updateGetAuthCodeBtn();
}

void LoginPage::slot_updateGetAuthCodeBtn()
{
	int leftSec = ui.getAuthCodeBtn->property("leftSecond").toInt();
	if (leftSec > 0)
	{
		ui.getAuthCodeBtn->setText(tr("%1秒后可重试").arg(leftSec));
		ui.getAuthCodeBtn->setProperty("leftSecond", leftSec-1);
		QTimer::singleShot(1000, this, &LoginPage::slot_updateGetAuthCodeBtn);
	}
	else
	{
		ui.getAuthCodeBtn->setDisabled(false);
		ui.getAuthCodeBtn->setText("获取验证码");
	}
}

void LoginPage::slot_checkAutoLogin()
{
	const App::CfgInfo& appCfg = App::instance()->getCfgInfo();
	if (appCfg.lastLoginType==App::APPLT_ACCNT && appCfg.lastLoginAccount.length()>0 && appCfg.lastLoginPwd.length()>0)
	{
		//账号密码登录
		ui.stackLogin->setCurrentWidget(ui.pagePassword);
		ui.editPassword->setFocus();
		DemoSvrWebAPI::instance()->loginByPassWord(appCfg.lastLoginAccount, appCfg.lastLoginPwd);
		this->setDisabled(true);
		ui.pswdLoginBtn->setEnabled(false);
	}
	else if (appCfg.lastLoginType == App::APPLT_PHONE && appCfg.lastLoginNumber.length() > 0 && appCfg.lastLoginToken.length() > 0)
	{
		ui.stackLogin->setCurrentWidget(ui.pageAuthCode);
		ui.authCode->setFocus();
		DemoSvrWebAPI::instance()->loginByToken(appCfg.lastLoginToken);
		this->setDisabled(true);
		ui.authCodeLoginBtn->setEnabled(false);
	}
}


void LoginPage::slot_authCodeLoginBtnClicked()
{
	loginByAuthCode();
}

void LoginPage::slot_pswdLoginBtnClicked()
{
	loginByPassWord();
}

void LoginPage::slot_swtichPwdPageBtnClicked()
{
	ui.stackLogin->setCurrentWidget(ui.pagePassword);
}

void LoginPage::slot_swtichAuthCodePageBtnClicked()
{
	ui.stackLogin->setCurrentWidget(ui.pageAuthCode);
}

//手机号+验证码登录
void LoginPage::loginByAuthCode()
{
	QString phoneNumber = ui.phoneNumber->text();
	if (!isChinaMobile(phoneNumber))
	{
		CRMsgBox::information(this, tr("错误"), tr("请输入有效的手机号码!"));
		ui.phoneNumber->setFocus();
		return;
	}
	QString strAuthCode = ui.authCode->text();
	if (strAuthCode.isEmpty())
	{
		CRMsgBox::information(this, tr("错误"), tr("请输入有效验证码!"));
		ui.authCode->setFocus();
		return;
	}

	App::CfgInfo appCfg = App::instance()->getCfgInfo();
	appCfg.clearLoginInfo();
	appCfg.lastLoginType = App::APPLT_PHONE;
	appCfg.lastLoginNumber = phoneNumber;
	App::instance()->setCfgInfo(appCfg);

	this->setDisabled(true);
	ui.authCodeLoginBtn->setEnabled(false);
	DemoSvrWebAPI::instance()->loginByAuthCode(appCfg.lastLoginNumber, strAuthCode);
}

//账号+密码登录
void LoginPage::loginByPassWord()
{
	QString strAccount = ui.editAccount->text();
	if (strAccount.isEmpty())
	{
		CRMsgBox::information(this, tr("错误"), tr("请输入账号!"));
		ui.editAccount->setFocus();
		return;
	}
	QString strPassword = ui.editPassword->text();
	if (strPassword.isEmpty())
	{
		CRMsgBox::information(this, tr("错误"), tr("请输入密码!"));
		ui.editPassword->setFocus();
		return;
	}

	App::CfgInfo appCfg = App::instance()->getCfgInfo();
	appCfg.clearLoginInfo();
	appCfg.lastLoginType = App::APPLT_ACCNT;
	appCfg.lastLoginAccount = strAccount;
	appCfg.lastLoginPwd = strPassword;
	App::instance()->setCfgInfo(appCfg);

	this->setDisabled(true);
	ui.pswdLoginBtn->setEnabled(false);
	DemoSvrWebAPI::instance()->loginByPassWord(strAccount, strPassword);
}

//登录业务服务器失败
void LoginPage::slot_DemoSvrLoginFailed(const QString &errMsg)
{
	onLoginFailed(errMsg);
}

//登录业务服务器成功
void LoginPage::slot_DemoSvrLoginSuccess(const DemoSvrWebAPI::LoginRsp &rsp)
{
	App::CfgInfo appCfg = App::instance()->getCfgInfo();
	appCfg.lastLoginToken = rsp.demoToken;
	appCfg.lastSdkAppId = rsp.sdkAppId;
	appCfg.lastSdkServer = rsp.sdkSvr;
	appCfg.lastSdkAuthType = rsp.sdkAuthType;
	appCfg.lastSdkToken = rsp.sdkToken;
	appCfg.lastSdkSecret = rsp.sdkSecret;
	appCfg.lastSdkUserID = rsp.sdkUsrID;
	appCfg.lastSdkUserName = rsp.sdkNickname;
	App::instance()->setCfgInfo(appCfg);

	//登录imsdk
	loginIMSDKSvr(appCfg);
}

//登录im sdk服务器
void LoginPage::loginIMSDKSvr(const App::CfgInfo &loginCfg)
{
	m_bIMSvrLogined = false;
	CRIMManager_Qt::instance()->initSDK(loginCfg.lastSdkServer, loginCfg.skipVerifyCert, App::instance()->getAcntDatPath(), App::instance()->getAcntLogPath(), CRIMLogLevel(App::instance()->getLogLv()));
	if (loginCfg.lastSdkAuthType == 1)
	{
		CRIMManager_Qt::instance()->loginWithToken(loginCfg.lastSdkUserID, loginCfg.lastSdkToken);
	}
	else
	{
		CRIMManager_Qt::instance()->loginWithAppID(loginCfg.lastSdkUserID, loginCfg.lastSdkAppId, loginCfg.lastSdkSecret);
	}
}

//登录im sdk服务器成功
void LoginPage::slot_IMSDKLoginSuccess(const QString &uid, const QString &cookie)
{
	m_bIMSvrLogined = true;
	onLoginSuccess();
}

//登录im sdk服务器失败
void LoginPage::slot_IMSDKLoginFailed(int code, const QString &error, const QString &cookie)
{
	onLoginFailed(tr("登录服务器失败(%1)").arg(error));
}

void LoginPage::onLoginFailed(const QString &error)
{
	this->setDisabled(false);
	ui.authCodeLoginBtn->setEnabled(true);
	ui.pswdLoginBtn->setEnabled(true);

	CRMsgBox::information(this, tr("错误"), error);
}

void LoginPage::onLoginSuccess()
{
	this->setDisabled(false);
	ui.authCodeLoginBtn->setEnabled(true);
	ui.pswdLoginBtn->setEnabled(true);

	emit s_loginSuccess();
}

void LoginPage::logout()
{
	g_sdkMain->logout();

	CRIMManager_Qt::instance()->logout();
	CRIMManager_Qt::instance()->unInitSDK();
}

