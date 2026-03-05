#include "stdafx.h"
#include "FriendDetailWidget.h"
#include "MessageMgrPage.h"

FriendDetailWidget::FriendDetailWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	ui.friendBaseLayout->setContentsMargins(12, 12, 9, 12);
	ui.friendBaseLayout->setSpacing(30);
	ui.formLayout->setContentsMargins(12, 12, 12, 12);
	ui.lbFriendFace->setFixedSize(DEFUALT_ICONSZ);

	ui.privateChatTime->addItem("15秒", 15);
	ui.privateChatTime->addItem("30秒", 30);
	ui.privateChatTime->addItem("1分钟", 60);
	ui.privateChatTime->addItem("5分钟", 300);
	ui.privateChatTime->setCurrentIndex(0);

	connect(DemoSvrWebAPI::instance(), &DemoSvrWebAPI::s_queryUserInfoRslt, this, &FriendDetailWidget::slot_queryUserInfoRslt);
	connect(ui.btnRemark, &QToolButton::clicked, ui.remark, &CRTxtEditor::startEdit);
	connect(ui.remark, &CRTxtEditor::s_commit, this, &FriendDetailWidget::slot_commitRemark);
	connect(ui.btnPin, &QCheckBox::stateChanged, this, &FriendDetailWidget::slot_btnPinStateChanged);
	connect(ui.btnDnd, &QCheckBox::stateChanged, this, &FriendDetailWidget::slot_btnDndStateChanged);
	connect(ui.btnPrivateChat, &QCheckBox::stateChanged, this, &FriendDetailWidget::slot_btnPrivateChatChanged);
	connect(ui.privateChatTime, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_currentIndexChanged(int)));

	connect(ui.btnFriendChat, &QPushButton::clicked, this, &FriendDetailWidget::slot_btnFriendChatClicked);
	connect(ui.btnClearFriend, &QPushButton::clicked, this, &FriendDetailWidget::slot_btnClearFriendClicked);
	connect(ui.btnDelFriend, &QPushButton::clicked, this, &FriendDetailWidget::slot_btnDelFriendClicked);
	connect(ui.btnAudioChat, &QPushButton::clicked, this, &FriendDetailWidget::slot_audioChatBtnCliecked);
	connect(ui.btnVideoChat, &QPushButton::clicked, this, &FriendDetailWidget::slot_videoChatBtnCliecked);
	
}

FriendDetailWidget::~FriendDetailWidget()
{

}

void FriendDetailWidget::setFriendInfo(const QString &userID)
{
	if (m_uInfo.userID != userID)
	{
		m_uInfo = DemoSvrWebAPI::UserInfo();
		m_uInfo.userID = userID;
	}

	DemoSvrWebAPI::instance()->queryUserInfo(userID);
	updateUserInfoToUI();
}

void FriendDetailWidget::updateUserInfoToUI()
{
	ui.lbFriendFace->setPixmap(QPixmap());
	ui.lbFriendName->setText(QString());
	ui.id->setText(QString());
	ui.gender->setText(QString());
	ui.birth->setText(QString());
	ui.phone->setText(QString());
	ui.email->setText(QString());
	ui.remark->setText(QString());

	const CRIMFriendInfo *info = CRIMManager_Qt::instance()->getFriendInfo(m_uInfo.userID);
	if (info == NULL)
	{
		return;
	}

	ui.remark->setText(info->remark);
	m_bInitializing = true;
	{
		const CRIMConversationInfo* conversationInfo = MessageMgrPage::instance()->getConversationByUserID(info->userID);
		ui.btnPin->setChecked(conversationInfo != NULL && conversationInfo->isPinned);
		ui.btnDnd->setChecked(conversationInfo != NULL && conversationInfo->recvMsgOpt == CRIM_RecvMsgOpt_NotNotify);
		ui.btnPrivateChat->setChecked(conversationInfo != NULL && conversationInfo->isPrivateChat);
		int curPrivateTime = (conversationInfo == nullptr) ? DEFUALT_BURNDURATION : conversationInfo->burnDuration;
		int timeIndex = ui.privateChatTime->findData(curPrivateTime);
		if (timeIndex < 0){
			timeIndex = ui.privateChatTime->findData(DEFUALT_BURNDURATION);
		}
		ui.privateChatTime->setCurrentIndex(timeIndex);
		udpateBurnDuration();
	}
	m_bInitializing = false;

	//好友基本信息
	ImIconMgr::instance()->bindIcon(ui.lbFriendFace, IMIconInfo(info->userID, IMICON_USR, ui.lbFriendFace->size()));
	ui.lbFriendName->setText(info->nickname);
	ui.id->setText(m_uInfo.phoneNumber);
	ui.remark->setText(info->remark);
	ui.gender->setText(getGenderStr(m_uInfo.gender));
	QString birthDay = (m_uInfo.birth > 0) ? QDateTime::fromMSecsSinceEpoch(m_uInfo.birth).date().toString("yyyy/MM/dd") : QString();
	ui.birth->setText(birthDay);
	ui.phone->setText(m_uInfo.phoneNumber);
	ui.email->setText(m_uInfo.email);
}

