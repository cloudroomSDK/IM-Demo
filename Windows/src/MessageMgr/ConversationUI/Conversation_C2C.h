#ifndef CONVERSATION_C2C_H
#define CONVERSATION_C2C_H

#include "ui_Conversation_C2C.h"
#include "ConversationBaseWidget.h"
class Conversation_C2C : public ConversationBaseWidget
{
	Q_OBJECT

public:
	Conversation_C2C(QWidget *parent = 0);
	~Conversation_C2C();

	void init(const CRIMConversationInfo &info) override;
	void uninit() override;
	void updateInfo(const CRIMConversationInfo &info) override;
	void clearMsg() override;

	void setSplitterSize(const QList<int> &sz) override;
	QList<int> getSplitterSize() override;

	QString getDraftTxt() override;
	QTextDocumentFragment getTextFragment() override;
	void setTextFragment(const QTextDocumentFragment &doc) override;

protected slots:
	void slot_sendMsg(const QList<UIMsgInfo> &msgs);
	void slot_lbNameClicked();
	void slot_friendInfoChanged(const CRIMFriendInfo &info);
	void slot_createGrpClicked();
	void slot_btnSettingClicked();
	void slot_startImCall();

private:
	void updateFriendInfo();
	Ui::Conversation_C2C ui;

};

#endif // Conversation_C2C_H
