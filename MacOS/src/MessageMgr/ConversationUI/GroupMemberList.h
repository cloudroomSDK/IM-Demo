#ifndef GROUPMEMBERLIST_H
#define GROUPMEMBERLIST_H

#include "ui_GroupHostItemWidget.h"

const QString g_addGroupMemberStr = "_cr_addGrpMember_";

class GrpMemberListItem : public ListWidgetItemWithIMIcon
{
public:
	GrpMemberListItem() {}
	~GrpMemberListItem() {}

	void updateMemberInfo(const CRIMGrpMemberInfo &info)
	{
		m_userID = info.userID;
		m_roleLevel = info.roleLevel;
		if (m_nickName != info.nickname)
		{
			m_nickName = info.nickname;
			m_gbkName = QTextCodec::codecForName("gbk")->fromUnicode(info.nickname.toLower());
		}
		QString tooltip = m_nickName;
		if (m_roleLevel == CRIM_GMR_Owner)
		{
			tooltip += QObject::tr("（群主）");
		}
		else if (m_roleLevel == CRIM_GMR_Admin)
		{
			tooltip += QObject::tr("（管理员）");
		}
		setText(m_nickName);
		setToolTip(tooltip);
	}

	bool isGroupManager()
	{
		return m_roleLevel == CRIM_GMR_Owner || m_roleLevel == CRIM_GMR_Admin;
	}

	virtual bool operator<(const QListWidgetItem &other) const
	{
		const GrpMemberListItem* pOtherMember = static_cast<const GrpMemberListItem*>(&other);
		//添加按钮排最后
		if (m_userID == g_addGroupMemberStr)
			return false;
		if (pOtherMember->m_userID == g_addGroupMemberStr)
			return true;

		//群主排第一
		if (m_roleLevel == CRIM_GMR_Owner)
			return true;
		if (pOtherMember->m_roleLevel == CRIM_GMR_Owner)
			return false;

		//群管理员排第二
		if (m_roleLevel == CRIM_GMR_Admin)
			return true;
		if (pOtherMember->m_roleLevel == CRIM_GMR_Admin)
			return false;

		//我排第三
		if (m_userID == CRIMManager_Qt::instance()->getUserID())
			return true;
		if (pOtherMember->m_userID == CRIMManager_Qt::instance()->getUserID())
			return false;

		//昵称
		return m_gbkName < pOtherMember->m_gbkName;
	}
public:
	QString m_userID;
	CRIMGroupMemberRole m_roleLevel;
	QString m_nickName;
	QByteArray m_gbkName;
};

#define ToMemberListItem(a) (static_cast<GrpMemberListItem*>(a))

class GroupHostItemWidget :public QWidget
{
public:
	GroupHostItemWidget(QWidget *parent = 0);
	Ui::GroupHostItemWidget ui;

protected:
	void resizeEvent(QResizeEvent *event) override;
};

class GroupMemberList : public QListWidget
{
	Q_OBJECT

public:
	GroupMemberList(QWidget *parent = 0);
	~GroupMemberList();

	void setGroupInfo(const CRIMGrpInfo_Ex *info);
	void clearGroupInfo();
	QString getUserIDByNicknmae(const QString &nickName);
	void searchMember(const QString &str);
	const QSet<QString> &getManagers() const { return m_managerIds; }

signals:
	//changeType: 0：列表被清空，1：添加单个，2：删除单个,3：更新单个
	void s_memberChanged(int changeType, const QString &userId, const QString &newName, const QString &oldName = "");
	void s_actorChanged();

protected slots:
	void slot_grpMemberInfoChanged(const CRIMGrpMemberInfo &info);
	void slot_grpMemberAdded(const CRIMGrpMemberInfo &info);
	void slot_grpMemberDeleted(const CRIMGrpMemberInfo &info);
	void slot_itemDoubleClicked(QListWidgetItem *item);

protected:
	void addMember(const CRIMGrpMemberInfo &info);
	void removeMember(const CRIMGrpMemberInfo &info);
	void updateMember(const QString &userId);
	void updateHostWidget(GrpMemberListItem* pItem);
	void contextMenuEvent(QContextMenuEvent * event) override;

private:
	QString m_groupID;
	QHash<QString, GrpMemberListItem*>		m_itemsForUserId;
	QSet<QString>						m_managerIds;
};

#endif // GROUPMEMBERLIST_H
