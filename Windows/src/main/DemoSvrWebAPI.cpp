#include "stdafx.h"
#include "DemoSvrWebAPI.h"

#if defined(WIN32)
#	define CUR_PLATFORM CRIM_WIN
#elif defined(LINUX)
#	define CUR_PLATFORM CRIM_LINUX
#elif defined(MAC)
#	define CUR_PLATFORM CRIM_XOS
#endif

DemoSvrWebAPI* DemoSvrWebAPI::instance()
{
	static DemoSvrWebAPI ins;
	return &ins;
}

DemoSvrWebAPI::DemoSvrWebAPI()
{
	qRegisterMetaType<DemoSvrWebAPI::UserInfo>("DemoWebAPI::UserInfo");
	qRegisterMetaType<DemoSvrWebAPI::LoginRsp>("DemoWebAPI::LoginRsp");
	m_netMgr = new QNetworkAccessManager(this);
}

void DemoSvrWebAPI::onSvrChanged()
{
	m_netMgr->deleteLater();
	m_netMgr = new QNetworkAccessManager(this);
}

void DemoSvrWebAPI::requestAuthCode(const QString &phoneNum)
{
    QString url = QString("%1/account/code/send").arg(App::instance()->getDemoServer());

	QUrl qurl(url);
	QNetworkRequest request(qurl);
	request.setRawHeader("Content-Type", "application/json; charset=utf-8");
	request.setRawHeader("operationID", makeUUID().toUtf8());
	request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
	cfgQNetworkRequestForHttps(&request, App::instance()->getCfgInfo().skipVerifyCert);

	QVariantMap varParams;
	varParams["phoneNumber"] = phoneNum;
	varParams["areaCode"] = "+86";
	varParams["usedFor"] = 3;
	QNetworkReply *netReply = m_netMgr->post(request, VarMapToJson(varParams));
	connect(netReply, &QNetworkReply::finished, this, &DemoSvrWebAPI::slot_requestAuthCodeFinished);
}

void DemoSvrWebAPI::slot_requestAuthCodeFinished()
{
	QNetworkReply *netReply = dynamic_cast<QNetworkReply*>(sender());
	if (netReply == nullptr)
		return;

	QVariantMap varMap;
	QString errMsg = getRsp(netReply, varMap);
}


void DemoSvrWebAPI::loginByAuthCode(const QString &phoneNum, const QString &authCode)
{
	const App::CfgInfo& appCfg = App::instance()->getCfgInfo();
	QString url = QString("%1/account/login").arg(appCfg.businessServer);

	QUrl qurl(url);
	QNetworkRequest request(qurl);
	request.setRawHeader("Content-Type", "application/json; charset=utf-8");
	request.setRawHeader("operationID", makeUUID().toUtf8());
	request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
	cfgQNetworkRequestForHttps(&request, App::instance()->getCfgInfo().skipVerifyCert);

	QVariantMap varParams;
	//varParams["password"] = "";
	varParams["areaCode"] = "+86";
	varParams["platform"] = CUR_PLATFORM;
	varParams["verifyCode"] = authCode;
	varParams["phoneNumber"] = phoneNum;
	QNetworkReply *netReply = m_netMgr->post(request, VarMapToJson(varParams));
	connect(netReply, &QNetworkReply::finished, this, &DemoSvrWebAPI::slot_loginFinished);
}

void DemoSvrWebAPI::loginByPassWord(const QString &account, const QString &password)
{
    QString url = QString("%1/account/login").arg(App::instance()->getDemoServer());

	QUrl qurl(url);
	QNetworkRequest request(qurl);
	request.setRawHeader("Content-Type", "application/json; charset=utf-8");
	request.setRawHeader("operationID", makeUUID().toUtf8());
	request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
	cfgQNetworkRequestForHttps(&request, App::instance()->getCfgInfo().skipVerifyCert);

	QVariantMap varParams;
	varParams["account"] = account;
	varParams["password"] = MakeMd5(password.toLocal8Bit());
	varParams["platform"] = CUR_PLATFORM;
	QNetworkReply *netReply = m_netMgr->post(request, VarMapToJson(varParams));
	connect(netReply, &QNetworkReply::finished, this, &DemoSvrWebAPI::slot_loginFinished);
}

