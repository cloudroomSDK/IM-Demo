#include "stdafx.h"
#include "GroupDetailWidget.h"
#include "MessageMgrPage.h"
#include "GroupMemberList.h"
#include "DlgAddGroupMember.h"
#include "UserInfoPage.h"

#define MemberItem_W	65
#define MemberItem_H	75

GroupDetailWidget::GroupDetailWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	ui.groupBaseLayout->setContentsMargins(12, 12, 9, 12);
	ui.groupBaseLayout->setHorizontalSpacing(30);
	ui.groupBaseLayout->setHorizontalSpacing(30);
	ui.lbGroupFace->setFixedSize(QSize(60,60));

	ui.memberList->setIconSize(DEFUALT_ICONSZ);
	ui.memberList->setGridSize(QSize(MemberItem_W, MemberItem_H));
	ui.memberList->installEventFilter(this);
	ui.memberList->setSortingEnabled(true);

	connect(ui.memberList, &QListWidget::itemClicked, this, &GroupDetailWidget::slot_MemberListItemClicked);

	connect(ui.btnGroupName, &QToolButton::clicked, ui.editGroupName, &CRTxtEditor::startEdit);
	connect(ui.editGroupName, &CRTxtEditor::s_commit, this, &GroupDetailWidget::slot_commitGroupName);
	connect(ui.btnMyName, &QToolButton::clicked, ui.editMyName, &CRTxtEditor::startEdit);
	connect(ui.editMyName, &CRTxtEditor::s_commit, this, &GroupDetailWidget::slot_commitMyName);

	connect(ui.btnPin, &QCheckBox::stateChanged, this, &GroupDetailWidget::slot_btnPinStateChanged);
	connect(ui.btnDnd, &QCheckBox::stateChanged, this, &GroupDetailWidget::slot_btnDndStateChanged);
	connect(ui.btnForbid, &QCheckBox::stateChanged, this, &GroupDetailWidget::slot_btnForbidStateChanged);

	connect(ui.memberListCtrlBtn, &QPushButton::toggled, this, &GroupDetailWidget::slot_MemberListCtrlBtnToggled);
	connect(ui.btnGroupChat, &QPushButton::clicked, this, &GroupDetailWidget::slot_btnGroupChatClicked);
	connect(ui.btnClearGroup, &QPushButton::clicked, this, &GroupDetailWidget::slot_btnClearGroupClicked);
	connect(ui.btnDismissGroup, &QPushButton::clicked, this, &GroupDetailWidget::slot_btnDismissGroupClicked);
	connect(ui.btnQuitGroup, &QPushButton::clicked, this, &GroupDetailWidget::slot_btnQuitGroupClicked);

	connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_grpInfoChanged, this, &GroupDetailWidget::slot_grpInfoChanged);
	connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_grpMemberInfoChanged, this, &GroupDetailWidget::slot_grpMemberInfoChanged);
	connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_grpMemberAdded, this, &GroupDetailWidget::slot_grpMemberChanged);
	connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_grpMemberDeleted, this, &GroupDetailWidget::slot_grpMemberChanged);

}

GroupDetailWidget::~GroupDetailWidget()
{

}

