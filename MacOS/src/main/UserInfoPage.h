#ifndef USERINFOPAGE_H
#define USERINFOPAGE_H

#include "ui_UserInfoPage.h"
#include "DemoSvrWebAPI.h"

class UserInfoPage : public QNoNCDialog
{
	Q_OBJECT

public:
	UserInfoPage(QWidget *parent, bool bDlgMode=false);
	~UserInfoPage();

	void show(const QString &uid, const QString &grpID = QString(), const QPoint &pos = QPoint());
	void show(const DemoSvrWebAPI::UserInfo &info, const QPoint &pos = QPoint());

protected slots:
	void slot_queryUserInfoRslt(const QString &errMsg, const DemoSvrWebAPI::UserInfo &info);
	void slot_editBtnClicked();
	void slot_sendMsgBtnClicked();
	void slot_addFriendBtnClicked();

protected:
	bool event(QEvent *e) override;

	void updateUserInfoToUI();
	void updateDlgPos(const QPoint &pos);

private:
	Ui::UserInfoPage* ui;
	bool m_bDlgMode{ false };
	DemoSvrWebAPI::UserInfo m_uInfo;
};

#endif // USERINFOPAGE_H
