#include "stdafx.h"
#include "CRDialogBtns.h"
#include "ui_CRDialogBtns.h"

CRDialogBtns::CRDialogBtns(QWidget *parent) : TitleBar(parent)
{
	ui = new Ui::CRDialogBtns;
	ui->setupUi(this);
#ifdef MAC
	ui->btnsWidget->hide();
#endif
}

CRDialogBtns::~CRDialogBtns()
{
	delete ui;
}


void CRDialogBtns::showEvent(QShowEvent *event)
{
	//之所以不在构造函数进行绑定，是因为ui生成的代码，类似QStackedWidget中的子页面在构造时，父窗口为空， 导至无法获取Window
	if (_pDlg == nullptr)
	{
		QNoNCDialog *pDlg = dynamic_cast<QNoNCDialog*>(this->window());
		this->bindDialog(pDlg);
		this->setSysBtnStyle(TitleBar::TBTN_ALL);

		ui->titleMinBtn->setFocusPolicy(Qt::NoFocus);
		ui->titleRestoreBtn->setFocusPolicy(Qt::NoFocus);
		ui->titleMaxBtn->setFocusPolicy(Qt::NoFocus);
		ui->titleCloseBtn->setFocusPolicy(Qt::NoFocus);
		this->bindTitleBarBtns(ui->titleCloseBtn, ui->titleMinBtn, ui->titleRestoreBtn, ui->titleMaxBtn);
	}
	QWidget::showEvent(event);
}
