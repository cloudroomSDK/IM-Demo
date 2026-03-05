#include "stdafx.h"
#include "ImCalloutDlg.h"
#include "ui_ImCalloutDlg.h"
#include "AudioMgr.h"
#include "MainDialog.h"
#include "LoginDlg.h"
#include "UserStatusMgr.h"
#include "MeetingMainDialog.h"

#define ICON_SIZE	60

ImCalloutDlg::ImCalloutDlg(IMCallType type, const QString &userID, QWidget *parent) : QNoNCDialog(parent)
{
	const CRIMFriendInfo *pInfo = CRIMManager_Qt::instance()->getFriendInfo(userID);
	if (pInfo == NULL)
	{
		CRIMMgrLogDebug("start im call failed! userid:%s not exist!", userID.toUtf8().constData());
		this->deleteLater();
		return;
	}

	g_sdkMain->AddCallBack(this);
	g_sdkMain->getSDKMeeting().AddCallBack(this);
	UserStatusMgr::instance()->setIMCallStatus(true);

	StandardTitleBar *pTitleBar = new StandardTitleBar(this, StandardTitleBar::TBTN_CLOSE);
	this->setTitleBar(pTitleBar);

	QWidget *pClient = new QWidget(this);
	ui = new Ui::ImCalloutDlg();
	ui->setupUi(pClient);
	this->setClientWidget(pClient);
	this->setAttribute(Qt::WA_DeleteOnClose);
	this->cfgCloseType(false);

	_type = type;
	_fInfo = *pInfo;
	this->setWindowTitle(_type == IMAudioCall ? tr("语音呼叫") : tr("视频呼叫"));
	ImIconMgr::instance()->bindIcon(ui->icon, IMIconInfo(userID, IMICON_USR, QSize(ICON_SIZE, ICON_SIZE)));
	ui->name->setText(QString("%1").arg(_fInfo.getDisplayName()));

	connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_notifyCallAccepted, this, &ImCalloutDlg::slot_callAccepted);
	connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_notifyCallRejected, this, &ImCalloutDlg::slot_callRejected);
	connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_notifyCallHungUp, this, &ImCalloutDlg::slot_notifyCallHungUp);

	_timeout.setSingleShot(true);
	connect(&_timeout, &QTimer::timeout, this, &ImCalloutDlg::slot_timeout);
	connect(ui->cancelBtn, &QToolButton::clicked, this, &ImCalloutDlg::close);

    QString path = ":/Res/ringback.wav";
	AudioMgr::instance().startPlaySound(path);


	onCallinState(ST_Login, tr("正在连接通话服务器..."));
	_taskID = makeUUID();
	loginRtcSDK();
}

ImCalloutDlg::~ImCalloutDlg()
{
	AudioMgr::instance().stopPlaySound();
	closeMeetingDlg();
	logoutRtcSDK();

	if (g_sdkMain != nullptr)
	{
		g_sdkMain->getSDKMeeting().RmCallBack(this);
		g_sdkMain->RmCallBack(this);
	}
	UserStatusMgr::instance()->setIMCallStatus(false);

	delete ui;
}

void ImCalloutDlg::loginRtcSDK()
{
	CRIMMgrLogDebug("loginRtcSDK...");
	const App::CfgInfo &loginCfg = App::instance()->getCfgInfo();

	CRVSDK_WEBPROTOCOL webProtocol = CRVSDK_WEBPTC_HTTP;
	QString strSvr = loginCfg.lastSdkServer;
	int pos = strSvr.indexOf("://");
	if (pos > 0)
	{
		QString scheme = strSvr.left(pos).toLower();
		if (scheme == "https")
		{
			webProtocol = loginCfg.skipVerifyCert ? CRVSDK_WEBPTC_HTTPS_NOVERRIFY : CRVSDK_WEBPTC_HTTPS;
			strSvr = strSvr.mid(pos + 3);
		}
	}

	CRLoginDat loginDat;
	loginDat._serverAddr = qStrToCRStr(strSvr);
	loginDat._webProtocol = webProtocol;
	loginDat._sdkAuthType = (CRVSDK_AUTHTYPE)loginCfg.lastSdkAuthType == 1 ? CRVSDK_AUTHTP_TOKEN : CRVSDK_AUTHTP_SECRET;
	loginDat._appID = qStrToCRStr(loginCfg.lastSdkAppId);
	loginDat._md5_appSecret = qStrToCRStr(loginCfg.lastSdkSecret);
	loginDat._token = qStrToCRStr(loginCfg.lastSdkToken);
	loginDat._userID = qStrToCRStr(loginCfg.lastSdkUserID);

	g_sdkMain->login(loginDat);
}


