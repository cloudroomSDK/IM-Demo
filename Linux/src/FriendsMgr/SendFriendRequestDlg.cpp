#include "stdafx.h"
#include "SendFriendRequestDlg.h"
#include "ui_SendFriendRequestDlg.h"

#define MAX_REQ_MSG_LEN	 50

SendFriendRequestDlg::SendFriendRequestDlg(const DemoSvrWebAPI::UserInfo &info, QWidget *parent)
	: QNoNCDialog(parent)
{
	_info = info;
	StandardTitleBar *pTitleBar = new StandardTitleBar(this, StandardTitleBar::TBTN_CLOSE);
	this->setTitleBar(pTitleBar);

	QWidget *pClient = new QWidget(this);
	ui = new Ui::SendFriendRequestDlg();
	ui->setupUi(pClient);
	this->setClientWidget(pClient);
	new QWidgetInstallKeyHandler(ui->msgEdit, Qt::Key_Enter, this, SLOT(slot_addBtnClicked()));

	ui->name->setText(info.nickname);
	ui->id->setText(info.phoneNumber);
	ImIconMgr::instance()->bindIcon(ui->icon, IMIconInfo(info.userID, info.faceUrl, info.nickname));

	connect(ui->sendBtn, &QPushButton::clicked, this, &SendFriendRequestDlg::slot_sendBtnClicked);
	connect(ui->msgEdit, &QPlainTextEdit::textChanged, this, &SendFriendRequestDlg::slot_textChanged);
	connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_addFriendSuccess, this, &SendFriendRequestDlg::slot_addFriendSuccess);
	connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_addFriendFailed, this, &SendFriendRequestDlg::slot_addFriendFailed);

	slot_textChanged();
}

SendFriendRequestDlg::~SendFriendRequestDlg()
{
	delete ui;
}

void SendFriendRequestDlg::slot_textChanged()
{
	QString str = ui->msgEdit->toPlainText();
	if (str.length() > MAX_REQ_MSG_LEN)
	{
		str = str.left(MAX_REQ_MSG_LEN);
		ui->msgEdit->setPlainText(str);
		ui->msgEdit->moveCursor(QTextCursor::End);
		return;
	}

	ui->descCount->setText(tr("%1/%2").arg(str.length()).arg(MAX_REQ_MSG_LEN));
}

void SendFriendRequestDlg::slot_sendBtnClicked()
{
	QString strMsg = ui->msgEdit->toPlainText();
	CRIMManager_Qt::instance()->addFriend(_info.userID, strMsg, _info.userID.toUtf8());

	ui->msgEdit->setEnabled(false);
	ui->sendBtn->setEnabled(false);
}


void SendFriendRequestDlg::slot_addFriendSuccess(const QString &dat, const QString &cookie)
{
	if (cookie != _info.userID.toUtf8())
		return;

	this->close();
	CRMsgBox::information(this->parentWidget(), tr("提示"), tr("发送好友请求成功!"));
}

void SendFriendRequestDlg::slot_addFriendFailed(int code, const QString &err, const QString &cookie)
{
	if (cookie != _info.userID.toUtf8())
		return;

	ui->msgEdit->setEnabled(true);
	ui->sendBtn->setEnabled(true);

	QString msg = tr("发送好友请求失败!（%1)").arg(getErrDsc((CRIM_ERR_DEF)code));
	CRMsgBox::information(this, tr("提示"), msg);
}