void DemoSvrWebAPI::loginByToken(const QString &demoLoginToken)
{
    QString url = QString("%1/account/login2").arg(App::instance()->getDemoServer());

	QUrl qurl(url);
	QNetworkRequest request(qurl);
	request.setRawHeader("Content-Type", "application/json; charset=utf-8");
	request.setRawHeader("operationID", makeUUID().toUtf8());
	request.setRawHeader("token", demoLoginToken.toUtf8());
	request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
	cfgQNetworkRequestForHttps(&request, App::instance()->getCfgInfo().skipVerifyCert);

	QVariantMap varParams;
	varParams["platform"] = CUR_PLATFORM;
	QNetworkReply *netReply = m_netMgr->post(request, VarMapToJson(varParams));
	connect(netReply, &QNetworkReply::finished, this, &DemoSvrWebAPI::slot_loginFinished);
}

QString DemoSvrWebAPI::getRsp(QNetworkReply* netReply, QVariantMap &rsp)
{
	netReply->deleteLater();

	QNetworkReply::NetworkError err = netReply->error();
	if (err != QNetworkReply::NoError)
	{
		return QString("NetworkError:%1").arg(err);
	}

	int httpCode = netReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
	if (httpCode != 200)
	{
		return QString("HttpCode:%1").arg(httpCode);
	}
	QByteArray rslt = netReply->readAll();
	QVariantMap varMap = JsonToVarMap(rslt);
	int errCode = varMap["errCode"].toInt();
	if (errCode != 0)
	{
		QString errMsg = varMap["errMsg"].toString();
		if (errMsg.isEmpty())
		{
			return QString("errCode:%1").arg(errCode);
		}
		return errMsg;
	}

	rsp = varMap;
	return QString();
}



void DemoSvrWebAPI::slot_loginFinished()
{
	QNetworkReply *netReply = dynamic_cast<QNetworkReply*>(sender());
	if (netReply == nullptr)
		return;

	QVariantMap varMap;
	QString errMsg = getRsp(netReply, varMap);

	if (!errMsg.isEmpty())
	{
		emit s_loginFailed(tr("登录业务服务器失败（%1）").arg(errMsg));
		return;
	}

	QVariantMap vData = varMap.value("data").toMap();
	m_webApiToken = vData.value("chatToken").toString();

	LoginRsp rsp;
	rsp.demoToken = m_webApiToken;
	rsp.sdkSvr = vData.value("sdkSvr").toString();
	rsp.sdkAppId = vData.value("sdkAppId").toString();
	rsp.sdkAuthType = vData.value("sdkAuthType").toInt();
	rsp.sdkSecret = MakeMd5(QByteArray::fromBase64(vData.value("sdkSecret").toByteArray()));
	rsp.sdkToken = vData.value("sdkToken").toString();
	rsp.sdkUsrID = vData.value("userID").toString();
	rsp.sdkNickname = vData.value("nickName").toString();
	emit s_loginSuccess(rsp);
}


void DemoSvrWebAPI::queryUserInfo(const QString &userID)
{
    QString url = QString("%1/user/find/full").arg(App::instance()->getDemoServer());

	QUrl qurl(url);
	QNetworkRequest request(qurl);
	request.setRawHeader("Content-Type", "application/json; charset=utf-8");
	request.setRawHeader("operationID", makeUUID().toUtf8());
	request.setRawHeader("token", m_webApiToken.toUtf8());
	request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
	cfgQNetworkRequestForHttps(&request, App::instance()->getCfgInfo().skipVerifyCert);

	QVariantList vidList{ userID };
	QVariantMap varParams;
	varParams["userIDs"] = vidList;
	QNetworkReply *netReply = m_netMgr->post(request, VarMapToJson(varParams));
	netReply->setProperty("userID", userID);
	connect(netReply, &QNetworkReply::finished, this, &DemoSvrWebAPI::slot_queryUserInfoFinished);
}


void DemoSvrWebAPI::slot_queryUserInfoFinished()
{
	QNetworkReply *netReply = dynamic_cast<QNetworkReply*>(sender());
	if (netReply == nullptr)
		return;

	UserInfo uInfo;
	uInfo.userID = netReply->property("userID").toString();

	QVariantMap varMap;
	QString errMsg = getRsp(netReply, varMap);
	if (!errMsg.isEmpty())
	{
		emit s_queryUserInfoRslt(tr("查询信息失败（%1）").arg(errMsg), uInfo);
		return;
	}

	QVariantMap vData = varMap["data"].toMap();
	QVariantList vUsers = vData["users"].toList();
	if (vUsers.size() <= 0)
	{
		emit s_queryUserInfoRslt(tr("查询信息失败（不存在）").arg(errMsg), uInfo);
		return;
	}

	QVariantMap vUInfo = vUsers[0].toMap();
	uInfo.nickname = vUInfo["nickname"].toString();
	uInfo.faceUrl = vUInfo["faceURL"].toString();
	uInfo.phoneNumber = vUInfo["phoneNumber"].toString();
	uInfo.email = vUInfo["email"].toString();
	uInfo.gender = vUInfo["gender"].toInt();
	uInfo.birth = vUInfo["birth"].toLongLong();
	emit s_queryUserInfoRslt(QString(), uInfo);
}


