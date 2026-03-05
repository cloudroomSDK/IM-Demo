#ifndef CONVERSATIONITEMWIDGET_H
#define CONVERSATIONITEMWIDGET_H

#include "ui_SessionItemWidget.h"

class ConversationBaseWidget;
class SessionListItem : public QListWidgetItem
{
public:
	SessionListItem(const CRIMConversationInfo &info, QListWidget *view);
	~SessionListItem();

public:
	const CRIMConversationInfo & getInfo() const { return m_info; }

	//进入会话
	void enterConversation(ConversationBaseWidget *widget);
	//离开会话
	void leaveConversation();

	//更新会话信息（内部可能更改信息）
	void updateInfo(const CRIMConversationInfo &info);
	//清空聊天记录
	void clearMsg();
	//清空会话未读计数
	void clearUnreadCount();
	//清空群聊会话at状态（只支持群聊）
	void clearGroupAtType();
	//最后一次草稿时间
	qint64 getDraftTime() const{ return m_lastDraftTime; }
	//分割器位置
	void setSplitterSize(const QList<int> &sz);
	QList<int> getSplitterSize();
	//获取待发送的数据
	QTextDocumentFragment getTextFragment();
	void setTextFragment(const QTextDocumentFragment &doc);

protected:
	qint64 getLastTime() const
	{
		qint64 maxTime = 4823755960000;//2122-11-10 12:12:40
		qint64 draftTime = getDraftTime();
		if (draftTime <= 0 || draftTime > maxTime)
		{
			return m_info.latestMsgSendTime;
		}
		return qMax(m_info.latestMsgSendTime, getDraftTime());
	}
	virtual bool operator<(const QListWidgetItem &other) const
	{
		const SessionListItem* pOther = static_cast<const SessionListItem*>(&other);
		if (pOther == NULL)
		{
			return false;
		}

		//是否置顶
		if (m_info.isPinned != pOther->m_info.isPinned)
		{
			return m_info.isPinned;
		}
	
		//消息时间
		qint64 myTime = getLastTime();
		qint64 otherTime = pOther->getLastTime();
		return myTime > otherTime;
	}

private:
	static QList<int> s_lastSplitterSizes;
	CRIMConversationInfo m_info;
	QListWidget *m_listView{ nullptr };
	ConversationBaseWidget *m_conversationUI{ nullptr };
	qint64 m_lastDraftTime{ 0 };
};


///////////////////////////////////////////////
class SessionItemWidget : public QWidget
{
	Q_OBJECT

public:
	SessionItemWidget(QWidget *parent = 0);
	~SessionItemWidget();
	
	void setInfo(const CRIMConversationInfo &info);
	void clear();

signals:
	void s_delConversation(const QString &id);

protected:
	QString getContentText(QString &rsltPrefix);
	virtual void mousePressEvent(QMouseEvent *event);
	virtual void mouseReleaseEvent(QMouseEvent *event);

protected slots:
	void slot_delTriggered();

private:
	Ui::SessionItemWidget ui;
	CRIMConversationInfo m_info;
	QMenu *m_menu{ nullptr };
};

#endif // ConversationItemWidget_H
