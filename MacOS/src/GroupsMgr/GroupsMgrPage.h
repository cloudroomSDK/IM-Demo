#ifndef GROUPSMGRPAGE_H
#define GROUPSMGRPAGE_H

#include "ui_GroupsMgrPage.h"

class GroupListItem;
class GroupsMgrPage : public QFrame
{
	Q_OBJECT

public:
	GroupsMgrPage(QWidget *parent = 0);
	~GroupsMgrPage();

	struct GroupSearchDat
	{
		const CRIMGrpInfo_Ex* _grp;
		const CRIMGrpMemberInfo* _meb;
	};
	QList<GroupSearchDat> searchGroups(const QString &str);

protected slots:
	void slot_syncServerFinish();
	void slot_newGroupClicked();
	void slot_itemClicked(QListWidgetItem *item);
	void slot_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

	void slot_joinedGrpAdded(const CRIMGrpInfo &info);
	void slot_grpInfoChanged(const CRIMGrpInfo &info);
	void slot_joinedGrpDeleted(const CRIMGrpInfo &info);

	void slot_showGrpMemberInfo();
	void slot_cfgGrp();

protected:
	void addGroupItem(const CRIMGrpInfo_Ex *);
	void removeGroupItem();
	void updateGroupItem();

	GroupListItem* getItemByGrpID(const QString &grpID);

private:
	Ui::GroupsMgrPage ui;
};

#endif // GROUPSMGRPAGE_H
