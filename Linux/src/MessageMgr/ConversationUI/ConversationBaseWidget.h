#ifndef CONVERSATIONBASEWIDGET_H
#define CONVERSATIONBASEWIDGET_H

class ConversationBaseWidget : public QWidget
{
	Q_OBJECT

public:
	//初始化
	virtual void init(const CRIMConversationInfo &info);
	//反初始化
	virtual void uninit();
	//更新信息
	virtual void updateInfo(const CRIMConversationInfo &info);
	//清空聊天记录
	virtual void clearMsg() {}

	CRIMConversationType getType(){ return m_conversationInfo.conversationType; }
	const CRIMConversationInfo & getInfo() const { return m_conversationInfo; }

	//获取id，单聊返回用户id，群聊返回群聊id
	const QString& getID() const;
	//获取会话id
	const QString& getConversationID() const;
	//获取当前草稿信息
	virtual QString getDraftTxt() = 0;

	//更新分割器位置
 	virtual void setSplitterSize(const QList<int> &sz) = 0;
 	virtual QList<int> getSplitterSize() = 0;

	//获取待发送的数据
	virtual QTextDocumentFragment getTextFragment() = 0;
	virtual void setTextFragment(const QTextDocumentFragment &doc) = 0;

protected:
	ConversationBaseWidget(QWidget *parent = 0);
	virtual ~ConversationBaseWidget();

protected:
	CRIMConversationInfo m_conversationInfo;
};

#endif // ConversationBaseWidget_H
