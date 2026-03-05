#include "stdafx.h"
#include "FriendItemWidget.h"
#include "CRIM_Manager.h"

void FriendListItem::setItemDat(const CRIMFriendInfo *info)
{
	m_userID = info->userID;
	if (m_userID == g_strAddFriend || m_userID == g_strNewFriendMsg)
	{
		m_gbkName = m_userID.toUtf8().constData();
	}
	else
	{
		m_gbkName = QTextCodec::codecForName("gbk")->fromUnicode(info->getDisplayName().toLower());
	}
}


///////////////////////////////////////////////
FriendItemWidget::FriendItemWidget(FriendListItem *item, QWidget *parent)
	: QFrame(parent)
{
	ui.setupUi(this);
	m_item = item;
	ui.lbFriendFace->setFixedSize(DEFUALT_ICONSZ);
}

FriendItemWidget::~FriendItemWidget()
{

}


void FriendItemWidget::updateUI()
{
	QString strUserID = m_item->getUserID();
	if (strUserID == g_strAddFriend)
	{
		ui.lbFriendFace->setPixmap(svgRes2Pixmap(":/Res/friend_add.svg", ui.lbFriendFace->size()));
		ui.lbName->setText(tr("添加好友"));
		return;
	}
	
	if (strUserID == g_strNewFriendMsg)
	{
		ui.lbFriendFace->setPixmap(svgRes2Pixmap(":/Res/friend_new.svg", ui.lbFriendFace->size()));
		ui.lbName->setText(tr("新的好友"));

		if (m_newDatRedTip == nullptr)
		{
			m_newDatRedTip = addSubWidgetFor(ui.lbFriendFace, "NewDatRedTip", Qt::AlignRight | Qt::AlignTop);
			m_newDatRedTip->hide();
		}
		return;
	}

	const CRIMFriendInfo *pInfo = CRIMManager_Qt::instance()->getFriendInfo(strUserID);
	if (nullptr == pInfo)
	{
		return;
	}
	ImIconMgr::instance()->bindIcon(ui.lbFriendFace, IMIconInfo(pInfo->userID, IMICON_USR, ui.lbFriendFace->size()));
	ui.lbName->setText(pInfo->getDisplayName());
}

