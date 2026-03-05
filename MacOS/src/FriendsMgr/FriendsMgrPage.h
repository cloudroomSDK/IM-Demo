#ifndef FRIENDSMGRPAGE_H
#define FRIENDSMGRPAGE_H

#include "ui_FriendsMgrPage.h"
class FriendListItem;
class FriendsMgrPage : public QFrame
{
	Q_OBJECT

public:
	FriendsMgrPage(QWidget *parent = 0);
	~FriendsMgrPage();

	enum SearchMatchType{
		CSMT_ID = 0,
		CSMT_NAME,
		CSMT_REMARK,
		CSMT_PHONE,
		CSMT_EMAIL,

		CSMT_BUTT
	};
	struct FriendSearchDat
	{
		const CRIMFriendInfo* _frd;
		SearchMatchType _matchType;
	};

	QList<FriendSearchDat> searchFriends(const QString &str);

signals:
	void s_firendReqMsgChanged(bool hasFriendReqMsg);

protected slots:
	void slot_syncServerFinish();
	void slot_friendAdded(const CRIMFriendInfo &info);
	void slot_friendDeleted(const CRIMFriendInfo &info);
	void slot_firendReqMsgChanged(bool hasFriendReqMsg);

	void slot_itemClicked(QListWidgetItem *item);
	void slot_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

	void slot_addFriendTriggered();
	void slot_newFriendTriggered();
	void slot_delFriendTriggered();
	void slot_showFriendInfoTriggered();
	void slot_cfgFriendTriggered();

protected:
	void initList();
	void addFriendItem(const CRIMFriendInfo &info);
	void updateFriendItem(FriendListItem* pItem, const CRIMFriendInfo &info);
	FriendListItem* getItemByUserID(const QString& userID);
	int getRowByUserID(const QString& userID);

private:
	Ui::FriendsMgrPage ui;
};

#endif // FRIENDSMGRPAGE_H
