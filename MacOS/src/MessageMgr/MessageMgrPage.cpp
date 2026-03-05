#include "stdafx.h"
#include "MessageMgrPage.h"
#include "ConversationBaseWidget.h"
#include "Conversation_C2C.h"
#include "Conversation_Group.h"
#include "MainDialog.h"

MessageMgrPage *MessageMgrPage::m_instance = NULL;
MessageMgrPage::MessageMgrPage(QWidget *parent)
	: QWidget(parent)
{
	MessageMgrPage::m_instance = this;
	ui.setupUi(this);
	ui.searchEdt->setFocus();

	m_curConversation = NULL;
	ConversationBaseWidget *c2c = new Conversation_C2C();
	ConversationBaseWidget *group = new Conversation_Group();
	c2c->hide();
	group->hide();
	ui.clientLayout->addWidget(c2c);
	ui.clientLayout->addWidget(group);
	m_conversations[CRIM_Conversation_1v1] = c2c;
	m_conversations[CRIM_Conversation_Group] = group;

	ui.sessionList->setMessageMgrPage(this);
	connect(ui.sessionList, &SessionListWidget::s_setCurrentConversation, this, &MessageMgrPage::slot_setCurrentConversation);
	connect(ui.searchEdt, &CRLineEdit::textChanged, MainDialog::instance(), &MainDialog::slot_searchTextChanged);

	connect(g_atMgr.getActionById(ACT_ClearConverstationMsg), &QAction::triggered, this, &MessageMgrPage::slot_clearConversationMsg);
	
}

MessageMgrPage::~MessageMgrPage()
{
	for (auto it = m_conversations.begin(); it != m_conversations.end(); it++)
	{
		it.value()->deleteLater();
	}
	m_conversations.clear();
	MessageMgrPage::m_instance = NULL;
}

QList<const CRIMConversationInfo*> MessageMgrPage::getConversationList(bool bOnlyUnread)
{
	return ui.sessionList->getConversationList(bOnlyUnread);
}

const CRIMConversationInfo* MessageMgrPage::getConversationInfo(const QString &conversationID)
{
	return ui.sessionList->getConversationInfo(conversationID);
}

const CRIMConversationInfo* MessageMgrPage::getConversationByUserID(const QString &userID)
{
	return ui.sessionList->getConversationInfoBySourceId(userID, CRIM_Conversation_1v1);
}

const CRIMConversationInfo* MessageMgrPage::getConversationByGroupID(const QString &groupID)
{
	return ui.sessionList->getConversationInfoBySourceId(groupID, CRIM_Conversation_Group);
}

void MessageMgrPage::switchConversation(const QString &id, CRIMConversationType type)
{
	ui.sessionList->switchConversation(id, type);
}

void MessageMgrPage::switchConversation(const QString &conversationId)
{
	ui.sessionList->switchConversation(conversationId);
}

void MessageMgrPage::clearAllUnreadCount(const QStringList &conversationIds)
{
	ui.sessionList->clearAllUnreadCount(conversationIds);
}

void MessageMgrPage::jumpNextUnreadConversation()
{
	ui.sessionList->jumpNextUnreadConversation();
}


void MessageMgrPage::slot_setCurrentConversation(const CRIMConversationInfo &info)
{
	if (m_curConversation != NULL && m_curConversation->getConversationID() == info.conversationID && !info.conversationID.isEmpty())
	{
		return;
	}

	if (m_curConversation != NULL)
	{
		m_curConversation->hide();
		m_curConversation = NULL;
	}

	if (!info.conversationID.isEmpty())
	{
		ConversationBaseWidget *conversationWidget = getConversationWidget(info.conversationType);
		if (conversationWidget != NULL)
		{
			ui.defaultPage->hide();
			conversationWidget->show();
			m_curConversation = conversationWidget;
		}
	}
}

void MessageMgrPage::slot_clearConversationMsg()
{
	QAction *pAction = dynamic_cast<QAction*>(sender());
	if (pAction == nullptr)
		return;
	QString conversationID = pAction->property(EXT_ID).toString();
	if (conversationID.isEmpty())
		return;

	ui.sessionList->clearMsg(conversationID);
}

ConversationBaseWidget* MessageMgrPage::getConversationWidget(CRIMConversationType type)
{
	return m_conversations[type];
}