void GroupDetailWidget::setGroupInfo(const QString &grpID)
{
	const CRIMGrpInfo_Ex *info = CRIMManager_Qt::instance()->getJoinedGrpInfo(grpID);
	if (info == NULL)
	{
		return;
	}

	m_grpID = grpID;
	//群基本信息
	ImIconMgr::instance()->bindIcon(ui.lbGroupFace, IMIconInfo(info->groupID, IMICON_GRP, ui.lbGroupFace->size()));
	ui.lbGroupName->setText(tr("群名称：%1").arg(info->groupName));
	ui.lbGroupID->setText(tr("群ID：%1").arg(info->groupID));

	QString myUserID = CRIMManager_Qt::instance()->getUserID();
	QString myName = CRIMManager_Qt::instance()->getSelfUserInfo().nickname;

	//群成员信息
	ui.memberList->clear();
	for (auto it = info->_members.begin(); it != info->_members.end(); it++)
	{
		const CRIMGrpMemberInfo &memberInfo = it.value();
		GrpMemberListItem *pItem = new GrpMemberListItem;
		pItem->updateMemberInfo(memberInfo);
		pItem->setSizeHint(QSize(MemberItem_W, MemberItem_H));
		ImIconMgr::instance()->bindIcon(pItem, IMIconInfo(memberInfo.userID));
		ui.memberList->addItem(pItem);
		if (memberInfo.userID == myUserID)
		{
			myName = memberInfo.nickname;
		}
	}

	//添加一个add item
	{
		CRIMGrpMemberInfo tmpAddInfo;
		tmpAddInfo.groupID = m_grpID;
		tmpAddInfo.userID = g_addGroupMemberStr;
		tmpAddInfo.nickname = tr("添加");

		GrpMemberListItem *pItem = new GrpMemberListItem;
		pItem->updateMemberInfo(tmpAddInfo);
		pItem->setSizeHint(QSize(MemberItem_W, MemberItem_H));
		pItem->setIcon(QIcon(svgRes2Pixmap(":/Res/titleNewBtn.svg", DEFUALT_ICONSZ)));
		ui.memberList->addItem(pItem);
	}
	slot_MemberListCtrlBtnToggled();

	//群信息
	ui.editGroupName->setText(info->groupName);
	ui.editGroupID->setText(info->groupID);
	ui.editMyName->setText(myName);
	const CRIMConversationInfo* conversationInfo = MessageMgrPage::instance()->getConversationByGroupID(info->groupID);
	ui.btnPin->blockSignals(true);
	ui.btnDnd->blockSignals(true);
	ui.btnForbid->blockSignals(true);
	ui.btnPin->setChecked(conversationInfo != NULL && conversationInfo->isPinned);
	ui.btnDnd->setChecked(conversationInfo != NULL && conversationInfo->recvMsgOpt == CRIM_RecvMsgOpt_NotNotify);
	ui.btnForbid->setChecked(info->status == CRIM_GroupStatus_Muted);
	ui.btnPin->blockSignals(false);
	ui.btnDnd->blockSignals(false);
	ui.btnForbid->blockSignals(false);

	//权限处理
	bool bOwner = (info->ownerUserID == CRIMManager_Qt::instance()->getUserID());
	ui.btnGroupName->setVisible(bOwner);
	ui.forbidRowWidget->setVisible(bOwner);
	ui.btnDismissGroup->setVisible(bOwner);
	ui.btnQuitGroup->setVisible(!bOwner);

}

void GroupDetailWidget::slot_grpInfoChanged(const CRIMGrpInfo &info)
{
	if (info.groupID != m_grpID)
		return;

	setGroupInfo(m_grpID);
}

GrpMemberListItem* GroupDetailWidget::getMemberItem(const QString &userID)
{
	for (int i = 0; i < ui.memberList->count(); i++)
	{
		QListWidgetItem *pItem = ui.memberList->item(i);
		GrpMemberListItem *pMemberItem = static_cast<GrpMemberListItem*>(pItem);
		if ( pMemberItem==nullptr )
			continue;
		if (pMemberItem->m_userID == userID)
			return pMemberItem;
	}
	return nullptr;
}


void GroupDetailWidget::slot_grpMemberInfoChanged(const CRIMGrpMemberInfo &info)
{
	GrpMemberListItem* pItem = getMemberItem(info.userID);
	if (pItem == nullptr)
		return;

	pItem->updateMemberInfo(info);
}

void GroupDetailWidget::slot_grpMemberChanged()
{
	this->setGroupInfo(m_grpID);
}

void GroupDetailWidget::slot_MemberListItemClicked(QListWidgetItem *item)
{
	GrpMemberListItem *pMemberItem = static_cast<GrpMemberListItem*>(item);
	if (pMemberItem == nullptr)
		return;

	if (pMemberItem->m_userID == g_addGroupMemberStr)
	{
		//添加群成员
		DlgAddGroupMember *dlgAdd = new DlgAddGroupMember(m_grpID, this);
		dlgAdd->setAttribute(Qt::WA_DeleteOnClose);
		dlgAdd->exec();
		return;
	}
	else
	{
		UserInfoPage *pDlg = new UserInfoPage(this);
		pDlg->setAttribute(Qt::WA_DeleteOnClose);
		pDlg->show(pMemberItem->m_userID, m_grpID, QCursor::pos());
	}
}

void GroupDetailWidget::slot_MemberListCtrlBtnToggled()
{
	bool checked = ui.memberListCtrlBtn->isChecked();
	int nCol = qMax(1, (ui.memberList->width() - 4) / MemberItem_W);
	int nRow = (ui.memberList->count() + nCol - 1) / nCol;
	ui.memberListCtrlBtn->setVisible(nRow > 2);

	if (checked) //显示所有
	{
		ui.memberListCtrlBtn->setText(tr("收起 ↑"));
		int listHi = nRow * MemberItem_H + 4;
		ui.memberList->setFixedHeight(listHi);
	}
	else
	{
		ui.memberListCtrlBtn->setText(tr("更多成员 ↓"));

		int listHi = qMin(2, nRow) * MemberItem_H + 4;
		ui.memberList->setFixedHeight(listHi);
	}

	//第二行最尾，至倒数第二个，显示/隐藏
	for (int i = 0; i < (ui.memberList->count() - 1); i++)
	{
		QListWidgetItem *pItem = ui.memberList->item(i);
		if (pItem == nullptr)
			continue;
		bool bHide = (!checked) && (i>=(2 * nCol - 1));
		pItem->setHidden(bHide);
	}
}

