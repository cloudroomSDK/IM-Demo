#include "stdafx.h"
#include "Conversation_Group.h"
#include "CRIM_Manager.h"
#include "DlgAddGroupMember.h"
#include "QFindEdit.h"

Conversation_Group::Conversation_Group(QWidget *parent)
	: ConversationBaseWidget(parent)
{
	ui.setupUi(this);
	m_cookie = makeUUID().toUtf8();
	ui.chatTitleWidget->layout()->setContentsMargins(12, 0, 0, 0);
	ui.chatTitleWidget->layout()->setSpacing(12);
	ui.toolBarWidget->layout()->setContentsMargins(0, 0, 4, 0);
	ui.pageMemb->layout()->setContentsMargins(10, 24, 10, 0);
	ui.pageMemb->layout()->setSpacing(10);
	
	ui.lbFace->setFixedSize(DEFUALT_ICONSZ);
	ui.findEdit->setClearBtnVisibleType(CRLineEdit::CLRV_AUTO);
	ui.findEdit->setLeftIconVisible(true);
	ui.findEdit->setPlaceholderText(tr("搜索"));
	ui.splitter->setStretchFactor(0, 10);
	ui.splitter->setStretchFactor(1, 1);
	ui.stackedWidget->setCurrentWidget(ui.pageMemb);

	connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_createGrpSuccess, this, &Conversation_Group::slot_joinedGrpAdded);
	connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_grpInfoChanged, this, &Conversation_Group::slot_grpInfoChanged);
	connect(ui.memberList, &GroupMemberList::s_actorChanged, this, &Conversation_Group::slot_actorChanged);
	connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_joinedGrpAdded, this, &Conversation_Group::slot_joinedGrpAdded);
	connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_joinedGrpDeleted, this, &Conversation_Group::slot_joinedGrpDeleted);
	connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_joinedGrpDismissed, this, &Conversation_Group::slot_joinedGrpDismissed);
	connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_dismissGrpFailed, this, &Conversation_Group::slot_dismissGrpFailed);
	//connect(CRIMManager_Qt::instance(), &CRIMManager::s_dismissGrpSuccess, this, &Conversation_Group::slot_dismissGrpSuccess);
	connect(ui.findEdit, &QFindEdit::textChanged, this, &Conversation_Group::slot_findTextChanged);
	connect(ui.recvContainer, &IMMsgWidget_Recv::s_reEdit, ui.sendContainer, &MsgSendContainer::slot_reEdit);
	connect(ui.sendContainer, &MsgSendContainer::s_sendMsg, this, &Conversation_Group::slot_sendMsg);
	connect(ui.btnSetting, &QPushButton::clicked, this, &Conversation_Group::slot_btnSettingClicked);
	connect(ui.btnAddMember, &QPushButton::clicked, this, &Conversation_Group::slot_btnAddMemberClicked);
	connect(g_atMgr.getActionById(GMEM_ACT_AT_USER, "", ""), &QAction::triggered, this, &Conversation_Group::slot_actAtUser_triggered);
	connect(g_atMgr.getActionById(GMEM_ACT_TRANS_OWNER, "", ""), &QAction::triggered, this, &Conversation_Group::slot_actTransOwner_triggered);
	connect(g_atMgr.getActionById(GMEM_ACT_SET_HOST, "", ""), &QAction::triggered, this, &Conversation_Group::slot_actSetHost_triggered);
	connect(g_atMgr.getActionById(GMEM_ACT_UNSET_HOST, "", ""), &QAction::triggered, this, &Conversation_Group::slot_actUnsetHost_triggered);
	connect(g_atMgr.getActionById(GMEM_ACT_KICK, "", ""), &QAction::triggered, this, &Conversation_Group::slot_actKick_triggered);
}

Conversation_Group::~Conversation_Group()
{

}

void Conversation_Group::init(const CRIMConversationInfo &info)
{
	ConversationBaseWidget::init(info);

	ui.sendContainer->init(this, ui.memberList);
	ui.lbName->setText(info.showName());
	ImIconMgr::instance()->bindIcon(ui.lbFace, IMIconInfo(info.groupID, info.faceURL, info.showName(), IMICON_GRP, ui.lbFace->size()));
	ui.recvContainer->init(this);
	updateUI();
}

void Conversation_Group::uninit()
{
	ImIconMgr::instance()->unbindIcon(ui.lbFace);
	ui.sendContainer->uninit();
	ui.recvContainer->uninit();

	ConversationBaseWidget::uninit();
}

