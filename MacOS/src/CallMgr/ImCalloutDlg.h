#ifndef ImCalloutDlg_H
#define ImCalloutDlg_H

#include "ImCallDef.h"

class ImCallMgr;
class MeetingMainDialog;
namespace Ui {class ImCalloutDlg;};

class ImCalloutDlg : public QNoNCDialog, public CRVideoSDKMainCallBack, public CRVideoSDKMeetingCallBack
{
	Q_OBJECT

public:
	ImCalloutDlg(IMCallType type, const QString &userID, QWidget *parent = 0);
	~ImCalloutDlg();

protected slots:
	void slot_callAccepted(const CRIMInvitationInfo &info);
	void slot_callRejected(const CRIMInvitationInfo &info);
	void slot_notifyCallHungUp(const CRIMInvitationInfo &info);
	void slot_timeout();
	void slot_onMeetingFinished();

protected:
	void loginRslt(CRVSDK_ERR_DEF sdkErr, const char* cookie) override;
	void enterMeetingRslt(CRVSDK_ERR_DEF sdkErr) override;
	void closeEvent(QCloseEvent *evt) override;

protected:
	//登录音视频sdk
	void loginRtcSDK();
	void logoutRtcSDK();
	//创建会议
	void createMeeting();
	void createMeetingSuccess(const CRMeetInfo& meetObj, const char* cookie) override;
	void createMeetingFail(CRVSDK_ERR_DEF sdkErr, const char* cookie) override;
	//呼叫
	void startCall(int meetID);
	void hangupCall();
	//进入房间
	void enterMeeting();
	//通话
	void showMeetingDlg();
	void closeMeetingDlg();


	enum State {
		ST_Failed = 0,
		ST_Login,
		ST_CreateMeet,
		ST_Calling,
		ST_Accepted,
		ST_Meeting,
	};
	void onCallinState(State st, const QString &msg);

private:
	Ui::ImCalloutDlg *ui;
	QTimer			_timeout;

	CRIMFriendInfo	_fInfo;
	IMCallType		_type;
	QString			_taskID;

	CRIMInvitationInfo	_sInfo;
	bool			_bRtcLogined{ false };
	bool			_bAccept{ false };

	MeetingMainDialog* _meetingMainDlg{ nullptr };
};

#endif // ImCalloutDlg_H
