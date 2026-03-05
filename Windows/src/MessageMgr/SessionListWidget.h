#ifndef CONVERSATIONLISTWIDGET_H
#define CONVERSATIONLISTWIDGET_H

class SessionListItem;
class ConversationBaseWidget;
class MessageMgrPage;
class SessionListWidget : public QListWidget
{
	Q_OBJECT

public:
	SessionListWidget(QWidget *parent = 0);
	~SessionListWidget();
	void setMessageMgrPage(MessageMgrPage *page){m_mgrPage = page;}

	QList<const CRIMConversationInfo*> getConversationList(bool bOnlyUnread);
	const CRIMConversationInfo* getConversationInfo(const QString &conversationID);
	const CRIMConversationInfo* getConversationInfoBySourceId(const QString &id, CRIMConversationType type);
	void switchConversation(const QString &id, CRIMConversationType type);
	void switchConversation(const QString &conversationId);
	void clearMsg(const QString &conversationId);
	void clearAllUnreadCount(const QStringList &conversationIds = QStringList());
	void jumpNextUnreadConversation();

signals:
	void s_setCurrentConversation(const CRIMConversationInfo &info = CRIMConversationInfo());

protected slots:
	void slot_widgetItemChanged(QWidget *pWidget, QListWidgetItem *item);
	void slot_syncServerFinish();
	void slot_getAllConversationListSuccess(const QList<CRIMConversationInfo> &, const QString &cookie);
	void slot_getAllConversationListFailed(int code, const QString &error, const QString &cookie);
	void slot_getOneConversationSuccess(const CRIMConversationInfo &, const QString &cookie);
	void slot_getOneConversationFailed(int code, const QString &error, const QString &cookie);
	void slot_newConversation(const QList<CRIMConversationInfo> &list);
	void slot_delOneConversation(const QString &conversationId);
	void slot_deleteConversationAndDeleteAllMsgSuccess(const QString &conversationID);
	void slot_conversationChanged(const QList<CRIMConversationInfo> &list);
	void slot_friendInfoChanged2(const CRIMFriendInfo &oldInfo, const CRIMFriendInfo &newInfo);
	void slot_itemClicked(QListWidgetItem *item);

protected:
	void addConversation(const CRIMConversationInfo &info);
	void updateConversation(const CRIMConversationInfo &info);
	void removeConversation(const QString &conversationID);
	void clearAll();
	void setCurrentConversation(const QString &conversationID);
	SessionListItem* getConversationItemBySourceId(const QString &id, CRIMConversationType type);
	void clearSelection();

private:
	QMap<QString, SessionListItem *>	m_itemsForId;
	QListWidgetItem*					m_currentItem;
	QMap<QString, QTextDocumentFragment> m_sendTextForId;
	MessageMgrPage *m_mgrPage;
};

#endif // ConversationListWidget_H
