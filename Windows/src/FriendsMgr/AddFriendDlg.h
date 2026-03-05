#ifndef ADDFRIENDDLG_H
#define ADDFRIENDDLG_H

#include "DemoSvrWebAPI.h"
#include <QWidget>
namespace Ui {class AddFriendDlg;};

class AddFriendDlg : public QNoNCDialog
{
	Q_OBJECT

public:
	AddFriendDlg(QWidget *parent = 0);
	~AddFriendDlg();

protected slots:
	void slot_updateSearchBtnState();
	void on_searchBtn_clicked();
	void slot_searchUserInfoRslt(const QString &errMsg, const DemoSvrWebAPI::UserInfo &uInfo);

protected:
	void showDesc(const QString &txt);

private:
	Ui::AddFriendDlg *ui;
	QString		_searching;
};

#endif // ADDFRIENDDLG_H