void Conversation_Group::updateInfo(const CRIMConversationInfo &info)
{
	ConversationBaseWidget::updateInfo(info);

	ui.lbName->setText(info.showName());
	updateUI();
}


void Conversation_Group::clearMsg()
{
	ConversationBaseWidget::clearMsg();
	ui.sendContainer->clear();
	ui.recvContainer->clear();
}


void Conversation_Group::updateUI()
{
	UIStatus status = GS_NORMAL;
	const CRIMGrpInfo_Ex* pJoinedInfo = CRIMManager_Qt::instance()->getJoinedGrpInfo(m_conversationInfo.groupID);
	if (pJoinedInfo == NULL)//已不在群里，可能是解散，可能是退群
	{
		const CRIMGrpInfo* gQuitedInfo = CRIMManager_Qt::instance()->getQuittedGrpInfo(m_conversationInfo.groupID);
		if (gQuitedInfo != NULL && gQuitedInfo->status == CRIM_GroupStatus_Dismissed)
		{
			status = GS_DISMISSED;
		}
		else
		{
			status = GS_QUIT;
		}
	}
	else
	{
		status = (UIStatus)pJoinedInfo->status;
	}
	//群主、管理员没有禁言状态
	if (status == GS_MUTE)
	{
		const CRIMGrpMemberInfo* pMemInfo = CRIMManager_Qt::instance()->getGrpMemberInfo(m_conversationInfo.groupID, CRIMManager_Qt::instance()->getUserID());
		if (pMemInfo != NULL)
		{
			if (pMemInfo->roleLevel != CRIM_GMR_Member)
			{
				status = GS_NORMAL;
			}
		}
	}
	setUIStatus(status);
	if (pJoinedInfo != NULL)
	{
		fillGroupInfoData(pJoinedInfo);
	}
	else
	{
		ui.memberList->clearGroupInfo();
	}
}

void Conversation_Group::fillGroupInfoData(const CRIMGrpInfo_Ex* gInfo)
{
	ui.lbName->setText(gInfo->groupName);
	ImIconMgr::instance()->bindIcon(ui.lbFace, IMIconInfo(gInfo->groupID, gInfo->faceURL, gInfo->groupName, IMICON_GRP, DEFUALT_ICONSZ));
	ui.memberList->setGroupInfo(gInfo);
	ui.findEdit->clear();
	//我是管理员显示按钮
	const CRIMGrpMemberInfo* memberInfo = CRIMManager_Qt::instance()->getGrpMemberInfo(m_conversationInfo.groupID, CRIMManager_Qt::instance()->getUserID());
	bool bAddBtnVisible = memberInfo != NULL && memberInfo->isManager();
	if (gInfo->memberCount >= 100)//已达最大数量不显示
	{
		bAddBtnVisible = false;
	}
	ui.btnAddMember->setVisible(bAddBtnVisible);
	ui.lbMemberCount->setText(tr("群成员 %1人").arg(gInfo->memberCount));
	ui.findEdit->setVisible(gInfo->memberCount > 0);
}

void Conversation_Group::setUIStatus(UIStatus status)
{
	ui.toolBarWidget->setVisible(status != GS_DISMISSED && status != GS_QUIT);
	ui.memberWidget->setVisible(status != GS_DISMISSED && status != GS_QUIT);
	ui.sendContainer->setEnabled(status == GS_NORMAL);
	ui.sendContainer->ui.stackedWidget->setCurrentWidget(status == GS_NORMAL ? ui.sendContainer->ui.sendPage : ui.sendContainer->ui.defaultPage);
	QString statusDesc;
	if (status == GS_BLOCKED)
	{
		statusDesc = tr("群已被封");
	}
	else if (status == GS_MUTE)
	{
		statusDesc = tr("已禁言");
	}
	else if (status == GS_DISMISSED)
	{
		statusDesc = tr("群已解散");
	}
	else if (status == GS_QUIT)
	{
		statusDesc = tr("已退出群");
	}
	ui.sendContainer->ui.lbDisabledDesc->setText(statusDesc);
}

void Conversation_Group::setSplitterSize(const QList<int> &sz)
{
	ui.splitter->setSizes(sz);
}

QList<int> Conversation_Group::getSplitterSize()
{
	return ui.splitter->sizes();
}

QString Conversation_Group::getDraftTxt()
{
	QString draftTxt;
	QList<UIMsgInfo> curMsgInfos = ui.sendContainer->getCurrentMsg();
	for (auto &info : curMsgInfos)
	{
		draftTxt += info.getMsgPreviewText();
	}
	return draftTxt;
}