void ImCalloutDlg::loginRslt(CRVSDK_ERR_DEF sdkErr, const char* cookie)
{
	CRIMMgrLogDebug("loginRtcSDK rslt: %d", sdkErr);
	if (sdkErr != CRVSDKERR_NOERR)
	{
		onCallinState(ST_Failed, tr("连接通话服务器失败(%1)！").arg(getVideoSDKErrDesc(sdkErr)));
		return;
	}

	_bRtcLogined = true;
	createMeeting();
}

void ImCalloutDlg::logoutRtcSDK()
{
	CRIMMgrLogDebug("logoutRtcSDK...");
	g_sdkMain->getSDKMeeting().exitMeeting();
	g_sdkMain->logout();
}

//创建会议
void ImCalloutDlg::createMeeting()
{
	CRIMMgrLogDebug("createMeeting...");
	onCallinState(ST_CreateMeet, tr("正在建立通话通道..."));
	g_sdkMain->createMeeting("", _taskID.toUtf8().constData());
}

void ImCalloutDlg::createMeetingSuccess(const CRMeetInfo& meetObj, const char* cookie)
{
	if (cookie != _taskID)
		return;

	CRIMMgrLogDebug("createMeeting success!");
	startCall(meetObj._ID);
}

void ImCalloutDlg::createMeetingFail(CRVSDK_ERR_DEF sdkErr, const char* cookie)
{
	if (cookie != _taskID)
		return;

	onCallinState(ST_Failed, tr("建立连接失败！"));
}


void ImCalloutDlg::startCall(int meetID)
{
	CRIMMgrLogDebug("startCall, meetID:%d, invitationMsgId:%s", meetID, _taskID.toUtf8().constData());
	onCallinState(ST_Calling, tr("正在呼叫..."));
	
	_sInfo.invitationMsgId = _taskID;
	_sInfo.inviterUserID = CRIMManager_Qt::instance()->getUserID();
	_sInfo.inviteeUserIDList.push_back(_fInfo.userID);
	_sInfo.timeout = CALL_TIMEOUT_SECOND;
	_sInfo.initiateTime = CRIMManager_Qt::instance()->getCurrentSvrTime() / 1000;
	_sInfo.roomID = QString::number(meetID);
	_sInfo.mediaType = getIMCallTypeName(_type);

	_timeout.start(_sInfo.timeout * 1000);
	CRIMManager_Qt::instance()->call(_sInfo);
}

void ImCalloutDlg::slot_callRejected(const CRIMInvitationInfo &info)
{
	if (!bTheSameInvitation(info, _sInfo))
	{
		return;
	}

	CRIMMgrLogDebug("callRejected...invitationMsgId:%s", info.invitationMsgId.toUtf8().constData());
	QString strReason = tr("对方挂断了呼叫！");
	if (info.opType == CRIVT_OP_Busy)
	{
		strReason = tr("对方忙，请稍后再拨！");
	}
	else if (info.opType == CRIVT_OP_Timeout)
	{
		strReason = tr("对方无应答，请稍后再拨！");
	}
	onCallinState(ST_Failed, strReason);
}

void ImCalloutDlg::slot_callAccepted(const CRIMInvitationInfo &info)
{
	if (!bTheSameInvitation(info, _sInfo))
	{
		return;
	}

	CRIMMgrLogDebug("callAccepted, invitationMsgId:%s", _taskID.toUtf8().constData());
	_sInfo.acceptTime = (info.acceptTime > 0) ? info.acceptTime : (CRIMManager_Qt::instance()->getCurrentSvrTime() / 1000);
	_bAccept = true;
	enterMeeting();
}

void ImCalloutDlg::hangupCall()
{
	CRIMMgrLogDebug("hungupCall, invitationMsgId:%s", _sInfo.invitationMsgId.toUtf8().constData());
	if (!_sInfo.invitationMsgId.isEmpty())
	{
		_sInfo.opType = CRIVT_OP_Undef;
		_sInfo.opDesc.clear();
		_sInfo.makeDuration();
		CRIMManager_Qt::instance()->hangUpCall(_sInfo);
		clearInvitation(_sInfo);
	}

	closeMeetingDlg();
	this->close();
}


void ImCalloutDlg::slot_notifyCallHungUp(const CRIMInvitationInfo &info)
{
	if (!bTheSameInvitation(info, _sInfo))
	{
		return;
	}

	CRIMMgrLogDebug("notifyCallHungUp...invitationMsgId:%s", info.invitationMsgId.toUtf8().constData());
	onCallinState(ST_Failed, tr("通话结束"));
	closeMeetingDlg();
	this->close();
}


