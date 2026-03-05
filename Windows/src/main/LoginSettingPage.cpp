#include "stdafx.h"
#include "LoginSettingPage.h"
#include "App.h"

LoginSettingPage::LoginSettingPage(QWidget *parent)
	: QFrame(parent)
{
	ui.setupUi(this);
	updateUI(App::instance()->getCfgInfo());

	connect(ui.resetBtn, &QPushButton::clicked, this, &LoginSettingPage::slot_btnResetClicked);
	connect(ui.appServer, &QLineEdit::textChanged, this, &LoginSettingPage::slot_appServerChanged);
	upSkipVerifyCertVisible();
}

LoginSettingPage::~LoginSettingPage()
{

}

void LoginSettingPage::updateUI(const App::CfgInfo &cfg)
{
    QString text = cfg.businessServer;
    if( text.isEmpty() || text==g_default_businessServer )
    {
        text = tr("默认服务器");
    }
    ui.appServer->setText(text);
	ui.skipVerifyCert->setChecked(cfg.skipVerifyCert);
}

void LoginSettingPage::slot_appServerChanged()
{
	upSkipVerifyCertVisible();
}


void LoginSettingPage::upSkipVerifyCertVisible()
{
	QString txt = ui.appServer->text().toLower();
	if (txt == tr("默认服务器"))
	{
		txt = g_default_businessServer;
	}
	bool bHttps = txt.startsWith("https:");
	ui.skipVerifyCert->setVisible(bHttps);
}


void LoginSettingPage::slot_btnResetClicked()
{
	App::CfgInfo appCfg;
	updateUI(appCfg);
}

bool LoginSettingPage::save()
{
	const App::CfgInfo &appCfg = App::instance()->getCfgInfo();
	App::CfgInfo newCfg = appCfg;
	newCfg.businessServer = ui.appServer->text();
	newCfg.skipVerifyCert = ui.skipVerifyCert->isVisible() && ui.skipVerifyCert->isChecked();
    if( newCfg.businessServer==tr("默认服务器") )
    {
		newCfg.businessServer = g_default_businessServer;
    }
	if (newCfg.businessServer==appCfg.businessServer && newCfg.skipVerifyCert==appCfg.skipVerifyCert)
	{
		return true;
	}

	App::instance()->setCfgInfo(newCfg);
	emit s_cfgChanged();
	return true;
}
