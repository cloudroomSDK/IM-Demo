#include "stdafx.h"
#include "ImCallinDlg.h"
#include "ui_ImCallinDlg.h"
#include "AudioMgr.h"
#include "MainDialog.h"
#include "UserStatusMgr.h"
#include "MeetingMainDialog.h"

#define ICON_SIZE	60

ImCallinDlg::ImCallinDlg(const CRIMInvitationInfo &info, QWidget *parent) : QNoNCDialog(parent)
{
	const CRIMFriendInfo *pInfo = CRIMManager_Qt::instance()->getFriendInfo(info.inviterUserID);
	if (pInfo == NULL)
	{
		CRIMMgrLogDebug("im call in failed! userid:%s not exist!", info.inviterUserID.toUtf8().constData());
		this->deleteLater();
		return;
	}
	UserStatusMgr::instance()->setIMCallStatus(true);
	g_sdkMain->AddCallBack(this);
	g_sdkMain->getSDKMeeting().AddCallBack(this);


	StandardTitleBar *pTitleBar = new StandardTitleBar(this, StandardTitleBar::TBTN_CLOSE);
	this->setTitleBar(pTitleBar);

	QWidget *pClient = new QWidget(this);
	ui = new Ui::ImCallinDlg();
	ui->setupUi(pClient);
	this->setClientWidget(pClient);
	this->setAttribute(Qt::WA_DeleteOnClose);
	this->cfgCloseType(false);

	_sInfo = info;
	_type = getIMCallType(_sInfo.mediaType);
	_fInfo = *pInfo;
	this->setWindowTitle(_type == IMAudioCall ? tr("语音呼叫") : tr("视频呼叫"));
	ImIconMgr::instance()->bindIcon(ui->icon, IMIconInfo(_fInfo.userID, IMICON_USR, QSize(ICON_SIZE, ICON_SIZE)));
	ui->name->setText(QString("%1").arg(_fInfo.getDisplayName()));

	QString desc;
	if (_sInfo.groupID.isEmpty())
	{
		desc = (_type == IMAudioCall ? tr("语音通话") : tr("视频通话"));
	}
	else
	{
		desc = (_type == IMAudioCall ? tr("多人语音通话") : tr("多人视频通话"));
	}
	onCallinState(ST_Login, tr("%1邀请我进行%2。").arg(_fInfo.getDisplayName()).arg(desc.toLower()));

	connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_notifyCallCancelled, this, &ImCallinDlg::slot_notifyCallCancelled);
	connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_notifyCallAcceptedByOtherDevice, this, &ImCallinDlg::slot_notifyCallAcceptedByOtherDevice);
	connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_notifyCallRejectedByOtherDevice, this, &ImCallinDlg::slot_notifyCallRejectedByOtherDevice);
	connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_notifyCallHungUp, this, &ImCallinDlg::slot_notifyCallHungUp);

	connect(ui->acceptBtn, &QToolButton::clicked, this, &ImCallinDlg::slot_acceptBtnClicked);
	connect(ui->rejectBtn, &QToolButton::clicked, this, &ImCallinDlg::close);
	connect(&_timeout, &QTimer::timeout, this, &ImCallinDlg::slot_timeout);
	_timeout.setSingleShot(true);
	if (_sInfo.timeout <= 0)
	{
		_sInfo.timeout = 30;
	}
	_timeout.start(_sInfo.timeout * 1000);

	//登录音视频sdk
	loginRtcSDK();
}

ImCallinDlg::~ImCallinDlg()
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

void ImCallinDlg::loginRtcSDK()
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

void ImCallinDlg::loginRslt(CRVSDK_ERR_DEF sdkErr, const char* cookie)
{
	CRIMMgrLogDebug("loginRtcSDK rslt: %d", sdkErr);
	if (sdkErr == CRVSDKERR_NOERR)
	{
		_bRtcLogined = true;
		if (_bAccept)
		{
			enterMeeting();
		}
	}
	else
	{
		onCallinState(ST_Failed, tr("连接通话服务器失败(%1)！").arg(getVideoSDKErrDesc(sdkErr)));
	}
}

void ImCallinDlg::logoutRtcSDK()
{
	CRIMMgrLogDebug("logoutRtcSDK...");
	if (g_sdkMain)
	{
		g_sdkMain->getSDKMeeting().exitMeeting();
		g_sdkMain->logout();
	}
}


void ImCallinDlg::slot_notifyCallCancelled(const CRIMInvitationInfo &info)
{
	if (!bTheSameInvitation(info, _sInfo))
		return;

	CRIMMgrLogDebug("onCallCancelled...invitationMsgId:%s", info.invitationMsgId.toUtf8().constData());
	clearInvitation(_sInfo);
	this->close();
}

void ImCallinDlg::slot_notifyCallAcceptedByOtherDevice(const CRIMInvitationInfo &info)
{
	if (!bTheSameInvitation(info, _sInfo))
		return;

	CRIMMgrLogDebug("onCallAcceptedByOtherDevice...invitationMsgId:%s", info.invitationMsgId.toUtf8().constData());
	onCallinState(ST_Failed, tr("已在其它设备接听"));
	this->close();
}

