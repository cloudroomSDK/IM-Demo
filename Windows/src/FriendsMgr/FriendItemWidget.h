#ifndef FRIENDITEMWIDGET_H
#define FRIENDITEMWIDGET_H

#include "ui_FriendItemWidget.h"

const QString g_strAddFriend = "\x01_1";
const QString g_strNewFriendMsg = "\x01_2";

class FriendListItem : public QListWidgetItem
{
public:
	FriendListItem(QListWidget *view) : QListWidgetItem(view) {}
	~FriendListItem() {}

	void setItemDat(const CRIMFriendInfo *info);
	QString getUserID() { return m_userID; }

protected:
	virtual bool operator<(const QListWidgetItem &other) const
	{
		const FriendListItem* pOther = static_cast<const FriendListItem*>(&other);
		return m_gbkName < pOther->m_gbkName;
	}

private:
	QString		m_userID;
	QByteArray	m_gbkName;
};


///////////////////////////////////////////////
class FriendItemWidget : public QFrame
{
	Q_OBJECT

public:
	FriendItemWidget(FriendListItem *item, QWidget *parent = 0);
	~FriendItemWidget();
	
	void updateUI();
	void setNewDatRedTipVisible(bool b) {  if (m_newDatRedTip != nullptr)	m_newDatRedTip->setVisible(b); }

private:
	Ui::FriendItemWidget ui;
	FriendListItem *m_item;
	QWidget*		m_newDatRedTip{ nullptr };
};

#endif // FRIENDITEMWIDGET_H
