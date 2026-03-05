#include "stdafx.h"
#include "ConversationBaseWidget.h"
#include "CRIM_Manager.h"

ConversationBaseWidget::ConversationBaseWidget(QWidget *parent)
	: QWidget(parent)
{
}

ConversationBaseWidget::~ConversationBaseWidget()
{

}

void ConversationBaseWidget::init(const CRIMConversationInfo &info)
{
	m_conversationInfo = info;
}

void ConversationBaseWidget::uninit()
{
	m_conversationInfo = CRIMConversationInfo();
}

void ConversationBaseWidget::updateInfo(const CRIMConversationInfo &info)
{
	if (m_conversationInfo.conversationID != info.conversationID)
	{
		return;
	}
	m_conversationInfo = info;
}

const QString& ConversationBaseWidget::getID() const
{
	return m_conversationInfo.conversationType == CRIM_Conversation_Group ? m_conversationInfo.groupID : m_conversationInfo.userID;
}

const QString& ConversationBaseWidget::getConversationID() const
{
	return m_conversationInfo.conversationID;
}