QTextDocumentFragment Conversation_Group::getTextFragment()
{
	return ui.sendContainer->getTextFragment();
}

void Conversation_Group::setTextFragment(const QTextDocumentFragment &doc)
{
	ui.sendContainer->setTextFragment(doc);
}

void Conversation_Group::slot_joinedGrpAdded(const CRIMGrpInfo &info)
{
	if (m_conversationInfo.groupID != info.groupID)
	{
		return;
	}
	updateUI();
}

void Conversation_Group::slot_grpInfoChanged(const CRIMGrpInfo &info)
{
	if (m_conversationInfo.groupID != info.groupID)
	{
		return;
	}
	updateUI();
}

void Conversation_Group::slot_actorChanged()
{
	updateUI();
}

void Conversation_Group::slot_joinedGrpDeleted(const CRIMGrpInfo &info)
{
	if (m_conversationInfo.groupID != info.groupID)
	{
		return;
	}
	updateUI();
}

void Conversation_Group::slot_joinedGrpDismissed(const CRIMGrpInfo &info)
{
	if (m_conversationInfo.groupID != info.groupID)
	{
		return;
	}
	updateUI();
}

void Conversation_Group::slot_dismissGrpFailed(int code, const QString &error, const QString &cookie)
{
	if (m_cookie != cookie)
	{
		return;
	}

	CRMsgBox::information(this, QString(), tr("解散群聊失败（%1）").arg(getErrDsc((CRIM_ERR_DEF)code)));
}

void Conversation_Group::slot_findTextChanged(const QString &str)
{
	ui.memberList->searchMember(str);
}

void Conversation_Group::slot_sendMsg(const QList<UIMsgInfo> &msgs)
{
	//添加需要发送的数据到UI
	ui.recvContainer->appendMsg(msgs, IMMsgWidget_Recv::LOC_MSG);

	//发送消息
	for (auto &msgInfo : msgs)
	{
		if (msgInfo.getMsgID().isEmpty())
		{
			continue;
		}
		CRIMOfflinePushInfo pushInfo;
		pushInfo.title = m_conversationInfo.showName();
		pushInfo.desc = msgInfo.senderNickname + ": " + msgInfo.getMsgPreviewText();
		CRIMManager_Qt::instance()->sendMsg(msgInfo, "", m_conversationInfo.groupID, pushInfo);
	}
}

void Conversation_Group::slot_btnSettingClicked()
{
	g_atMgr.getActionById(ACT_CFG_Grp, m_conversationInfo.groupID)->trigger();
}

void Conversation_Group::slot_btnAddMemberClicked()
{
	//添加群成员
	DlgAddGroupMember *dlgAdd = new DlgAddGroupMember(m_conversationInfo.groupID, this);
	dlgAdd->setAttribute(Qt::WA_DeleteOnClose);
	dlgAdd->exec();
}

void Conversation_Group::slot_act_editGroupNameClicked()
{
	const CRIMGrpInfo_Ex* gInfo = CRIMManager_Qt::instance()->getJoinedGrpInfo(m_conversationInfo.groupID);
	if (gInfo == NULL)
	{
		return;
	}
	//编辑群名
	QString name = QInputDialog::getText(this, tr("编辑群名"), "");
	if (name.isEmpty())
	{
		return;
	}

	CRIMGrpInfo info = *gInfo;
 	info.groupName = name;
	CRIMManager_Qt::instance()->setGrpInfo(info);
}

void Conversation_Group::slot_act_quitGroupClicked()
{
	const CRIMGrpInfo_Ex* gInfo = CRIMManager_Qt::instance()->getJoinedGrpInfo(m_conversationInfo.groupID);
	if (gInfo == NULL)
	{
		return;
	}
	
	//如果我是群主，并且群里大于一个人,需要转群主身份
	if (gInfo->ownerUserID == CRIMManager_Qt::instance()->getUserID() && gInfo->memberCount > 1)
	{
		CRMsgBox::information(this, tr(""), tr("请先转让群主"));
		return;
	}

	CRMsgBox::StandardButton rslt = CRMsgBox::question(this, QString(), tr("确定退出群聊？"), CRMsgBox::Ok | CRMsgBox::Cancel, CRMsgBox::Ok);
	if (rslt != CRMsgBox::Ok)
	{
		return;
	}
	//退群
	CRIMManager_Qt::instance()->quitGrp(m_conversationInfo.groupID);
	//删除会话
//	CRIMManager_Qt::instance()->deleteConversation(m_conversationInfo.conversationID);
}

