#ifndef GROUPDETAILWIDGET_H
#define GROUPDETAILWIDGET_H

#include "ui_GroupDetailWidget.h"


class GrpMemberListItem;

class GroupDetailWidget : public QWidget
{
	Q_OBJECT

public:
	GroupDetailWidget(QWidget *parent = 0);
	~GroupDetailWidget();
	
	void setGroupInfo(const QString &grpID);
	Ui::GroupDetailWidget ui;

signals:
	void s_btnClearGroupClicked();
	void s_btnDismissGroupClicked();
	void s_btnQuitGroupClicked();

protected slots:
	void slot_MemberListCtrlBtnToggled();
	void slot_MemberListItemClicked(QListWidgetItem *item);

	void slot_commitGroupName(const QString &oldStr, const QString &newStr);
	void slot_commitMyName(const QString &oldStr, const QString &newStr);

	void slot_btnPinStateChanged(int state);
	void slot_btnDndStateChanged(int state);
	void slot_btnForbidStateChanged(int state);

	void slot_btnGroupChatClicked();
	void slot_btnClearGroupClicked();
	void slot_btnDismissGroupClicked();
	void slot_btnQuitGroupClicked();

	void slot_grpInfoChanged(const CRIMGrpInfo &info);
	void slot_grpMemberInfoChanged(const CRIMGrpMemberInfo &info);
	void slot_grpMemberChanged();

protected:
	bool eventFilter(QObject *watched, QEvent *event);

	GrpMemberListItem* getMemberItem(const QString &userID);

private:
	QString		m_grpID;
};

#endif // GROUPDETAILWIDGET_H