void DemoSvrWebAPI::updateSelfInfo(const UserInfo &info, const QString &cookie)
{
    QString url = QString("%1/user/update").arg(App::instance()->getDemoServer());

	QUrl qurl(url);
	QNetworkRequest request(qurl);
	request.setRawHeader("Content-Type", "application/json; charset=utf-8");
	request.setRawHeader("operationID", makeUUID().toUtf8());
	request.setRawHeader("token", m_webApiToken.toUtf8());
	request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
	cfgQNetworkRequestForHttps(&request, App::instance()->getCfgInfo().skipVerifyCert);

	QVariantMap varParams;
	varParams["userID"] = info.userID;
	varParams["nickname"] = info.nickname;
	varParams["faceURL"] = info.faceUrl;
	varParams["gender"] = info.gender;
    varParams["birth"] = (qint64)info.birth;
	varParams["phone"] = info.phoneNumber;
	varParams["email"] = info.email;
	QNetworkReply *netReply = m_netMgr->post(request, VarMapToJson(varParams));
	netReply->setProperty("cookie", cookie);
	connect(netReply, &QNetworkReply::finished, this, &DemoSvrWebAPI::slot_updateSelfInfoFinished);
}

void DemoSvrWebAPI::slot_updateSelfInfoFinished()
{
	QNetworkReply *netReply = dynamic_cast<QNetworkReply*>(sender());
	if (netReply == nullptr)
		return;

	QString cookie = netReply->property("cookie").toString();
	QVariantMap varMap;
	QString errMsg = getRsp(netReply, varMap);

	if (!errMsg.isEmpty())
	{
		emit s_updateSelfInfoRslt(tr("更新信息失败（%1）").arg(errMsg), cookie);
		return;
	}

	emit s_updateSelfInfoRslt(QString(), cookie);
}

void DemoSvrWebAPI::searchUserInfo(const QString &keyword)
{
    QString url = QString("%1/user/search/full").arg(App::instance()->getDemoServer());

	QUrl qurl(url);
	QNetworkRequest request(qurl);
	request.setRawHeader("Content-Type", "application/json; charset=utf-8");
	request.setRawHeader("operationID", makeUUID().toUtf8());
	request.setRawHeader("token", m_webApiToken.toUtf8());
	request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
	cfgQNetworkRequestForHttps(&request, App::instance()->getCfgInfo().skipVerifyCert);

	QVariantMap varParams;
	varParams["keyword"] = keyword;
	QVariantMap pageParams;
	pageParams["pageNumber"] = 1;
	pageParams["showNumber"] = 100;
	varParams["pagination"] = pageParams;
	QNetworkReply *netReply = m_netMgr->post(request, VarMapToJson(varParams));
// 	netReply->setProperty("userID", userID);
	connect(netReply, &QNetworkReply::finished, this, &DemoSvrWebAPI::slot_searchUserInfoFinished);
}

void DemoSvrWebAPI::slot_searchUserInfoFinished()
{
	QNetworkReply *netReply = dynamic_cast<QNetworkReply*>(sender());
	if (netReply == nullptr)
		return;

	UserInfo uInfo;
	QVariantMap varMap;
	QString errMsg = getRsp(netReply, varMap);
	if (!errMsg.isEmpty())
	{
		emit s_searchUserInfoRslt(tr("搜索用户失败（%1）").arg(errMsg), uInfo);
		return;
	}

	QVariantMap vData = varMap["data"].toMap();
	QVariantList vUsers = vData["users"].toList();
	if (vUsers.isEmpty())
	{
		emit s_searchUserInfoRslt(tr("未搜索到用户"), uInfo);
		return;
	}

	QVariantMap vUInfo = vUsers[0].toMap();
	uInfo.userID = vUInfo["userID"].toString();
	uInfo.nickname = vUInfo["nickname"].toString();
	uInfo.faceUrl = vUInfo["faceURL"].toString();
	uInfo.phoneNumber = vUInfo["phoneNumber"].toString();
	uInfo.email = vUInfo["email"].toString();
	uInfo.gender = vUInfo["gender"].toInt();
	uInfo.birth = vUInfo["birth"].toLongLong();

	emit s_searchUserInfoRslt(errMsg, uInfo);
}