void Conversation_Group::slot_act_dismissGruopClicked()
{
	CRMsgBox::StandardButton rslt = CRMsgBox::question(this, QString(), tr("确定解散群聊？"), CRMsgBox::Ok | CRMsgBox::Cancel, CRMsgBox::Ok);
	if (rslt != CRMsgBox::Ok)
	{
		return;
	}

	CRIMManager_Qt::instance()->dismissGrp(m_conversationInfo.groupID, m_cookie);
}

void Conversation_Group::slot_actAtUser_triggered()
{
	QAction *pAct = dynamic_cast<QAction*>(sender());
	if (pAct == NULL)
		return;

	QString groupID = pAct->property(EXT_GID).toString();
	QString userID = pAct->property(EXT_UID).toString();
	if (groupID != m_conversationInfo.groupID)
		return;

	//@群成员
	ui.sendContainer->insertAtText(userID);
}

void Conversation_Group::slot_actTransOwner_triggered()
{
	QAction *pAct = dynamic_cast<QAction*>(sender());
	if (pAct == NULL)
		return;

	QString groupID = pAct->property(EXT_GID).toString();
	QString userID = pAct->property(EXT_UID).toString();
	if (groupID != m_conversationInfo.groupID)
		return;

	//转让群主
	CRIMManager_Qt::instance()->transferGrpOwner(groupID, userID);
}

void Conversation_Group::slot_actSetHost_triggered()
{
	QAction *pAct = dynamic_cast<QAction*>(sender());
	if (pAct == NULL)
		return;

	QString groupID = pAct->property(EXT_GID).toString();
	QString userID = pAct->property(EXT_UID).toString();
	if (groupID != m_conversationInfo.groupID)
		return;

	const CRIMGrpMemberInfo* memInfo = CRIMManager_Qt::instance()->getGrpMemberInfo(groupID, userID);
	if (memInfo == NULL)
		return;

// 	CRMsgBox::StandardButton rslt = CRMsgBox::question(this, tr("设置管理员"), tr("确定设置%1为管理员？").arg(memInfo->nickname), CRMsgBox::Ok | CRMsgBox::Cancel, CRMsgBox::Ok);
// 	if (rslt != CRMsgBox::Ok)
// 	{
// 		return;
// 	}
	//设置管理员
	CRIMManager_Qt::instance()->setGrpMemberRoleLevel(groupID, userID, CRIM_GMR_Admin);
}

void Conversation_Group::slot_actUnsetHost_triggered()
{
	QAction *pAct = dynamic_cast<QAction*>(sender());
	if (pAct == NULL)
		return;

	QString groupID = pAct->property(EXT_GID).toString();
	QString userID = pAct->property(EXT_UID).toString();
	if (groupID != m_conversationInfo.groupID)
		return;

	const CRIMGrpMemberInfo* memInfo = CRIMManager_Qt::instance()->getGrpMemberInfo(groupID, userID);
	if (memInfo == NULL)
		return;

// 	CRMsgBox::StandardButton rslt = CRMsgBox::question(this, tr("收回管理员"), tr("收回%1的管理员？").arg(memInfo->nickname), CRMsgBox::Ok | CRMsgBox::Cancel, CRMsgBox::Ok);
// 	if (rslt != CRMsgBox::Ok)
// 	{
// 		return;
// 	}
	//收回管理员
	CRIMManager_Qt::instance()->setGrpMemberRoleLevel(groupID, userID, CRIM_GMR_Member);
}

void Conversation_Group::slot_actKick_triggered()
{
	QAction *pAct = dynamic_cast<QAction*>(sender());
	if (pAct == NULL)
		return;

	QString groupID = pAct->property(EXT_GID).toString();
	QString userID = pAct->property(EXT_UID).toString();
	if (groupID != m_conversationInfo.groupID)
		return;

	const CRIMGrpMemberInfo* memInfo = CRIMManager_Qt::instance()->getGrpMemberInfo(groupID, userID);
	if (memInfo == NULL)
		return;

	
	QString strNickName = QString("<span style=\"text-decoration: none; color:#2384C8;\">%1</span>").arg(memInfo->nickname);
	CRMsgBox::StandardButton rslt = CRMsgBox::question(this, QString(), tr("确定将%1从群里移出？").arg(strNickName), CRMsgBox::Ok | CRMsgBox::Cancel, CRMsgBox::Ok);
	if (rslt != CRMsgBox::Ok)
	{
		return;
	}
	//踢出群
	CRIMManager_Qt::instance()->kickGrpMember(groupID, QStringList()<<userID, "");
}
