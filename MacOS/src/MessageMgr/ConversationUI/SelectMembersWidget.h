#ifndef SelectMembersWidget_h
#define SelectMembersWidget_h

#include "ui_SelectMembersWidget.h"
#include "ui_SelectMemberRsltWidget.h"


class SelRsltWidget_SMW : public QWidget
{
	Q_OBJECT

signals:
	void s_delUsr(const QString &userId);
public:
	SelRsltWidget_SMW(const QString &id, QWidget *parent) : QWidget(parent)
	{
		ui.setupUi(this);
		ui.horizontalLayout->setSpacing(18);
		connect(ui.btnDel, &QToolButton::clicked, this, [=](bool checked){
			emit s_delUsr(id);
		});
	}

	Ui::SelectMemberRsltWidget ui;
};

class SelectMembersWidget : public QFrame
{
	Q_OBJECT

public:
	SelectMembersWidget(QWidget *parent = 0);
	~SelectMembersWidget();
	struct SelectCfg
	{
		QString			groupID;		//为空时：数据源为好友，否则为群成员

		QSet<QString>	disableIds;		//禁止选择成员
		QSet<QString>	selectedIds;	//已选择成员（禁止移除）
		int				maxSelect;		//最大选择数量
		SelectCfg()
		{
			maxSelect = 100;
		}
	};
	void init(const SelectCfg &cfg = SelectCfg());
	QList<QString> getSelectedIds(){ return m_selItems.keys(); }

protected slots:
	void slot_itemClicked(QListWidgetItem *item);
	void slot_itemChanged(QListWidgetItem *item);
	void slot_checkAllToggled(bool checked);
	void slot_delSelUser(const QString &users);

protected:
	void fillFriendDat(const QStringList &containsIds = QStringList());
	void fillGroupMemberDat(const QString &gId, const QStringList &containsIds = QStringList());
	void addUserToList(const CRIMFriendInfo *info);
	void addGroupUserToList(const CRIMGrpMemberInfo *info);

	void addUserToSelList(const QSet<QString> &users);
	void removeUserFromSelList(const QSet<QString> &users);
	bool isSelected(const QString &userId);
	bool isSelected(const QSet<QString> &userIds);
	void updateConnnection(bool bDisconnect);
	void updateCheckStates();

private:
	Ui::SelectMembersWidget ui;
	SelectCfg m_cfg;
	QHash<QString, QListWidgetItem*> m_selItems;
};

#endif // SelectMembersWidget_h
