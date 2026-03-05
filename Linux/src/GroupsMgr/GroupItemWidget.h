#ifndef GROUPITEMWIDGET_H
#define GROUPITEMWIDGET_H

#include "ui_GroupItemWidget.h"

const QString g_createNewGrp = "\x01_creategrp";

class GroupListItem : public QListWidgetItem
{
public:
	GroupListItem(const QString &grpID, QListWidget *view);
	~GroupListItem(); 

	void updateInfo(const QString &grpID);
	QString getGrpID() { return m_grpID; }

protected:
	bool operator<(const QListWidgetItem &other) const override;

private:
	QString		m_grpID;
	QListWidget *m_listView;
};


///////////////////////////////////////////////
class GroupItemWidget : public QWidget
{
	Q_OBJECT

public:
	GroupItemWidget(GroupListItem *item, QWidget *parent = 0);
	~GroupItemWidget();
	
	void updateUI();

private:
	Ui::GroupItemWidget ui;
	GroupListItem *m_item;
};

#endif // GROUPITEMWIDGET_H
