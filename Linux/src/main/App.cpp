#include "stdafx.h"
#include "App.h"
#include "LoginDlg.h"
#include "MainDialog.h"

#define APP_INI_FILE_NAME "CRIMSDKDemo.ini"

App* App::instance()
{
	App *pIns = static_cast<App*>(QApplication::instance());
	return pIns;
}

App::App(int &argc, char **argv)
    : QApplication(argc, argv)
{
	QString iniFile = getAppDatPath() + APP_INI_FILE_NAME;
	m_settings = new QSettings(iniFile, QSettings::IniFormat);

	readSettings();
	initResource();

    this->setApplicationDisplayName(this->getAppName());

	m_trayIcon = new CSystemTrayIcon(this);
	connect(m_trayIcon, &CSystemTrayIcon::s_show, this, &App::slot_trayIconShow);
	connect(m_trayIcon, &CSystemTrayIcon::s_exitApp, this, &App::slot_exitApp);
}

App::~App()
{
    delete m_settings;
}


void App::initResource()
{
	//加载qss
	QByteArray qssFileDat = ReadDataFromFile(":/Res/Demo.qss");
	qApp->setStyleSheet(QString::fromUtf8(qssFileDat));

	//取图标
	QPixmap icon(":/logo.png");
	int iconSizeList[] = { 16, 32, 64, 128, 256 };
	int nDefSize = sizeof(iconSizeList) / sizeof(int);
	for (int i = 0; i < nDefSize; i++)
	{
		QPixmap img = svgRes2Pixmap(":/Res/logo.svg", QSize(iconSizeList[i], iconSizeList[i]));
		m_appIcon.addPixmap(img);
	}
}

void App::readSettings()
{
	m_cfg.clear();

    m_settings->beginGroup("UserCfg");
    m_cfg.businessServer = m_settings->value("businessServer", m_cfg.businessServer).toString();
	if (m_cfg.businessServer.isEmpty())
	{
		m_cfg.businessServer = g_default_businessServer;
	}
	m_cfg.skipVerifyCert = m_settings->value("skipVerifyCert", m_cfg.skipVerifyCert).toBool();
	m_cfg.lastLoginType = (App::APP_LOGIN_TYPE)m_settings->value("lastLoginType", m_cfg.lastLoginType).toInt();
	m_cfg.lastLoginNumber = m_settings->value("lastloginNumber", m_cfg.lastLoginNumber).toString();
	m_cfg.lastLoginAccount = m_settings->value("lastLoginAccount", m_cfg.lastLoginAccount).toString();
	m_cfg.lastLoginPwd = m_settings->value("lastLoginPwd", m_cfg.lastLoginPwd).toString();
	m_cfg.lastLoginToken = m_settings->value("lastLoginToken", m_cfg.lastLoginToken).toString();
	m_cfg.lastSdkServer = m_settings->value("lastSdkServer", m_cfg.lastSdkServer).toString();
	m_cfg.lastSdkAuthType = m_settings->value("lastSdkAuthType", m_cfg.lastSdkAuthType).toInt();
	m_cfg.lastSdkToken = m_settings->value("lastSdkToken", m_cfg.lastSdkToken).toString();
	m_cfg.lastSdkAppId = m_settings->value("lastSdkAppId", m_cfg.lastSdkAppId).toString();
	m_cfg.lastSdkSecret = m_settings->value("lastSdkSecret", m_cfg.lastSdkSecret).toString();
	m_cfg.lastSdkUserID = m_settings->value("lastSdkUserID", m_cfg.lastSdkUserID).toString();
	m_cfg.lastSdkUserName = m_settings->value("lastSdkUserName", m_cfg.lastSdkUserName).toString();

	m_logLv = m_settings->value("logLv", 1).toInt();
    m_settings->endGroup();
}

