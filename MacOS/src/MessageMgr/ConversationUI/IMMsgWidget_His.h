#ifndef IMMSGWIDGET_HIS_H
#define IMMSGWIDGET_HIS_H

#include "IMMsgWidget.h"

class IMMsgWidget_His : public IMMsgWidget
{
	Q_OBJECT

public:
	IMMsgWidget_His(QWidget *parent = 0);
	~IMMsgWidget_His();

	void setConversationInfo(const CRIMConversationInfo &info){ m_conversationInfo = info; }
	//如果不传fromMsg，则加载最后几条数据
	void loadHistroyMsgs(const UIMsgInfo &fromMsg = UIMsgInfo(), const QString &highlighterText = "");
	
protected:
	bool isSessionMsg(const CRIMMessage &msg) override;
	enum LoadType
	{
		LOAD_FRONT,	//加载前面的数据
		LOAD_BACK,	//加载后面的数据
		LOAD_BOTH	//加载前面、后面的数据
	};
	//count=0：不加载，count>0：往后加载，count<0往前加载
	void loadData(const UIMsgInfo &msg, LoadType type, int count, bool bFisrtLoad = false);
	void appendMsg(const QList<CRIMMessage> &msgs, bool insertFirst);
	bool eventFilter(QObject *obj, QEvent *ev) Q_DECL_OVERRIDE;

protected slots:
	void slot_getHistoryMsgListSuccess(const QList<CRIMMessage> &msg, bool isEnd, const QString &cookie);
	void slot_getHistoryMsgListFailed(int code, const QString &error, const QString &cookie);
	void onLoadHistRslt();
	void slot_scrollToFromMsg();
private:
	QString m_fromMsgId;
	QString m_highlighterText;
	QByteArray m_cookie;
	QByteArray m_reverseCookie;
	int m_loadHisCount;
	UIMsgInfo m_firstMsg;
	UIMsgInfo m_lastMsg;
};

#endif // IMMsgWidget_His_H
