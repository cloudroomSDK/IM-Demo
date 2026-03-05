#ifndef DLGADDGROUPMEMBER_H
#define DLGADDGROUPMEMBER_H

#include "ui_DlgAddGroupMember.h"

class DlgAddGroupMember : public QNoNCDialog
{
	Q_OBJECT

public:
	DlgAddGroupMember(const QString &groupId, QWidget *parent = 0);
	~DlgAddGroupMember();

protected:
	void slot_btnOkClicked();
	void slot_btnCancelClicked();
	void slot_inviteUserToGrpFailed(int code, const QString &error, const QString &cookie);

private:
	Ui::DlgAddGroupMember ui;
	QString m_groupId;
};

#endif // DlgAddGroupMember_h
