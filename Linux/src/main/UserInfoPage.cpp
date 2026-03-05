#include "stdafx.h"
#include "UserInfoPage.h"
#include "ImIconMgr.h"
#include "App.h"
#include "EditMyInfoDlg.h"
#include "SendFriendRequestDlg.h"

UserInfoPage::UserInfoPage(QWidget *parent, bool bDlgMode) : QNoNCDialog(parent)
{
	m_bDlgMode = bDlgMode;
	if (m_bDlgMode)
	{
		//添加标题栏
		StandardTitleBar *pTitleBar = new StandardTitleBar(this, StandardTitleBar::TBTN_CLOSE);
		this->setTitleBar(pTitleBar);
	}

	ui = new Ui::UserInfoPage();
	QWidget *pClient = new QWidget(this);
	ui->setupUi(pClient);
	this->setClientWidget(pClient);

	ui->loading->show();
	ui->clientWidget->hide();
	ui->bottomWidget->hide();

	connect(DemoSvrWebAPI::instance(), &DemoSvrWebAPI::s_queryUserInfoRslt, this, &UserInfoPage::slot_queryUserInfoRslt);
	connect(ui->editBtn, &QPushButton::clicked, this, &UserInfoPage::slot_editBtnClicked);
	connect(ui->sendMsgBtn, &QPushButton::clicked, this, &UserInfoPage::slot_sendMsgBtnClicked);
	connect(ui->addFriendBtn, &QPushButton::clicked, this, &UserInfoPage::slot_addFriendBtnClicked);
}

UserInfoPage::~UserInfoPage()
{

}


void UserInfoPage::slot_queryUserInfoRslt(const QString &errMsg, const DemoSvrWebAPI::UserInfo &uInfo)
{
	if (!errMsg.isEmpty())
	{
		ui->loading->setText(errMsg);
		return;
	}

	if (m_uInfo.userID != uInfo.userID)
		return;

	m_uInfo = uInfo;
	updateUserInfoToUI();
	ui->loading->hide();
	ui->clientWidget->show();
	ui->bottomWidget->show();
}

void UserInfoPage::updateUserInfoToUI()
{
	bool bMySelf = (m_uInfo.userID == CRIMManager_Qt::instance()->getUserID());
	bool bFriend = (CRIMManager_Qt::instance()->getFriendInfo(m_uInfo.userID) != nullptr);

	ImIconMgr::instance()->bindIcon(ui->icon, IMIconInfo(m_uInfo.userID, m_uInfo.faceUrl, m_uInfo.nickname));
	ui->name->setText(m_uInfo.nickname);
	ui->id->setText(m_uInfo.userID);
	ui->gender->setText(getGenderStr(m_uInfo.gender));

	if (bMySelf || bFriend)
	{
		QString birthDay = (m_uInfo.birth != 0) ? QDateTime::fromMSecsSinceEpoch(m_uInfo.birth).date().toString("yyyy/MM/dd") : QString();
		ui->birth->setText(birthDay);
		ui->phone->setText(m_uInfo.phoneNumber);
		ui->email->setText(m_uInfo.email);
	}

	ui->editBtn->setVisible(bMySelf);
	ui->sendMsgBtn->setVisible(bFriend);
	ui->addFriendBtn->setVisible(!bMySelf && !bFriend);
}

void UserInfoPage::updateDlgPos(const QPoint &pos)
{
	if (!pos.isNull())
	{
		QSize sz = this->size();
		QPoint dstPos(pos);
		QRect rt = QApplication::desktop()->availableGeometry(this);
		if ((dstPos.x() + sz.width()) >= rt.right())
		{
			dstPos.setX(pos.x() - sz.width());
		}
		if ((dstPos.y() + sz.height()) >= rt.bottom())
		{
			dstPos.setY(pos.y() - sz.height());
		}

		this->move(dstPos);
	}
}

void UserInfoPage::show(const QString &uid, const QString &grpID, const QPoint &pos)
{
	DemoSvrWebAPI::instance()->queryUserInfo(uid);
	ui->loading->show();
	ui->clientWidget->hide();
	ui->bottomWidget->hide();

	m_uInfo = DemoSvrWebAPI::UserInfo();
	m_uInfo.userID = uid;

	bool bMySelf = (uid == CRIMManager_Qt::instance()->getUserID());
	const CRIMFriendInfo* pFrdInfo = CRIMManager_Qt::instance()->getFriendInfo(uid);
	if (bMySelf) //自已
	{
		const CRIMUserInfo &info = CRIMManager_Qt::instance()->getSelfUserInfo();
		m_uInfo.userID = info.userID;
		m_uInfo.faceUrl = info.faceURL;
		m_uInfo.nickname = info.nickname;
	}
	else if (pFrdInfo != nullptr) //好友
	{
		m_uInfo.userID = pFrdInfo->userID;
		m_uInfo.faceUrl = pFrdInfo->faceURL;
		m_uInfo.nickname = pFrdInfo->nickname;
	}
	else if (grpID.length()>0) //群成员
	{
		const CRIMGrpMemberInfo *pMember = CRIMManager_Qt::instance()->getGrpMemberInfo(grpID, uid);
		if (pMember != nullptr)
		{
			m_uInfo.userID = pMember->userID;
			m_uInfo.faceUrl = pMember->faceURL;
			m_uInfo.nickname = pMember->nickname;
		}
	}

	updateUserInfoToUI();
	QNoNCDialog::show();
	updateDlgPos(pos);
}

void UserInfoPage::show(const DemoSvrWebAPI::UserInfo &info, const QPoint &pos)
{
	ui->loading->hide();
	ui->clientWidget->show();
	ui->bottomWidget->show();

	m_uInfo = info;
	updateUserInfoToUI();
	QNoNCDialog::show();
	updateDlgPos(pos);
}


bool UserInfoPage::event(QEvent *e)
{
	if (!m_bDlgMode && e->type() == QEvent::WindowDeactivate)
	{
		QWidget *activeWind = QApplication::activeWindow();
		if (activeWind == NULL || activeWind->parent() != this)
		{
			close();
			return true;
		}
	}
	return QNoNCDialog::event(e);
}

void UserInfoPage::slot_editBtnClicked()
{
	QWidget *pParent = this->parentWidget();
	QRect rt = this->geometry();
	this->close();

	bool bMySelf = (m_uInfo.userID == CRIMManager_Qt::instance()->getUserID());
	if (!bMySelf)
		return;

	EditMyInfoDlg *pDlg = new EditMyInfoDlg(m_uInfo, pParent);
	pDlg->setAttribute(Qt::WA_DeleteOnClose);
	pDlg->setModal(true);
	pDlg->setGeometry(rt);
	pDlg->show();
}

void UserInfoPage::slot_sendMsgBtnClicked()
{
	this->close();
	g_atMgr.getActionById(ACT_START_Chat1v1, m_uInfo.userID)->trigger();
}

void UserInfoPage::slot_addFriendBtnClicked()
{
	QWidget *pParent = this->parentWidget();
	this->close();

	SendFriendRequestDlg *pDlg = new SendFriendRequestDlg(m_uInfo, pParent);
	pDlg->setAttribute(Qt::WA_DeleteOnClose);
	pDlg->setModal(true);
	pDlg->show();
}