void App::writeSettings()
{
    m_settings->beginGroup("UserCfg");
	QString strBServer = m_cfg.businessServer;
	if (strBServer == g_default_businessServer)
	{
		strBServer.clear();
	}
	m_settings->setValue("businessServer", strBServer);
	m_settings->setValue("skipVerifyCert", m_cfg.skipVerifyCert);
	m_settings->setValue("lastLoginType", m_cfg.lastLoginType);
    m_settings->setValue("lastloginNumber", m_cfg.lastLoginNumber);
	m_settings->setValue("lastLoginAccount", m_cfg.lastLoginAccount);
	m_settings->setValue("lastLoginPwd", m_cfg.lastLoginPwd);
	m_settings->setValue("lastLoginToken", m_cfg.lastLoginToken);
	m_settings->setValue("lastSdkAuthType", m_cfg.lastSdkAuthType);
    m_settings->setValue("lastSdkServer", m_cfg.lastSdkServer);
	m_settings->setValue("lastSdkToken", m_cfg.lastSdkToken);
	m_settings->setValue("lastSdkAppId", m_cfg.lastSdkAppId);
	m_settings->setValue("lastSdkSecret", m_cfg.lastSdkSecret);
	m_settings->setValue("lastSdkUserID", m_cfg.lastSdkUserID);
	m_settings->setValue("lastSdkUserName", m_cfg.lastSdkUserName);
    m_settings->endGroup();
    m_settings->sync();
}


QString App::getAppIDFromToken(const QString &tokenStr)
{
	QStringList datas = tokenStr.split(".");
	if (datas.size() >= 3)
	{
		QByteArray base64Str = datas.at(1).toUtf8();
		QByteArray jsonStr = QByteArray::fromBase64(base64Str);
		QVariantMap varMap = JsonToVarMap(jsonStr);
		return varMap["appID"].toString();
	}
	return QString();
}


void App::setCfgInfo(const App::CfgInfo &cfg)
{
	m_cfg = cfg;
	writeSettings();
}

const App::CfgInfo &App::getCfgInfo() const
{
	return m_cfg;
}

QString App::getDemoServer() const
{
    QString str = m_cfg.businessServer;
    if( str.isEmpty() )
    {
        str = g_default_businessServer;
    }
    return str;
}


QString App::getStringCfg(const QString &key, const QString &group)
{
	QString result;
    m_settings->beginGroup(group);
    if (m_settings->contains(key))
	{
        result = m_settings->value(key).toString();
	}
    m_settings->endGroup();
	return result;
}

int  App::getIntCfg(const QString &key, int defaultValue, const QString &group)
{
	QString result = getStringCfg(key, group);
	if (result.isEmpty())
		return defaultValue;
	return result.toInt();
}

QString App::getAppDatPath()
{
#ifndef MAC
	QString str = AddBackslashAtend(QCoreApplication::applicationDirPath());
#else
	QString str = AddBackslashAtend(QStandardPaths::writableLocation(QStandardPaths::DataLocation));
#endif
	return str;
}

QString App::getAcntRootPath()
{
	QString str = getAppDatPath();
	QString acntID = getCfgInfo().lastSdkUserID;
	if (acntID.isEmpty())
	{
		acntID = "DefaultUser";
	}
	str += AddBackslashAtend(acntID);
	return str;
}

QString App::getAcntDatPath()
{
	QString path = getAcntRootPath() + AddBackslashAtend("db");
	QDir qdir;
	qdir.mkpath(path);
	return path;
}

QString App::getAcntLogPath()
{
	QString path = getAcntRootPath() + AddBackslashAtend("log");
	QDir qdir;
	qdir.mkpath(path);
	return path;
}

QString App::getAcntChatTempPath()
{
	QString path = getAcntRootPath() + AddBackslashAtend("ChatTmp");
	QDir qdir;
	qdir.mkpath(path);
	return path;
}

QString App::getAcntFileStorage()
{
	QString path = getAcntRootPath() + AddBackslashAtend("FileStorage");
	QDir qdir;
	qdir.mkpath(path);
	return path;
}

QString App::getUsrLoginAcnt()
{
	const CfgInfo &info = getCfgInfo();
	if (info.lastLoginNumber.length() > 0)
		return info.lastLoginNumber;
	if (info.lastLoginAccount.length() > 0)
		return info.lastLoginAccount;
	return getAppName();
}

void App::slot_trayIconShow()
{
	if (MainDialog::hasInstance())
	{
		ShowAndActiveWindow(MainDialog::instance());
	}
	else
	{
		ShowAndActiveWindow(LoginDlg::instance());
	}
}

void App::slot_exitApp()
{
	LoginDlg::instance()->logout();
	QApplication::exit();
}
