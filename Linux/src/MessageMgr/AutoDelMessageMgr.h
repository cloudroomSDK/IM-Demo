#ifndef AutoDelMessageMgr_H
#define AutoDelMessageMgr_H

//自动删除消息管理类（用于阅后即焚功能）
class AutoDelMessageMgr : public QObject
{
	Q_OBJECT

public:
	static AutoDelMessageMgr* getInstance();
	struct DelInfo
	{
		QString conversationID;
		QString msgId;
		qint64 duration;	//在多少秒后删除，小于等于0立即删除
	};
	void burnMsg(const QList<DelInfo> &infos);
	void burnMsg(const DelInfo &info);
	void cancelByConversationId(const QString &conversationID);
	void cancelByMsgId(const QString &conversationID, const QString &msgId);
	QList<AutoDelMessageMgr::DelInfo> getInfosByConversation(const QString &conversationID);

signals:
	void s_msgsDurationProgress();

protected:
	AutoDelMessageMgr();
	~AutoDelMessageMgr();

protected slots:
	void slot_timeout();

private:
	QMap<QString, QList<DelInfo>> m_delInfoMap;
	QTimer m_delTimer;
};

#endif // AutoDelMessageMgr_H
