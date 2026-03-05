#include "stdafx.h"
#include "MeetingMainDialog.h"
#include "UserStatusMgr.h"

#define	VIDEO_WH_16_9 (float(16.0)/9)
#define	VIDEO_WH_9_16 (float(9.0)/16)
#define SMALLVIDEO_SPCE 3
MeetingMainDialog::MeetingMainDialog(int meetid, IMCallType imCallType, const QString &userId, QWidget *parent)
	: QNoNCDialog(parent)
{
 	QWidget *pClient = new QWidget(this);
	ui.setupUi(pClient);

	this->cfgCloseType(false);
	this->setClientWidget(pClient);
	this->setWindowTitle(App::instance()->getUsrLoginAcnt());

	m_myUserId = qStrToCRStr(userId);
	g_sdkMain->getSDKMeeting().AddCallBack(this);
	UserStatusMgr::instance()->setMeetingStatus(true);

	connect(ui.btnMic, &QAbstractButton::clicked, this, &MeetingMainDialog::slot_btnMicClicked);
	connect(ui.btnVideo, &QAbstractButton::clicked, this, &MeetingMainDialog::slot_btnVideoClicked);
	connect(ui.btnExit, &QAbstractButton::clicked, this, &MeetingMainDialog::slot_btnExitClicked);
	ui.bigVideo->setFixedSize(300, 300 / VIDEO_WH_9_16);
 	ui.smallVideo->setFixedSize(ui.bigVideo->width() / 2, ui.bigVideo->width() / 2 / VIDEO_WH_16_9);
	ui.smallVideo->move(ui.bigVideo->width() - ui.smallVideo->width() - SMALLVIDEO_SPCE, SMALLVIDEO_SPCE);
	ui.bottomWidget->setFixedSize(ui.bigVideo->width(), 70);
	ui.bottomWidget->move(0, ui.bigVideo->height() - ui.bottomWidget->height());

	init();
	//开启麦克风
	g_sdkMain->getSDKMeeting().openMic(m_myUserId.constData());
	if (imCallType == IMVideoCall)
	{
		//开启摄像头
		g_sdkMain->getSDKMeeting().openVideo(m_myUserId.constData());
	}
}

MeetingMainDialog::~MeetingMainDialog()
{
	if (g_sdkMain != nullptr)
	{
		g_sdkMain->getSDKMeeting().RmCallBack(this);
	}
	UserStatusMgr::instance()->setMeetingStatus(false);
}

void MeetingMainDialog::init()
{
	updateVideoUis();
	updateMicStatus();
	updateVideoStatus();
}

void MeetingMainDialog::updateVideoUis()
{
	//设置自己的视频
	CRUserVideoID myUserVideoId(m_myUserId, -1);
	ui.smallVideo->setVideoInfo(myUserVideoId);

	//获取其他人
	CRUserVideoID peerUserVideoId;
	CRBase::CRArray<CRMeetingMember> allMems = g_sdkMain->getSDKMeeting().getAllMembers();
	for(uint32_t i = 0; i < allMems.count(); i++)
	{
		const CRMeetingMember &mem = allMems.item(i);
		if (mem._userId != m_myUserId)
		{
			peerUserVideoId = CRUserVideoID(mem._userId, -1);
			break;
		}
	}
	ui.bigVideo->setVideoInfo(peerUserVideoId);
}

void MeetingMainDialog::updateMicStatus()
{
	CRVSDK_ASTATUS aStatus = g_sdkMain->getSDKMeeting().getMicStatus(m_myUserId.constData());
	QString icon = aStatus == CRVSDK_AST_OPEN ? ":/Res/mic_open.svg" : ":/Res/mic_close.svg";
	ui.btnMic->setIcon(QIcon(icon));
}

void MeetingMainDialog::updateVideoStatus()
{
	CRVSDK_VSTATUS vStatus = g_sdkMain->getSDKMeeting().getVideoStatus(m_myUserId.constData());
	QString icon = vStatus == CRVSDK_VST_OPEN ? ":/Res/video_open.svg" : ":/Res/video_close.svg";
	ui.btnVideo->setIcon(QIcon(icon));
	if (vStatus == CRVSDK_VST_OPEN)
	{
		ui.smallVideo->show();
	}
	else
	{
		ui.smallVideo->hide();
	}
}

void MeetingMainDialog::slot_btnMicClicked()
{
	CRVSDK_ASTATUS aStatus = g_sdkMain->getSDKMeeting().getMicStatus(m_myUserId.constData());
	if (aStatus == CRVSDK_AST_OPEN)
	{
		g_sdkMain->getSDKMeeting().closeMic(m_myUserId.constData());
	}
	else
	{
		g_sdkMain->getSDKMeeting().openMic(m_myUserId.constData());
	}
}

void MeetingMainDialog::slot_btnVideoClicked()
{
	CRVSDK_VSTATUS vStatus = g_sdkMain->getSDKMeeting().getVideoStatus(m_myUserId.constData());
	if (vStatus == CRVSDK_VST_OPEN)
	{
		g_sdkMain->getSDKMeeting().closeVideo(m_myUserId.constData());
	}
	else
	{
		g_sdkMain->getSDKMeeting().openVideo(m_myUserId.constData());
	}
}

void MeetingMainDialog::slot_btnExitClicked()
{
	this->close();
}

void MeetingMainDialog::closeEvent(QCloseEvent *e)
{
	g_sdkMain->getSDKMeeting().exitMeeting();
	QNoNCDialog::closeEvent(e);
}

void MeetingMainDialog::enterEvent(QEvent *event)
{
	ui.bottomWidget->show();
}

void MeetingMainDialog::leaveEvent(QEvent *event)
{
	ui.bottomWidget->hide();
}

//成员进入
void MeetingMainDialog::notifyUserEnterMeeting(const char* userID)
{
	updateVideoUis();
}

//成员离开
void MeetingMainDialog::notifyUserLeftMeeting(const char* userID)
{
	updateVideoUis();
}

//麦克风状态变化
void MeetingMainDialog::notifyMicStatusChanged(const char* userID, CRVSDK_ASTATUS oldStatus, CRVSDK_ASTATUS newStatus, const char* oprUserID)
{
	if (m_myUserId != userID)
	{
		return;
	}
	updateMicStatus();
}

//摄像头状态变化
void MeetingMainDialog::notifyVideoStatusChanged(const char* userID, CRVSDK_VSTATUS oldStatus, CRVSDK_VSTATUS newStatus, const char* oprUserID)
{
	if (m_myUserId != userID)
	{
		return;
	}
	updateVideoStatus();
}

void MeetingMainDialog::notifyMeetingStopped()
{
	this->close();
}