void FriendDetailWidget::slot_queryUserInfoRslt(const QString &errMsg, const DemoSvrWebAPI::UserInfo &uInfo)
{
	if (!errMsg.isEmpty())
		return;
	if (m_uInfo.userID != uInfo.userID)
		return;

	m_uInfo = uInfo;
	updateUserInfoToUI();
}

void FriendDetailWidget::slot_commitRemark(const QString &oldStr, const QString &newStr)
{
	QString myUserID = CRIMManager_Qt::instance()->getUserID();
	if (oldStr == newStr)
	{
		return;
	}

	CRIMManager_Qt::instance()->setFriendRemark(m_uInfo.userID, newStr);
}

void FriendDetailWidget::slot_btnPinStateChanged(int state)
{
	if (m_bInitializing) return;
	QString converSationID = CRIMManager_Qt::instance()->getConversationIDBySessionType(m_uInfo.userID, CRIM_Conversation_1v1);
	CRIMManager_Qt::instance()->pinConversation(converSationID, state != 0);
}

void FriendDetailWidget::slot_btnDndStateChanged(int state)
{
	if (m_bInitializing) return;
	QString converSationID = CRIMManager_Qt::instance()->getConversationIDBySessionType(m_uInfo.userID, CRIM_Conversation_1v1);
	CRIMReceiveMsgOpt opt = (state == 0) ? CRIM_RecvMsgOpt_Receive : CRIM_RecvMsgOpt_NotNotify;
	CRIMManager_Qt::instance()->setConversationRecvMsgOpt(converSationID, opt);
}

void FriendDetailWidget::slot_btnPrivateChatChanged(int state)
{
	if (m_bInitializing) return;

	udpateBurnDuration();
	QString converSationID = CRIMManager_Qt::instance()->getConversationIDBySessionType(m_uInfo.userID, CRIM_Conversation_1v1);
	bool bPrivate = (state != 0);
	int time = ui.privateChatTime->currentData().toInt();
	CRIMManager_Qt::instance()->setConversationPrivateChat(converSationID, bPrivate, time);
}

void FriendDetailWidget::slot_currentIndexChanged(int index)
{
	if (m_bInitializing) return;

	QString converSationID = CRIMManager_Qt::instance()->getConversationIDBySessionType(m_uInfo.userID, CRIM_Conversation_1v1);
	bool bPrivate = ui.btnPrivateChat->isChecked();
	int time = ui.privateChatTime->currentData().toInt();
	CRIMManager_Qt::instance()->setConversationPrivateChat(converSationID, bPrivate, time);
}

void FriendDetailWidget::udpateBurnDuration()
{
	bool bPrivate = ui.btnPrivateChat->isChecked();
	ui.PrivateChatTimeDesc->setVisible(bPrivate);
	ui.privateChatTime->setVisible(bPrivate);
}


void FriendDetailWidget::slot_btnFriendChatClicked()
{
	g_atMgr.getActionById(ACT_START_Chat1v1, m_uInfo.userID)->trigger();
}

void FriendDetailWidget::slot_audioChatBtnCliecked()
{
	g_atMgr.getActionById(ACT_START_AudioChat1v1, m_uInfo.userID)->trigger();
	emit s_audioCallClicked();
}

void FriendDetailWidget::slot_videoChatBtnCliecked()
{
	g_atMgr.getActionById(ACT_START_VideoChat1v1, m_uInfo.userID)->trigger();
	emit s_videoCallClicked();
}

void FriendDetailWidget::slot_btnClearFriendClicked()
{
	if (CRMsgBox::warning(this, tr("清空聊天记录"), tr("聊天记录清空后将无法恢复，您确认要清空吗？"), CRMsgBox::Ok | CRMsgBox::Cancel) != CRMsgBox::Ok)
		return;
	QString converSationID = CRIMManager_Qt::instance()->getConversationIDBySessionType(m_uInfo.userID, CRIM_Conversation_1v1);
	g_atMgr.getActionById(ACT_ClearConverstationMsg, converSationID)->trigger();
	emit s_clearMsgClicked();
}

void FriendDetailWidget::slot_btnDelFriendClicked()
{
	if (CRMsgBox::warning(this, tr("删除好友"), tr("删除好友后将无法再聊天，您确认要删除吗？"), CRMsgBox::Ok | CRMsgBox::Cancel) != CRMsgBox::Ok)
		return;
	CRIMManager_Qt::instance()->deleteFriend(m_uInfo.userID);
	emit s_delFriendClicked();
}
