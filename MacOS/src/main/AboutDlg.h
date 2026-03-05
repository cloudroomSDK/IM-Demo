#ifndef ABOUTDLG_H
#define ABOUTDLG_H

namespace Ui {class AboutDlg;};

class AboutDlg : public QNoNCDialog
{
	Q_OBJECT

public:
	AboutDlg(QWidget *parent = 0);
	~AboutDlg();

private:
	Ui::AboutDlg *ui;
};

#endif // ABOUTDLG_H
