#include "stdafx.h"
#include "AddFriendDlg.h"
#include "ui_AddFriendDlg.h"
#include "UserInfoPage.h"


AddFriendDlg::AddFriendDlg(QWidget *parent)
	: QNoNCDialog(parent)
{
	StandardTitleBar *pTitleBar = new StandardTitleBar(this, StandardTitleBar::TBTN_CLOSE);
	this->setTitleBar(pTitleBar);

	QWidget *pClient = new QWidget(this);
	ui = new Ui::AddFriendDlg();
	ui->setupUi(pClient);
	this->setClientWidget(pClient);
	this->setAttribute(Qt::WA_DeleteOnClose);

	new QWidgetInstallKeyHandler(ui->lineEdit, Qt::Key_Enter, this, SLOT(on_searchBtn_clicked()));

	pClient->layout()->setContentsMargins(25, 25, 25, 25);
	ui->lineEdit->setLeftIconVisible(true);
	slot_updateSearchBtnState();

	connect(ui->lineEdit, &QLineEdit::textChanged, this, &AddFriendDlg::slot_updateSearchBtnState);
	connect(ui->searchBtn, &QToolButton::clicked, this, &AddFriendDlg::on_searchBtn_clicked);
	connect(ui->cancelBtn, &QToolButton::clicked, this, &AddFriendDlg::close);
	connect(DemoSvrWebAPI::instance(), &DemoSvrWebAPI::s_searchUserInfoRslt, this, &AddFriendDlg::slot_searchUserInfoRslt);
}

AddFriendDlg::~AddFriendDlg()
{
	delete ui;
}

void AddFriendDlg::slot_updateSearchBtnState()
{
	ui->lineEdit->setEnabled(_searching.isEmpty());

	bool bEnable = (ui->lineEdit->text().length() > 0 && _searching.isEmpty());
	ui->searchBtn->setEnabled(bEnable);
}


void AddFriendDlg::on_searchBtn_clicked()
{
	QString txt = ui->lineEdit->text();
	_searching = txt;
	slot_updateSearchBtnState();
	DemoSvrWebAPI::instance()->searchUserInfo(_searching);
}

void AddFriendDlg::slot_searchUserInfoRslt(const QString &errMsg, const DemoSvrWebAPI::UserInfo &uInfo)
{
	_searching.clear();
	slot_updateSearchBtnState();
	if(!errMsg.isEmpty())
	{
		CRMsgBox::information(this, tr("提示"), tr("查找失败!（%1)").arg(errMsg));
		return;
	}

	UserInfoPage *pDlg = new UserInfoPage(this, true);
	pDlg->setAttribute(Qt::WA_DeleteOnClose, true);
	pDlg->setModal(true);
	pDlg->show(uInfo);
}
