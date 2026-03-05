#include "stdafx.h"
#include "Conversation_C2C.h"

Conversation_C2C::Conversation_C2C(QWidget *parent)
	: ConversationBaseWidget(parent)
{
	ui.setupUi(this);
	ui.chatTitleWidget->layout()->setContentsMargins(12, 0, 0, 0);
	ui.chatTitleWidget->layout()->setSpacing(12);
	ui.toolBarWidget->layout()->setContentsMargins(0, 0, 8, 0);
	ui.lbFace->setFixedSize(DEFUALT_ICONSZ);
	ui.splitter->setStretchFactor(0, 10);
	ui.splitter->setStretchFactor(1, 1);

	connect(ui.recvContainer, &IMMsgWidget_Recv::s_reEdit, ui.sendContainer, &MsgSendContainer::slot_reEdit);
	connect(ui.sendContainer, &MsgSendContainer::s_sendMsg, this, &Conversation_C2C::slot_sendMsg);
	connect(ui.btnSetting, &QAbstractButton::clicked, this, &Conversation_C2C::slot_btnSettingClicked);
	connect(ui.btnAudioMsg, &QAbstractButton::clicked, this, &Conversation_C2C::slot_startImCall);
	connect(ui.btnVideoMsg, &QAbstractButton::clicked, this, &Conversation_C2C::slot_startImCall);
	connect(ui.btnCreateGrp, &QAbstractButton::clicked, this, &Conversation_C2C::slot_createGrpClicked);
	connect(ui.lbFace, &QClickedLabel::clicked, this, &Conversation_C2C::slot_lbNameClicked);
	connect(ui.lbName, &QClickedLabel::clicked, this, &Conversation_C2C::slot_lbNameClicked);
	connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_friendAdded, this, &Conversation_C2C::slot_friendInfoChanged);
	connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_friendDeleted, this, &Conversation_C2C::slot_friendInfoChanged);
	connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_friendInfoChanged, this, &Conversation_C2C::slot_friendInfoChanged);
}

Conversation_C2C::~Conversation_C2C()
{

}

void Conversation_C2C::init(const CRIMConversationInfo &info)
{
	ConversationBaseWidget::init(info);
	ui.sendContainer->init(this, nullptr);
	ui.recvContainer->init(this);
	updateInfo(info);
	ImIconMgr::instance()->bindIcon(ui.lbFace, IMIconInfo(info.userID, info.faceURL, info.showName(), IMICON_USR, ui.lbFace->size()));
	updateFriendInfo();
}

void Conversation_C2C::uninit()
{
	ImIconMgr::instance()->unbindIcon(ui.lbFace);
	ui.sendContainer->uninit();
	ui.recvContainer->uninit();
	ConversationBaseWidget::uninit();
}

void Conversation_C2C::updateFriendInfo()
{
	const CRIMFriendInfo* friendInfo = CRIMManager_Qt::instance()->getFriendInfo(m_conversationInfo.userID);
	ui.toolBarWidget->setVisible(friendInfo != NULL);
	if (friendInfo != NULL)
	{
		ui.lbName->setText(friendInfo->getDisplayName());
	}
}

void Conversation_C2C::updateInfo(const CRIMConversationInfo &info)
{
	ConversationBaseWidget::updateInfo(info);

	ui.lbName->setText(info.showName());
}

void Conversation_C2C::clearMsg()
{
	ConversationBaseWidget::clearMsg();
	ui.sendContainer->clear();
	ui.recvContainer->clear();
}


void Conversation_C2C::slot_sendMsg(const QList<UIMsgInfo> &msgs)
{
	//非好友不能发送消息
	if (!CRIMManager_Qt::instance()->isFriend(m_conversationInfo.userID))
	{
		CRMsgBox::warning(this, tr("消息发送失败"), tr("他(她）已经不是你的好友，请重新添加"));
		return;
	}

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
		pushInfo.title = msgInfo.senderNickname;
		pushInfo.desc = msgInfo.getMsgPreviewText();
		CRIMManager_Qt::instance()->sendMsg(msgInfo, m_conversationInfo.userID, "", pushInfo);
	}
}

void Conversation_C2C::slot_startImCall()
{
	bool bVideoCall = (sender() == ui.btnVideoMsg);
	ACTION_ID aid = bVideoCall ? ACT_START_VideoChat1v1 : ACT_START_AudioChat1v1;
	g_atMgr.getActionById(aid, m_conversationInfo.userID)->trigger();
}

void Conversation_C2C::setSplitterSize(const QList<int> &sz)
{
	ui.splitter->setSizes(sz);
}

QList<int> Conversation_C2C::getSplitterSize()
{
	return ui.splitter->sizes();
}

QString Conversation_C2C::getDraftTxt()
{
	QString draftTxt;
	QList<UIMsgInfo> curMsgInfos = ui.sendContainer->getCurrentMsg();
	for (auto &info : curMsgInfos)
	{
		draftTxt += info.getMsgPreviewText();
	}
	return draftTxt;
}

QTextDocumentFragment Conversation_C2C::getTextFragment()
{
	return ui.sendContainer->getTextFragment();
}

void Conversation_C2C::setTextFragment(const QTextDocumentFragment &doc)
{
	ui.sendContainer->setTextFragment(doc);
}

void Conversation_C2C::slot_btnSettingClicked()
{
	g_atMgr.getActionById(ACT_CFG_Friend, m_conversationInfo.userID)->trigger();
}

void Conversation_C2C::slot_lbNameClicked()
{
	g_atMgr.getActionById(GMEM_ACT_SHOW_GrpMemberInfo, m_conversationInfo.groupID, m_conversationInfo.userID)->trigger();
}

void Conversation_C2C::slot_friendInfoChanged(const CRIMFriendInfo &info)
{
	if (m_conversationInfo.userID != info.userID && CRIMManager_Qt::instance()->isFriend(info.userID))
	{
		return;
	}
	updateFriendInfo();
}

void Conversation_C2C::slot_createGrpClicked()
{
	QAction *pAct = g_atMgr.getActionById(ACT_NEW_GrpChat, m_conversationInfo.userID);
	pAct->triggered(false);
}

