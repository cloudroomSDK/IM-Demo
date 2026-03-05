#ifndef CONVERSATION_GROUP_H
#define CONVERSATION_GROUP_H

#include "ui_Conversation_Group.h"
#include "ConversationBaseWidget.h"

class Conversation_Group : public ConversationBaseWidget
{
	Q_OBJECT

public:
	Conversation_Group(QWidget *parent = 0);
	~Conversation_Group();

	void init(const CRIMConversationInfo &info) override;
	void uninit() override;
	void updateInfo(const CRIMConversationInfo &info) override;
	void clearMsg() override;
	void setSplitterSize(const QList<int> &sz) override;
	QList<int> getSplitterSize() override;
	QString getDraftTxt() override;
	QTextDocumentFragment getTextFragment() override;
	void setTextFragment(const QTextDocumentFragment &doc) override;

protected:
	void updateUI();
	void fillGroupInfoData(const CRIMGrpInfo_Ex* gInfo);
	enum UIStatus
	{
		GS_NORMAL = 0,			/// 正常
		GS_BLOCKED,				/// 被封
		GS_DISMISSED,			/// 解散
		GS_MUTE,				/// 禁言
		GS_QUIT,				/// 已退出
	};
	void setUIStatus(UIStatus status);

protected slots:
	void slot_sendMsg(const QList<UIMsgInfo> &msgs);
	void slot_btnSettingClicked();
	void slot_btnAddMemberClicked();
	void slot_act_editGroupNameClicked();
	void slot_act_quitGroupClicked();
	void slot_act_dismissGruopClicked();
	void slot_joinedGrpAdded(const CRIMGrpInfo &info);
	void slot_grpInfoChanged(const CRIMGrpInfo &info);
	void slot_joinedGrpDeleted(const CRIMGrpInfo &info);
	void slot_joinedGrpDismissed(const CRIMGrpInfo &info);
	void slot_actorChanged();
	void slot_dismissGrpFailed(int code, const QString &error, const QString &cookie);
	void slot_findTextChanged(const QString &str);
	void slot_actAtUser_triggered();
	void slot_actTransOwner_triggered();
	void slot_actSetHost_triggered();
	void slot_actUnsetHost_triggered();
	void slot_actKick_triggered();

private:
	Ui::Conversation_Group ui;
	QByteArray m_cookie;
};

#endif // Conversation_Group_H
