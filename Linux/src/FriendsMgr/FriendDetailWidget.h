#ifndef FRIENDDETAILWIDGET_H
#define FRIENDDETAILWIDGET_H

#include "ui_FriendDetailWidget.h"


///////////////////////////////////////////////
class FriendDetailWidget : public QWidget
{
	Q_OBJECT

public:
	FriendDetailWidget(QWidget *parent = 0);
	~FriendDetailWidget();
	
	void setFriendInfo(const QString &userID);
	void setHideChatBtn(bool bHide)
	{
		ui.btnFriendChat->setVisible(!bHide);
	}

signals:
	void s_clearMsgClicked();
	void s_delFriendClicked();
	void s_videoCallClicked();
	void s_audioCallClicked();

protected slots:
	void slot_queryUserInfoRslt(const QString &errMsg, const DemoSvrWebAPI::UserInfo &info);

	void slot_commitRemark(const QString &oldStr, const QString &newStr);
	void slot_btnPinStateChanged(int state);
	void slot_btnDndStateChanged(int state);
	void slot_btnPrivateChatChanged(int state);
	void slot_currentIndexChanged(int index);

	void slot_btnFriendChatClicked();
	void slot_audioChatBtnCliecked();
	void slot_videoChatBtnCliecked();
	void slot_btnClearFriendClicked();
	void slot_btnDelFriendClicked();

protected:
	void updateUserInfoToUI();
	void udpateBurnDuration();


private:
	Ui::FriendDetailWidget ui;
	DemoSvrWebAPI::UserInfo m_uInfo;
	bool	m_bInitializing{ false };
};

#endif // FRIENDDETAILWIDGET_H
