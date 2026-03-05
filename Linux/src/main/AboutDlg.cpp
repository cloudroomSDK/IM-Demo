#include "stdafx.h"
#include "AboutDlg.h"
#include "ui_AboutDlg.h"
#include "Version.h"


AboutDlg::AboutDlg(QWidget *parent)	: QNoNCDialog(parent)
{
	StandardTitleBar *pTitleBar = new StandardTitleBar(this, StandardTitleBar::TBTN_CLOSE);
	this->setTitleBar(pTitleBar);

	QWidget *pClient = new QFrame(this);
	ui = new Ui::AboutDlg();
	ui->setupUi(pClient);
	this->setClientWidget(pClient);
	
	QString demoVer = QString("Demo版本：V%1").arg(STR_VERSION);
	ui->productVer->setText(demoVer);
	QString sdkVer = QString("SDK版本：V%1").arg(CRIMSDK::getVersion());
	ui->sdkVer->setText(sdkVer);
}

AboutDlg::~AboutDlg()
{
	delete ui;
}
