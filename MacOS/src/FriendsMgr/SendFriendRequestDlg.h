#ifndef SendFriendRequestDlg_h
#define SendFriendRequestDlg_H

#include "DemoSvrWebAPI.h"
namespace Ui {class SendFriendRequestDlg;};

class SendFriendRequestDlg : public QNoNCDialog
{
	Q_OBJECT

public:
	SendFriendRequestDlg(const DemoSvrWebAPI::UserInfo &info, QWidget *parent = 0);
	~SendFriendRequestDlg();

protected slots:
	void slot_textChanged();
	void slot_sendBtnClicked();

	void slot_addFriendSuccess(const QString &dat, const QString &cookie);
	void slot_addFriendFailed(int code, const QString &err, const QString &cookie);

private:
	Ui::SendFriendRequestDlg *ui;
	DemoSvrWebAPI::UserInfo	_info;
};

#endif // SendFriendRequestDlg_H
