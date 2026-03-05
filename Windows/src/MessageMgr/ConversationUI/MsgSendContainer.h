#ifndef MSGSENDCONTAINER_H
#define MSGSENDCONTAINER_H

#include "ui_MsgSendContainer.h"
class ConversationBaseWidget;
class GroupMemberList;
class DlgFastSelMember;
class MsgSendContainer : public QWidget
{
	Q_OBJECT

public:
	MsgSendContainer(QWidget *parent = 0);
	~MsgSendContainer();
	Ui::MsgSendContainer ui;
	
	void init(ConversationBaseWidget *w, GroupMemberList *memberList);
	void uninit();

	QList<UIMsgInfo> getCurrentMsg();
	void insertText(const QString &txt);
	void insertHtmlText(const QString &htmlTxt);
	void insertAtText(const QString &atUserId);
	
	QTextDocumentFragment getTextFragment();
	void setTextFragment(const QTextDocumentFragment &doc);
	void clear();

signals:
	void s_sendMsg(const QList<UIMsgInfo> &msgs);

protected:
	UIMsgInfo createTextMsgStruct(const QString &text, QMap<QString, QString> &atUserId);
	QList<UIMsgInfo> parseMsgFromSendedit();
	bool eventFilter(QObject *watched, QEvent *event);
	void insertAtMsg(const UIMsgInfo &msg);
	bool checkMessageExceedingLimitSz(const QList<UIMsgInfo> &s);

public slots:
	void slot_reEdit(const UIMsgInfo &content);

protected slots:
	void slot_btnFaceClicked(bool bChecked);
	void slot_faceSel(const QString& strName);
	void slot_btnOpenPicClicked();
	void slot_btnOpenFileClicked();
	void slot_sendEditTextChanged();
	void slot_fastSelFinished(const QString &str);
	void slot_memberChanged(int changeType, const QString &userId, const QString &newName, const QString &oldName);
	void slot_btnSendMsgClicked();

private:
	ConversationBaseWidget* m_baseWidget;
	GroupMemberList *m_memberList;
	QAction *m_actEnter;
	DlgFastSelMember *m_dlgFastSelMember;
	QTextCursor m_curAtCursor;
};

#endif // MsgSendContainer_H
