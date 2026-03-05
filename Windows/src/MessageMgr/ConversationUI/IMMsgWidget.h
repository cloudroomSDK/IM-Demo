#ifndef IMMSGWIDGET_H
#define IMMSGWIDGET_H

#include "ChatWidget.h"
class IMMsgWidget : public ChatWidget
{
	Q_OBJECT

public:
	IMMsgWidget(QWidget *parent = 0);
	~IMMsgWidget();
	enum WidgetType
	{
		MSGS_NULL,	
		MSGS_RECV,	//接收消息框
		MSGS_HIS,	//历史消息框
		MSGS_SEARCH,//搜索结果消息框
	};
	void setType(WidgetType type);

protected:
	void initShortcut();
	void downloadFile(const QString &msgId);
	//内容点击
	virtual void contentClickEvent(CursorPosType type, const QString &cursorText, const UIMsgInfo &msg);
	//菜单显示
	virtual void onMenuExec(QMenu *menu, ChatWidget::MenuType menuType, const UIMsgInfo &msgInfo){ ; }
	virtual bool isSessionMsg(const CRIMMessage &msg);
	QString getDownloadFileName(const UIMsgInfo &info);

protected slots:
	void slot_faceClicked(const UIMsgInfo &msg);
	void slot_contentClicked(CursorPosType type, const QString &cursorText, const UIMsgInfo &msg);
	void slot_rightClicked(ChatWidget::MenuType menuType, const UIMsgInfo &msg, const QVariantMap &exParams);
	void slot_downloadFileErr(const QString &url, const QString &errMsg, const QVariant &cookie);
	void slot_downloadFileProgress(const QString &url, qint64 pos, qint64 size, const QVariant &cookie);
	void slot_downloadFileFinished(const QString &url, const QString &locPathFileName, const QVariant &cookie);
	void slot_act_copyClicked();
	void slot_act_checkMultiClicked();
	void slot_act_delClicked();
	void slot_act_clearClicked();
	void slot_act_saveAsClicked();
	void slot_act_openDirClicked();

protected:
	CRIMConversationInfo m_conversationInfo;
	WidgetType m_type;
	QString	m_downloadCookie;
	QAction *m_act_copy;
};

#endif // IMMSGWIDGET_H
