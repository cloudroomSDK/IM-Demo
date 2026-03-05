#ifndef DemoSvrWebAPI_h
#define DemoSvrWebAPI_h

class DemoSvrWebAPI : public QObject
{
	Q_OBJECT
	DemoSvrWebAPI();
public:
	~DemoSvrWebAPI() {}
	static DemoSvrWebAPI* instance();
	void onSvrChanged();

	struct UserInfo
	{
		QString userID;
		QString nickname;
		QString faceUrl;
		QString phoneNumber;
		QString email;
		int gender{ 0 }; //0:未定义, 1:男， 2:女
		int64_t birth{ 0 };
	};

	struct LoginRsp
	{
		QString demoToken;	//业务token

		QString sdkSvr;		//sdk服务器地址
		QString sdkUsrID;	//sdk 用户id
		QString sdkNickname;//sdk 用户昵称
		QString sdkAppId;	//sdk appid;
		int sdkAuthType;	//sdk 鉴权方式，默认0，0:密钥鉴权，1:token鉴权
		QString sdkSecret;	//密钥鉴权时，密钥
		QString sdkToken;	//token鉴权时，token
	};

	//获取验证码
	void requestAuthCode(const QString &phoneNum);
	//验证码注册并登录
	void loginByAuthCode(const QString &phoneNum, const QString &authCode);
	//用户名密码登录
	void loginByPassWord(const QString &account, const QString &password);
	//历史token登录
	void loginByToken(const QString &demoLoginToken);

	void queryUserInfo(const QString &userID);
	void updateSelfInfo(const UserInfo &info, const QString &cookie);

	// 搜索用户（添加好友时使用）
	void searchUserInfo(const QString &keyword);

signals:
	void s_loginSuccess(const LoginRsp &rsp);
	void s_loginFailed(const QString &errMsg);
	void s_queryUserInfoRslt(const QString &errMsg, const UserInfo &info);
	void s_updateSelfInfoRslt(const QString &errMsg, const QString &cookie);
	void s_searchUserInfoRslt(const QString &errMsg, const UserInfo &info);

protected slots:
	void slot_requestAuthCodeFinished();
	void slot_loginFinished();
	void slot_queryUserInfoFinished();
	void slot_updateSelfInfoFinished();
	void slot_searchUserInfoFinished();

protected:
	QString getRsp(QNetworkReply* netReply, QVariantMap &rsp);

protected:
	QNetworkAccessManager* m_netMgr{ nullptr };
	QString m_webApiToken;
};

#endif