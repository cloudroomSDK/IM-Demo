#ifndef IMMSGWIDGET_RECV_H
#define IMMSGWIDGET_RECV_H

#include "ui_UnreadTipsWidget.h"
#include "IMMsgWidget.h"

class ConversationBaseWidget;
class IMMsgWidget_Recv : public IMMsgWidget
{
	Q_OBJECT

signals:
	void s_reEdit(const UIMsgInfo &msg);

public:
	IMMsgWidget_Recv(QWidget *parent = 0);
	~IMMsgWidget_Recv();
	
	void init(ConversationBaseWidget *w);
	void uninit();

	enum AppendMsgType
	{
		NEW_MSG,		//新消息
		HIS_MSG,		//历史消息
		LOC_MSG,		//本地消息
	};
	void appendMsg(const QList<UIMsgInfo> &msgs, AppendMsgType type = NEW_MSG);

protected:
	bool isSessionMsg(const CRIMMessage &msg, AppendMsgType type);
	void appendMsg(const QList<CRIMMessage> &msgs, AppendMsgType type = NEW_MSG);
	void appendMsg_inner(const QList<UIMsgInfo> &msgs, int orgMsgSize, AppendMsgType type);
	bool canRevoke(const UIMsgInfo &msg);

	void showEvent(QShowEvent *e) Q_DECL_OVERRIDE;
	void resizeEvent(QResizeEvent *e) Q_DECL_OVERRIDE;
	enum TIPS_TYPE
	{
		TIPS_UNREADMSG,
		TIPS_AT
	};
	void showUnreadTips(TIPS_TYPE type, int count); 
	virtual void contentClickEvent(CursorPosType type, const QString &cursorText, const UIMsgInfo &msg);
	virtual void onMenuExec(QMenu *menu, ChatWidget::MenuType menuType, const UIMsgInfo &msg);
	void loadHisData(int loadCount, const QVariantMap &ext = QVariantMap());
	bool isValidSession();
	void updateConnection(bool bConnect);
	void onMsgRevoked(const QString &msgId, const UIMsgInfo &revokeNotifyMsg = UIMsgInfo());
	void updateCurPageMsgsState();
	void slot_msgsDurationProgress();
	void removeMsg_inner(const QString &msgId);
	bool burnMsg(const UIMsgInfo &uiMsg);

protected slots:
	void slot_getHistoryMsgListSuccess(const QList<CRIMMessage> &msgs, bool isEnd, const QString &cookie);
	void slot_getHistoryMsgListFailed(int code, const QString &error, const QString &cookie);
	void slot_recvNewMsgs(const QList<CRIMMessage> &msgs);
	void slot_revokeMsgSuccess(const QString &conversationId, const QString &msgId, const QString &cookie);
	void slot_recvMsgRevoked(const CRIMRevokedInfo &info);
	void slot_msgDeleted(const CRIMMessage &msg);
	void slot_markMsgAsReadByMsgIDSuccess(const QString &conversationID, const QStringList &msgIDs, const QString &cookie);
	void slot_recv1v1ReadReceipt(const QList<CRIMReadReceiptInfo> &list);
	void slot_sendMsgSuccess(const CRIMMessage &msg, const QString &cookie);
	void slot_sendMsgFailed(const QString &msgID, int code, const QString &error, const QString &cookie);
	void slot_sendMsgProgress(const QString &msgID, int progress, const QString &cookie);
	void slot_viewScrollValueChanged(int value);
	void slot_loadMoreClicked();
	void slot_btnHisMsgClicked(); 
	void slot_resendClicked(const UIMsgInfo &msg);
	void slot_revokMessageClicked();
	void slot_btnUnreadAtClicked();
	void slot_btnUnreadMsgClicked();
	void slot_closeUnreadTips();
	void slot_revokeMsgTimeout();

private:
	ConversationBaseWidget* m_baseWidget{ nullptr };
	Ui::UnreadTipsWidget uiUnreadTips;
	QPushButton *m_previewMsgWidget{ nullptr };
	QWidget *m_unreadTipsWidget{ nullptr };
	CRIMMessage m_firstHisMsg;
	int m_curLoadHisCount{ 0 };
	QByteArray m_cookie;

	QString m_hisLastAtMsgId;
	QStringList m_unreadAtMsg;
	QString m_firstUnreadMsgId;
	
	struct RevokedMsgInfo_UI
	{
		UIMsgInfo srcMsgInfo;
		QSharedPointer<QTimer> revokeTimer;
	};
	QMap<QString, RevokedMsgInfo_UI> m_revokedMsgs;
	bool m_bLoading{ false };
};

#endif // IMMSGWIDGET_RECV_H