void ImCalloutDlg::closeEvent(QCloseEvent *evt)
{
	QNoNCDialog::closeEvent(evt);

	CRIMMgrLogDebug("ImCalloutDlg closeEvent, invitationMsgId:%s", _sInfo.invitationMsgId.toUtf8().constData());
	if (!_sInfo.invitationMsgId.isEmpty())
	{
		if (_bAccept)
		{
			hangupCall();
		}
		else
		{
			_sInfo.opType = CRIVT_OP_Undef;
			_sInfo.opDesc = "cancel";
			CRIMManager_Qt::instance()->cancelCall(_sInfo);
			onCallinState(ST_Failed, tr("呼叫取消"));
		}
	}
}


void ImCalloutDlg::slot_timeout()
{
	CRIMMgrLogDebug("call out timeout, invitationMsgId:%s", _sInfo.invitationMsgId.toUtf8().constData());
	if (!_sInfo.invitationMsgId.isEmpty())
	{
		_sInfo.opType = CRIVT_OP_Timeout;
		_sInfo.opDesc = "timeout";
		CRIMManager_Qt::instance()->cancelCall(_sInfo);
	}
	onCallinState(ST_Failed, tr("对方没有应答！"));
}

void ImCalloutDlg::enterMeeting()
{
	if (UserStatusMgr::instance()->getMeetingStatus())
	{
		CRIMMgrLogDebug("enterMeeting failed: is in meeting! invitationMsgId:%s", _sInfo.invitationMsgId.toUtf8().constData());
		onCallinState(ST_Failed, tr("通话失败，已在会议中！"));
		return;
	}

	onCallinState(ST_Accepted, tr("正在建立通话..."));

	int meetID = _sInfo.roomID.toInt();
	CRIMMgrLogDebug("enterMeeting:%d, invitationMsgId:%s", meetID, _sInfo.invitationMsgId.toUtf8().constData());
	QString myName = CRIMManager_Qt::instance()->getSelfUserInfo().nickname;
	g_sdkMain->getSDKMeeting().enterMeeting(meetID, myName.toUtf8().constData());
}

void ImCalloutDlg::enterMeetingRslt(CRVSDK_ERR_DEF sdkErr)
{
	CRIMMgrLogDebug("enterMeetingRslt:%d, invitationMsgId:%s", sdkErr, _sInfo.invitationMsgId.toUtf8().constData());
	if (CRVSDKERR_NOERR == sdkErr)
	{
		onCallinState(ST_Meeting, "通话中");
		showMeetingDlg();
	}
	else
	{
		onCallinState(ST_Failed, tr("进入通话失败(%1)！").arg(getVideoSDKErrDesc(sdkErr)));
	}
}

void ImCalloutDlg::showMeetingDlg()
{
	int meetID = _sInfo.roomID.toInt();
	//显示主窗口，隐藏登录窗口
	_meetingMainDlg = new MeetingMainDialog(meetID, _type, App::instance()->getCfgInfo().lastSdkUserID, this->parentWidget());
	connect(_meetingMainDlg, &MeetingMainDialog::finished, this, &ImCalloutDlg::slot_onMeetingFinished);
	_meetingMainDlg->show();
}

void ImCalloutDlg::slot_onMeetingFinished()
{
	CRIMMgrLogDebug("onMeetingFinished, invitationMsgId:%s", _sInfo.invitationMsgId.toUtf8().constData());
	hangupCall();
}


void ImCalloutDlg::closeMeetingDlg()
{
	if (_meetingMainDlg != nullptr)
	{
		_meetingMainDlg->deleteLater();
		_meetingMainDlg = nullptr;
	}
	if (g_sdkMain)
	{
		g_sdkMain->getSDKMeeting().exitMeeting();
	}
}


void ImCalloutDlg::onCallinState(State st, const QString &msg)
{
	ui->desc->setText(msg);

	switch (st)
	{
	case ST_Login:
		{
			QString path = ":/Res/ring.wav";
			AudioMgr::instance().startPlaySound(path);
		}
		break;

	case ST_CreateMeet:
		break;
	case ST_Calling:
		break;
	case ST_Accepted:
		_timeout.stop();
		AudioMgr::instance().stopPlaySound();
		break;

	case ST_Meeting:
		this->hide();
		break;

	default:
		ui->cancelBtn->setText(tr("关闭"));
		clearInvitation(_sInfo);
		_timeout.stop();
		AudioMgr::instance().stopPlaySound();
		break;
	}

}
