#include "stdafx.h"
#include "SessionItemWidget.h"
#include "CRIM_Manager.h"
#include "ConversationBaseWidget.h"

SessionListItem::SessionListItem(const CRIMConversationInfo &info, QListWidget *view)
{
	m_info = info;
	m_listView = view;
	if (!info.draftText.isEmpty())
	{
		CRIMManager_Qt::instance()->setConversationDraft(m_info.conversationID, "");
	}
	setBackgroundColor(info.isPinned ? QColor(244, 245, 246) : Qt::transparent);
}

SessionListItem::~SessionListItem()
{
	if (m_conversationUI != NULL)
	{
		m_conversationUI->uninit();
		m_conversationUI = NULL;
	}
}

void SessionListItem::enterConversation(ConversationBaseWidget *widget)
{
	//清空未读消息
	clearUnreadCount();
	//清除@消息标志位
	clearGroupAtType();
	//清除草稿
	CRIMManager_Qt::instance()->setConversationDraft(m_info.conversationID, "");

	m_conversationUI = widget;
	if (m_conversationUI != NULL)
	{
		m_conversationUI->init(m_info);
	}
}

void SessionListItem::leaveConversation()
{
	if (m_conversationUI != NULL)
	{
		m_conversationUI->uninit();
		//草稿处理
		QString draftTxt = m_conversationUI->getDraftTxt(); 
		if (!draftTxt.isEmpty())
		{
			m_lastDraftTime = QDateTime::currentMSecsSinceEpoch();
			CRIMManager_Qt::instance()->setConversationDraft(m_info.conversationID, draftTxt);
		}
		m_conversationUI = NULL;
	}
}

void SessionListItem::clearMsg()
{
	CRIMManager_Qt::instance()->clearConversationAndDeleteAllMsg(m_info.conversationID);

	//清空未读消息
	clearUnreadCount();
	//清除@消息标志位
	clearGroupAtType();
	//清除草稿
	CRIMManager_Qt::instance()->setConversationDraft(m_info.conversationID, "");

	if (m_conversationUI != nullptr)
	{
		m_conversationUI->clearMsg();
	}
}


void SessionListItem::clearUnreadCount()
{
	CRIMManager_Qt::instance()->markConversationMsgAsRead(m_info.conversationID);
}

void SessionListItem::clearGroupAtType()
{
	if (m_info.conversationType != CRIM_Conversation_Group)
		return;

	CRIMManager_Qt::instance()->resetConversationGrpAtType(m_info.conversationID);
}

void SessionListItem::updateInfo(const CRIMConversationInfo &info)
{
	if (m_info.conversationID != info.conversationID)
		return;

	m_info = info;
	setBackgroundColor(info.isPinned ? QColor(244, 245, 246) : Qt::transparent);
	if (m_listView->currentItem() == this)
	{
// 		//清空未读消息
// 		if (m_info.unreadCount > 0)
// 		{
// 			m_info.unreadCount = 0;
// 			clearUnreadCount();
// 		}
// 		//清除@消息标志位
// 		if (m_info.groupAtType != CRIM_GAT_AtNormal)
// 		{
// 			m_info.groupAtType = CRIM_GAT_AtNormal;
// 			clearGroupAtType();
// 		}
	}
	if (m_conversationUI != NULL)
	{
		m_conversationUI->updateInfo(m_info);
	}
}

void SessionListItem::setSplitterSize(const QList<int> &sz)
{
	if (m_conversationUI != NULL)
	{
		m_conversationUI->setSplitterSize(sz);
	}
}

QList<int> SessionListItem::getSplitterSize()
{
	if (m_conversationUI == NULL)
	{
		return QList<int>() << 300 << 100;
	}
	return m_conversationUI->getSplitterSize();
}

QTextDocumentFragment SessionListItem::getTextFragment()
{
	if (m_conversationUI != NULL)
	{
		return m_conversationUI->getTextFragment();
	}
	return QTextDocumentFragment();
}

void SessionListItem::setTextFragment(const QTextDocumentFragment &doc)
{
	if (m_conversationUI != NULL)
	{
		return m_conversationUI->setTextFragment(doc);
	}
}