void GroupDetailWidget::slot_commitGroupName(const QString &oldStr, const QString &newStr)
{
	const CRIMGrpInfo_Ex *grpInfo = CRIMManager_Qt::instance()->getJoinedGrpInfo(m_grpID);
	if (grpInfo == nullptr)
		return;

	if (newStr.isEmpty())
	{
		ui.editGroupName->setText(oldStr);
		CRMsgBox::warning(this, tr("提示"), tr("群聊名称不能为空！"), CRMsgBox::Ok);
		return;
	}

	CRIMGrpInfo info = *((CRIMGrpInfo*)grpInfo);
	info.groupName = newStr;
	CRIMManager_Qt::instance()->setGrpInfo(info);
}

void GroupDetailWidget::slot_commitMyName(const QString &oldStr, const QString &newStr)
{
	QString myUserID = CRIMManager_Qt::instance()->getUserID();
	if (newStr.isEmpty())
	{
		ui.editGroupName->setText(oldStr);
		CRMsgBox::warning(this, tr("提示"), tr("昵称不能为空！"), CRMsgBox::Ok);
		return;
	}
	if (oldStr == newStr)
	{
		return;
	}

	CRIMManager_Qt::instance()->setGrpMemberNickname(m_grpID, myUserID, newStr);
}

void GroupDetailWidget::slot_btnPinStateChanged(int state)
{
	QString converSationID = CRIMManager_Qt::instance()->getConversationIDBySessionType(m_grpID, CRIM_Conversation_Group);
	CRIMManager_Qt::instance()->pinConversation(converSationID, state != 0);
}


void GroupDetailWidget::slot_btnDndStateChanged(int state)
{
	QString converSationID = CRIMManager_Qt::instance()->getConversationIDBySessionType(m_grpID, CRIM_Conversation_Group);
	CRIMReceiveMsgOpt opt = (state == 0) ? CRIM_RecvMsgOpt_Receive : CRIM_RecvMsgOpt_NotNotify;
	CRIMManager_Qt::instance()->setConversationRecvMsgOpt(converSationID, opt);
}

void GroupDetailWidget::slot_btnForbidStateChanged(int state)
{
	CRIMManager_Qt::instance()->changeGrpMute(m_grpID, (state != 0));
}


bool GroupDetailWidget::eventFilter(QObject *watched, QEvent *event)
{
	if (watched == ui.memberList)
	{
		if (event->type() == QEvent::Resize)
		{
			slot_MemberListCtrlBtnToggled();
		}
	}

	return QWidget::eventFilter(watched, event);
}

void GroupDetailWidget::slot_btnGroupChatClicked()
{
	g_atMgr.getActionById(ACT_START_ChatGrp, m_grpID)->trigger();
}

void GroupDetailWidget::slot_btnClearGroupClicked()
{
	
	if (CRMsgBox::warning(this, tr("清空聊天记录"), tr("聊天记录清空后将无法恢复，您确认要清空吗？"), CRMsgBox::Ok | CRMsgBox::Cancel) != CRMsgBox::Ok)
		return;

	QString converSationID = CRIMManager_Qt::instance()->getConversationIDBySessionType(m_grpID, CRIMSDK::CRIM_Conversation_Group);
	g_atMgr.getActionById(ACT_ClearConverstationMsg, converSationID)->trigger();
	emit s_btnClearGroupClicked();
}

void GroupDetailWidget::slot_btnDismissGroupClicked()
{
	if (CRMsgBox::warning(this, tr("解散群聊"), tr("您确认要解散此群吗？"), CRMsgBox::Ok | CRMsgBox::Cancel) != CRMsgBox::Ok)
		return;
	CRIMManager_Qt::instance()->dismissGrp(m_grpID);
	emit s_btnDismissGroupClicked();
}

void GroupDetailWidget::slot_btnQuitGroupClicked()
{
	if (CRMsgBox::warning(this, tr("退出群聊"), tr("您确认要退出此群吗？"), CRMsgBox::Ok | CRMsgBox::Cancel) != CRMsgBox::Ok)
		return;
	CRIMManager_Qt::instance()->quitGrp(m_grpID);
	emit s_btnQuitGroupClicked();
}
