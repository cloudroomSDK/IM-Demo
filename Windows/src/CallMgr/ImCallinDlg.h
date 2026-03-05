#ifndef ImCallinDlg_h
#define ImCallinDlg_h

#include "ImCallDef.h"

namespace Ui {class ImCallinDlg;};
class MeetingMainDialog;

class ImCallinDlg : public QNoNCDialog, public CRVideoSDKMainCallBack, public CRVideoSDKMeetingCallBack
{
	Q_OBJECT

public:
	ImCallinDlg(const CRIMInvitationInfo &msg, QWidget *parent = 0);
	~ImCallinDlg();

protected slots:
	void slot_acceptBtnClicked();
	void slot_timeout();

	void slot_notifyCallCancelled(const CRIMInvitationInfo &info);
	void slot_notifyCallAcceptedByOtherDevice(const CRIMInvitationInfo &info);
	void slot_notifyCallRejectedByOtherDevice(const CRIMInvitationInfo &info);
	void slot_notifyCallHungUp(const CRIMInvitationInfo &info);
	void slot_onMeetingFinished();

protected:
	void loginRslt(CRVSDK_ERR_DEF sdkErr, const char* cookie) override;
	void enterMeetingRslt(CRVSDK_ERR_DEF sdkErr) override;
	void closeEvent(QCloseEvent *evt) override;

protected:
	//登录音视频sdk
	void loginRtcSDK();
	void logoutRtcSDK();
	//进入房间
	void enterMeeting();
	//通话
	void showMeetingDlg();
	void closeMeetingDlg();
	//挂断
	void hangupCall();

	enum State {
		ST_Failed = 0,
		ST_Login,
		ST_Calling,
		ST_Accepted,
		ST_Meeting,
	};
	void onCallinState(State st, const QString &msg);

private:
	Ui::ImCallinDlg *ui;
	QTimer			_timeout;

	CRIMFriendInfo	_fInfo;
	IMCallType		_type;
	CRIMInvitationInfo _sInfo;
	bool			_bRtcLogined{ false };
	bool			_bAccept{ false };

	MeetingMainDialog* _meetingMainDlg{ nullptr };
};

#endif // ImCallinDlg_h
