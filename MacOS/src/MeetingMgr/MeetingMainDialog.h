#ifndef MeetingMainDialog_H
#define MeetingMainDialog_H

#include "ui_MeetingMainDialog.h"

class MeetingMainDialog : public QNoNCDialog, public CRVideoSDKMeetingCallBack
{
	Q_OBJECT

public:
	MeetingMainDialog(int meetid, IMCallType imCallType, const QString &userId, QWidget *parent = 0);
	~MeetingMainDialog();

protected:
	void closeEvent(QCloseEvent *) override;
	void enterEvent(QEvent *event) override;
	void leaveEvent(QEvent *event) override;
	void notifyMeetingStopped() override;
	void notifyUserEnterMeeting(const char* userID) override;
	void notifyUserLeftMeeting(const char* userID) override;
	void notifyMicStatusChanged(const char* userID, CRVSDK_ASTATUS oldStatus, CRVSDK_ASTATUS newStatus, const char* oprUserID) override;
	void notifyVideoStatusChanged(const char* userID, CRVSDK_VSTATUS oldStatus, CRVSDK_VSTATUS newStatus, const char* oprUserID) override;

protected slots:
	void slot_btnMicClicked();
	void slot_btnVideoClicked();
	void slot_btnExitClicked();

protected:
	void init();
	void updateVideoUis();
	void updateMicStatus();
	void updateVideoStatus();

private:
	Ui::MeetingMainDialog		ui;
	CRBase::CRString m_myUserId;

};

#endif // MeetingMainDialog_H
