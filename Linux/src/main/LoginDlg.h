#ifndef LOGINDLG_H
#define LOGINDLG_H

namespace Ui {
 class LoginDlg;
}

class LoginDlg : public QNoNCDialog
{
    Q_OBJECT

public:
    LoginDlg(QWidget *parent = 0);
    ~LoginDlg();

	static LoginDlg* instance() { return s_pLogin; }

	void signOut();
	void logout();

protected:
	void closeEvent(QCloseEvent *e) override;


protected slots:
	void slot_btnLoginSettingClicked();
	void slot_btnBackClicked();
	void slot_loginSuccess();
	void slot_kickedOffline();

private:
    static LoginDlg* s_pLogin;
	Ui::LoginDlg *ui;
    QToolButton *m_pSettingBtn;
	QToolButton *m_pBackBtn;
};

#endif // LOGINDLG_H
