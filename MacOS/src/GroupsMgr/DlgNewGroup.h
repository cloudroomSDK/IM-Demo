#ifndef DLGNEWGROUP_H
#define DLGNEWGROUP_H

#include "ui_DlgNewGroup.h"

class DlgNewGroup : public QNoNCDialog
{
	Q_OBJECT

public:
	DlgNewGroup(const QSet<QString> &initUsrIds, QWidget *parent = 0);
	~DlgNewGroup();

protected:
	void slot_btnOkClicked();
	void slot_btnCancelClicked();
	void slot_createGroupSuccess(const CRIMGrpInfo &, const QString &cookie);
	void slot_createGroupFailed(int code, const QString &error, const QString &cookie);

private:
	Ui::DlgNewGroup ui;

};

#endif // DlgNewGroup_h