void ImCallinDlg::slot_notifyCallRejectedByOtherDevice(const CRIMInvitationInfo &info)
{
	if (!bTheSameInvitation(info, _sInfo))
		return;

	CRIMMgrLogDebug("onCallRejectedByOtherDevice...invitationMsgId:%s", info.invitationMsgId.toUtf8().constData());
	onCallinState(ST_Failed, tr("已在其它设备拒绝"));
	this->close();
}

void ImCallinDlg::slot_notifyCallHungUp(const CRIMInvitationInfo &info)
{
	if (!bTheSameInvitation(info, _sInfo))
		return;

	CRIMMgrLogDebug("notifyCallHungUp...invitationMsgId:%s", info.invitationMsgId.toUtf8().constData());
	onCallinState(ST_Failed, tr("通话结束"));
	closeMeetingDlg();
	this->close();
}

void ImCallinDlg::closeEvent(QCloseEvent *evt)
{
	QNoNCDialog::closeEvent(evt);

	CRIMMgrLogDebug("ImCallinDlg closeEvent...invitationMsgId:%s", _sInfo.invitationMsgId.toUtf8().constData());
	if (!_sInfo.invitationMsgId.isEmpty())
	{
		if (_bAccept)
		{
			hangupCall();
		}
		else
		{
			CRIMInvitationInfo newInfo(_sInfo);
			newInfo.opType = CRIVT_OP_Reject;
			newInfo.opDesc = "reject";
			CRIMManager_Qt::instance()->rejectCall(newInfo);
			onCallinState(ST_Failed, tr("已拒绝"));
		}
	}
}

void ImCallinDlg::slot_acceptBtnClicked()
{
	CRIMMgrLogDebug("acceptBtnClicked...invitationMsgId:%s", _sInfo.invitationMsgId.toUtf8().constData());
	_bAccept = true;
	_sInfo.acceptTime = CRIMManager_Qt::instance()->getCurrentSvrTime()/1000;
	CRIMManager_Qt::instance()->acceptCall(_sInfo);

	onCallinState(ST_Accepted, tr("正在建立通话连接..."));
	if (_bRtcLogined)
	{
		enterMeeting();
	}
}

void ImCallinDlg::slot_timeout()
{
	CRIMMgrLogDebug("call in timeout, invitationMsgId:%s", _sInfo.invitationMsgId.toUtf8().constData());
	//被叫端超时只关闭窗口，不发出拒绝消息（避免源端与源端消息重复）
	onCallinState(ST_Failed, tr("超时结束"));
	this->close();
}

void ImCallinDlg::enterMeeting()
{
	int meetID  = _sInfo.roomID.toInt();
	if (UserStatusMgr::instance()->getMeetingStatus())
	{
		CRIMMgrLogDebug("enterMeeting failed: is in meeting! invitationMsgId:%s", _sInfo.invitationMsgId.toUtf8().constData());
		onCallinState(ST_Failed, tr("通话失败，已在会议中！"));
		return;
	}

	CRIMMgrLogDebug("enterMeeting:%d, invitationMsgId:%s", meetID, _sInfo.invitationMsgId.toUtf8().constData());
	QString myName = CRIMManager_Qt::instance()->getSelfUserInfo().nickname;
	g_sdkMain->getSDKMeeting().enterMeeting(meetID, myName.toUtf8().constData());
}

void ImCallinDlg::enterMeetingRslt(CRVSDK_ERR_DEF sdkErr)
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

void ImCallinDlg::showMeetingDlg()
{
	int meetID = _sInfo.roomID.toInt();
	//显示主窗口，隐藏登录窗口
	_meetingMainDlg = new MeetingMainDialog(meetID, _type, App::instance()->getCfgInfo().lastSdkUserID, this->parentWidget());
	connect(_meetingMainDlg, &MeetingMainDialog::finished, this, &ImCallinDlg::slot_onMeetingFinished);
	_meetingMainDlg->show();
}

void ImCallinDlg::slot_onMeetingFinished()
{
	CRIMMgrLogDebug("onMeetingFinished, invitationMsgId:%s", _sInfo.invitationMsgId.toUtf8().constData());
	hangupCall();
}

void ImCallinDlg::closeMeetingDlg()
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

void ImCallinDlg::hangupCall()
{
	CRIMMgrLogDebug("hangupCall, invitationMsgId:%s", _sInfo.invitationMsgId.toUtf8().constData());
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


void ImCallinDlg::onCallinState(State st, const QString &msg)
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

	case ST_Calling: 
		break;

	case ST_Accepted:
		ui->acceptBtn->hide();
		_timeout.stop();
		AudioMgr::instance().stopPlaySound();
		break;

	case ST_Meeting:
		this->hide();
		break;

	default:
		ui->acceptBtn->hide();
		ui->rejectBtn->setText(tr("关闭"));
		clearInvitation(_sInfo);
		_timeout.stop();
		AudioMgr::instance().stopPlaySound();
		break;
	}

}
