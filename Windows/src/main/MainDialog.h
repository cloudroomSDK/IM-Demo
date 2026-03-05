#ifndef MAINDIALOG_H
#define MAINDIALOG_H

#include "ui_MainDialog.h"

class GlobalSearchRsltDlg;
class FriendsMgrPage;
class GroupsMgrPage;
class MeetingMainDialog;
class ImCallMgr;

class MainDialog : public QNoNCDialog
{
	Q_OBJECT

	MainDialog(QWidget *parent = 0);
public:
	~MainDialog();
	static MainDialog* instance();
	static bool hasInstance() { return (s_pMainDialog != nullptr); }

	void showOnLoginSuccess();

	FriendsMgrPage* getFriendsMgrPage();
	GroupsMgrPage* getGroupsMgrPage();

public slots:
	void slot_searchTextChanged(const QString &);
	void slot_updateSearchRsltDlgPos();

protected slots:
	void slot_getTotalUnreadMsgCountSuccess(int count, const QString &cookie);
	void slot_totalUnreadMsgCountChanged(int count);

	void slot_myselBtnClicked();
	void slot_messageBtnClicked();
	void slot_friendsBtnClicked();
	void slot_groupBtnClicked();
	void slot_moreBtnClicked();
	void slot_settingActionTrigered();
	void slot_aboutActionTrigered();
	void slot_signOutAccount();

	void slot_start_chat1v1_triggered();
	void slot_start_chatGrp_triggered();

	void slot_showNextMsgSession();

protected:
	void resizeEvent(QResizeEvent *e) override;

	void switch2Conversation(const QString &uId, CRIMConversationType type);
	void startFlashMainDlg();
	void stopFlashMainDlg();

private:
	static MainDialog* s_pMainDialog;
	Ui::MainDialog		ui;
	QClickedLabel*		m_lbMsgUnreadCount{ nullptr };
	QScopedPointer<GlobalSearchRsltDlg, QScopedPointerDeleteLater>	m_searchRsltGuard;
	ImCallMgr*			m_callMgr{ nullptr };
};

#endif // MAINDIALOG_H
