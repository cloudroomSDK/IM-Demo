#ifndef LOGINSETTINGPAGE_H
#define LOGINSETTINGPAGE_H

#include <QFrame>
#include "ui_LoginSettingPage.h"

class LoginSettingPage : public QFrame
{
	Q_OBJECT

public:
	LoginSettingPage(QWidget *parent = 0);
	~LoginSettingPage();

	bool save();

signals:
	void s_cfgChanged();

protected:
	void slot_btnResetClicked();
	void slot_appServerChanged();

protected:
	void updateUI(const App::CfgInfo &cfg);
	void upSkipVerifyCertVisible();

private:
	Ui::LoginSettingPage ui;

};

#endif // LOGINSETTINGPAGE_H