///////////////////////////////////////////////
SessionItemWidget::SessionItemWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	ui.horizontalLayout->setContentsMargins(12, 0, 9, 0);
	ui.horizontalLayout->setSpacing(15);
	ui.verticalLayout->setContentsMargins(0, 0, 0, 2);
	ui.verticalLayout->setSpacing(5);
	ui.lbConversationFace->setFixedSize(DEFUALT_ICONSZ);

	m_menu = new QMenu(this);
	QAction *act_del = new QAction(tr("删除聊天"), this);
	m_menu->addAction(act_del);
	connect(act_del, &QAction::triggered, this, &SessionItemWidget::slot_delTriggered);
#ifdef MAC
    this->setMaximumWidth(246);
#endif
}

SessionItemWidget::~SessionItemWidget()
{

}

void SessionItemWidget::clear()
{

}

void SessionItemWidget::setInfo(const CRIMConversationInfo &info)
{
	m_info = info;
	if (info.conversationType == CRIM_Conversation_1v1)
	{
		ImIconMgr::instance()->bindIcon(ui.lbConversationFace, IMIconInfo(info.userID, info.faceURL, info.showName(), IMICON_USR, ui.lbConversationFace->size()));
	}
	else
	{
		ImIconMgr::instance()->bindIcon(ui.lbConversationFace, IMIconInfo(info.groupID, info.faceURL, info.showName(), IMICON_GRP, ui.lbConversationFace->size()));
	}

	qint64 lastTime_t = info.latestMsgSendTime;
	QDateTime lastTime = QDateTime::fromMSecsSinceEpoch(lastTime_t);
	QLocale locale(QLocale::Chinese);
	QString timeTxt = locale.toString(lastTime, tr("hh:mm"));
	int offsetDay = qAbs(lastTime.daysTo(QDateTime::currentDateTime()));
	if (offsetDay == 1)
	{
		timeTxt = tr("昨天");
	}
	else if (offsetDay > 1)
	{
		timeTxt = locale.toString(lastTime, tr("MM/dd"));
	}
	ui.lbLastTime->setText(timeTxt);
	ui.lbName->setText(info.showName());

	QString strMsgNote;
	QString strContent = getContentText(strMsgNote);
 	ui.lbMsgNote->setText(strMsgNote);
	ui.lbMsgNote->setVisible(!strMsgNote.isEmpty());
 	ui.lbLastMsg->setText(strContent);


	//未读消息(免打扰时，为disable状态）
	ui.lbUnRead->setDisabled(info.recvMsgOpt == CRIM_RecvMsgOpt_NotNotify);
	QString unreadCountStr = info.unreadCount > 99 ? QString("99+") : QString::number(info.unreadCount);
	ui.lbUnRead->setText(unreadCountStr);
	WidgetStyleUpdate(ui.lbUnRead);
}

QString SessionItemWidget::getContentText(QString &rsltPrefix)
{
	QString strPrefix, strContent;
	QString drafText = m_info.draftText.trimmed();
	if (!drafText.isEmpty())//草稿
	{
		strPrefix = tr("[草稿]");
		strContent = drafText;
	}
	else if (m_info.groupAtType == CRIM_GAT_AtAllAtMe 
		|| m_info.groupAtType == CRIM_GAT_AtAll 
		|| m_info.groupAtType == CRIM_GAT_AtMe)//at消息
	{
		strPrefix = tr("[@我]");//Mentionde you
	}
	
	if (m_info.latestMsg != NULL && strContent.isEmpty())
	{
		UIMsgInfo uiMsgInfo = *m_info.latestMsg;
		strContent = uiMsgInfo.getMsgPreviewText();
	}
	rsltPrefix = strPrefix;
	return strContent;
}

void SessionItemWidget::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		QWidget::mousePressEvent(event);
	}
}

void SessionItemWidget::mouseReleaseEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		QWidget::mouseReleaseEvent(event);
	}
	else if (event->button() == Qt::RightButton)
	{
		//弹出菜单不切换选中状态
		m_menu->exec(QCursor::pos());
	}
}

void SessionItemWidget::slot_delTriggered()
{
	emit s_delConversation(m_info.conversationID);
}
