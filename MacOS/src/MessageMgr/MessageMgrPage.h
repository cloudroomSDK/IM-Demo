#ifndef MESSAGEMGRPAGE_H
#define MESSAGEMGRPAGE_H

#include "ui_MessageMgrPage.h"
class ConversationBaseWidget;
class MessageMgrPage : public QWidget
{
	Q_OBJECT
	friend class SessionListWidget;

signals:
	void s_conversationListChanged();
	
public:
	MessageMgrPage(QWidget *parent = 0);
	~MessageMgrPage();

	static MessageMgrPage* instance(){ return m_instance; }
	QList<const CRIMConversationInfo*> getConversationList(bool bOnlyUnread = false);
	const CRIMConversationInfo* getConversationInfo(const QString &conversationID);
	const CRIMConversationInfo* getConversationByUserID(const QString &userID);
	const CRIMConversationInfo* getConversationByGroupID(const QString &groupID);

	//外部调用：选择当前会话进入，单聊：用户id，群聊：群组id
	void switchConversation(const QString &id, CRIMConversationType type);
	//外部调用：选择当前会话进入，conversationId：会话id
	void switchConversation(const QString &conversationId);
	//外部调用：清空会话未读计数，传空代表清除所有会话
	void clearAllUnreadCount(const QStringList &conversationIds = QStringList());
	//跳转到下一个未读会话
	void jumpNextUnreadConversation();

protected slots:
	void slot_setCurrentConversation(const CRIMConversationInfo &);
	void slot_clearConversationMsg();

protected:
	ConversationBaseWidget* getConversationWidget(CRIMConversationType type);

private:
	Ui::MessageMgrPage ui;
	static MessageMgrPage *m_instance;
	QMap<CRIMConversationType, ConversationBaseWidget*> m_conversations;
	ConversationBaseWidget* m_curConversation;
};

#endif // MessageMgrPage_H
