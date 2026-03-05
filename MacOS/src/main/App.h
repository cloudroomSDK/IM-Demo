#ifndef APP_H
#define APP_H

#include "CSystemTrayIcon.h"
#include "AccountInfo.h"

class App : public QApplication
{
public:
    App(int &argc, char **argv);
	~App();
	static App *instance();

	enum APP_LOGIN_TYPE
	{
		APPLT_PHONE = 0,		//电话方式登录
		APPLT_ACCNT,			//帐号方式登录
	};

	struct CfgInfo{
		QString				businessServer;	//业务服务器
		bool				skipVerifyCert; //https支持自签证书

		APP_LOGIN_TYPE		lastLoginType;		//登录一次登录类型
		QString				lastLoginNumber;	//上次登录电话号码
		QString				lastLoginAccount;	//上次登录账号
		QString				lastLoginPwd;		//上次登录密码
		QString				lastLoginToken;		//上次登录的token

		QString				lastSdkServer;		//上次登录的imsdk 服务器
		QString				lastSdkUserID;		//上次登录的imsdk userid
		QString				lastSdkUserName;	//上次登录的imsdk username
		int					lastSdkAuthType;	//上次登录的imsdk 鉴权方式
		QString				lastSdkToken;		//上次登录的imsdk token
		QString				lastSdkAppId;		//上次登录的imsdk appid
		QString				lastSdkSecret;		//上次登录的imsdk secret

		CfgInfo()
		{
			clear();
		}
		void clear()
		{
            businessServer.clear();
			skipVerifyCert = false;

			clearLoginInfo();
		}

		void clearLoginInfo()
		{
			lastLoginType = APPLT_ACCNT;
			lastLoginNumber.clear();
			lastLoginAccount.clear();
			lastLoginPwd.clear();
			lastLoginToken.clear();

			lastSdkServer.clear();
			lastSdkUserID.clear();
			lastSdkToken.clear();
			lastSdkAuthType = 0;
			lastSdkAppId.clear();
			lastSdkSecret.clear();
		}
	};

public:
	//app配置
	void setCfgInfo(const App::CfgInfo &cfg);
	const App::CfgInfo &getCfgInfo() const;
    QString getDemoServer() const;
	int getLogLv() const { return m_logLv; }

	//配置文件读写
	QString getStringCfg(const QString &key, const QString &group = "UserCfg");
	int getIntCfg(const QString &key, int defaultValue = 0, const QString &group = "UserCfg");

	QIcon getAppIcon() { return m_appIcon; }
    QString getAppName() { return "CRIM demo"; }
	CSystemTrayIcon* getTrayIcon() { return m_trayIcon; }

	//程序数据目录
	QString getAppDatPath();

	//用户数据路径
	QString getAcntDatPath();
	//聊天临时文件路径
	QString getAcntChatTempPath();
	//日志文件路径
	QString getAcntLogPath();
	//文件存储路径
	QString getAcntFileStorage();

	QString getUsrLoginAcnt();

protected:
	QString getAcntRootPath();
	void readSettings();
	void writeSettings();
	void initResource();
	QString getAppIDFromToken(const QString &tokenStr);

public slots :
	void slot_trayIconShow();
	void slot_exitApp();

protected:
	CfgInfo		m_cfg;
	int			m_logLv{ 1 };
    QSettings	*m_settings;
	QIcon		m_appIcon;
	CSystemTrayIcon* m_trayIcon;

};
#endif
