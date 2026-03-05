#ifndef LOGINPAGE_H
#define LOGINPAGE_H

#include <QFrame>
#include "ui_LoginPage.h"

class LoginPage : public QFrame
{
	Q_OBJECT

public:
	LoginPage(QWidget *parent = 0);
	~LoginPage();

	void logout();

signals:
	void s_loginSuccess();

public slots:
	void slot_cfgChanged();

protected slots:
	void slot_checkAutoLogin();
	void slot_authCodeLoginBtnClicked();
	void slot_pswdLoginBtnClicked();
	void slot_getAuthCodeBtnClicked();
	void slot_DemoSvrLoginFailed(const QString &errMsg);
	void slot_DemoSvrLoginSuccess(const DemoSvrWebAPI::LoginRsp &rsp);
	void slot_IMSDKLoginSuccess(const QString &uid, const QString &cookie);
	void slot_IMSDKLoginFailed(int code, const QString &error, const QString &cookie);
	void slot_updateGetAuthCodeBtn();
	void slot_swtichPwdPageBtnClicked();
	void slot_swtichAuthCodePageBtnClicked();

protected:
	void loginByAuthCode();
	void loginByPassWord();
	void loginIMSDKSvr(const App::CfgInfo &loginCfg);

	void onLoginFailed(const QString &error);
	void onLoginSuccess();

private:
	Ui::LoginPage	ui;
	bool m_bIMSvrLogined;
};

#endif // LOGINPAGE_H
