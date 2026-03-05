#include "stdafx.h"
#include "IMMsgWidget_Search.h"
#include "QActionStyleBtn.h"
#include "MessageMgrPage.h"
#include "SearchUI/GlobalSearch_defs.h"

#define EACH_PAGE_COUNT	100

IMMsgWidget_Search::IMMsgWidget_Search(QWidget *parent) : IMMsgWidget(parent)
{
	this->setType(IMMsgWidget::MSGS_SEARCH);
	this->setChatViewMode(ChatStyle::CHAT_LIST);//列表显示模式

	connect(this, &QListView::entered, this, &IMMsgWidget_Search::slot_itemEntered);
	connect(this, &QListView::viewportEntered, this, &IMMsgWidget_Search::slot_viewportEntered);
	
	//g_AudioMgr_uConnect
	m_btnViewContext = new QActionStyleBtn(this->viewport());
	m_btnViewContext->setCursor(Qt::PointingHandCursor);
	m_btnViewContext->setObjectName("btnViewContext");
	m_btnViewContext->hide();

	m_pItem = NULL;

	connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_searchLocalMsgsSuccess, this, &IMMsgWidget_Search::slot_searchLocalMsgsSuccess);
	connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_searchLocalMsgsFailed, this, &IMMsgWidget_Search::slot_searchLocalMsgsFailed);
	connect(this, &ChatWidget::s_loadMoreClicked, this, &IMMsgWidget_Search::slot_loadMoreClicked);
}

IMMsgWidget_Search::~IMMsgWidget_Search()
{

}

bool IMMsgWidget_Search::isSessionMsg(const CRIMMessage &msg)
{
	if (UIMsgInfo::isSystemMsg(msg.contentType))
	{
		return false;
	}
	return IMMsgWidget::isSessionMsg(msg);
}

void IMMsgWidget_Search::init(GlobalSearchConvsationItem *pItem, const QString &searchText, int searchTime)
{
	this->clear();

	m_pItem = pItem;
	m_searchText = searchText;
	m_searchTime = searchTime;
	const CRIMConversationInfo* pConv = MessageMgrPage::instance()->getConversationInfo(pItem->_ID);
	m_conversationInfo = *pConv;
	m_loadedPage = 0;
	m_queryTaskID.clear();
	setDataState(ChatWidget::DS_CANLOAD);

	slot_loadMoreClicked();
}

void IMMsgWidget_Search::slot_loadMoreClicked()
{
	int loadedCount = m_loadedPage * EACH_PAGE_COUNT;
	int needCount = loadedCount + EACH_PAGE_COUNT;

	//有更多缓存数据
	if (m_pItem->_rslt.messageList.size() > loadedCount)
	{
		addOnePageMsg();
		return;
	}

	//db 中有更多数据
	if (m_pItem->_rslt.messageCount > loadedCount)
	{
		if (m_queryTaskID.isEmpty())
		{
			setDataState(ChatWidget::DS_LOADING);
			QStringList keyList; keyList.push_back(m_searchText);
			m_queryTaskID = m_conversationInfo.conversationID.toUtf8() + "\1" + m_searchText.toUtf8() + "\1" + QByteArray::number(m_loadedPage);
			CRIMManager_Qt::instance()->searchLocalMsgs(m_conversationInfo.conversationID, keyList, 0, QStringList(), QList<CRIMMsgType>(), m_searchTime, 0, m_loadedPage + 1, EACH_PAGE_COUNT, m_queryTaskID);
		}
		return;
	}

	setDataState(ChatWidget::DS_NO_MOREDATA);
}


void IMMsgWidget_Search::slot_searchLocalMsgsSuccess(const CRIMSearchResultInfo &rslt, const QString &cookie)
{
	if (cookie != m_queryTaskID)
		return;
	m_queryTaskID.clear();

	if (rslt.items.size() > 0)
	{
		//把查询的内容追加到缓存
		m_pItem->_rslt.messageList += rslt.items[0].messageList;
	}
	addOnePageMsg();
}

void IMMsgWidget_Search::addOnePageMsg()
{
	int startPos = m_loadedPage * EACH_PAGE_COUNT;
	int endPos = startPos + EACH_PAGE_COUNT;

	QList<UIMsgInfo> uiMsgs;
	for (int i = startPos; i < endPos && i<m_pItem->_rslt.messageList.size(); i++)
	{
		CRIMMessagePtr &msg = m_pItem->_rslt.messageList[i];
		// 过滤不支持的类型
		if (!isSessionMsg(*msg))
		{
			continue;
		}
		UIMsgInfo uiMsg = *msg;
		uiMsg.highlighterText = m_searchText;
		uiMsgs.push_front(uiMsg);
	}
	this->insertMsg(uiMsgs, true);
	m_loadedPage++;

	if ((m_loadedPage*EACH_PAGE_COUNT) >= m_pItem->_rslt.messageCount)
	{
		setDataState(ChatWidget::DS_NO_MOREDATA);
	}
	else
	{
		setDataState(ChatWidget::DS_CANLOAD);
	}
}


void IMMsgWidget_Search::slot_searchLocalMsgsFailed(int code, const QString &error, const QString &cookie)
{
	m_queryTaskID.clear();
	CRIMMgrLogWarn("searchLocalMessage failed(%d,%s), searchText:%s, page:%d", code, error.toUtf8().constData(), cookie.constData(), m_loadedPage);
}


void IMMsgWidget_Search::slot_itemEntered(const QModelIndex &index)
{
	QRect itemRt = this->visualRect(index);
	const UIMsgInfo &msgInfo = this->getMsgInfo(index);
	m_btnViewContext->SetAction(g_atMgr.getActionById(ACT_IM_VIEWCONTEXT, "", QVariant::fromValue<UIMsgInfo>(msgInfo)), QActionStyleBtn::BIND_ALL & ~QActionStyleBtn::BIND_VISIABLE);
	m_btnViewContext->show();
	m_btnViewContext->move(itemRt.right() - m_btnViewContext->width() - 6, itemRt.top() + 25);
}

void IMMsgWidget_Search::slot_viewportEntered()
{
	m_btnViewContext->hide();
}

void IMMsgWidget_Search::leaveEvent(QEvent *event)
{
	IMMsgWidget::leaveEvent(event);
	slot_viewportEntered();
}

void IMMsgWidget_Search::onMenuExec(QMenu *menu, ChatWidget::MenuType menuType, const UIMsgInfo &msg)
{
	if (menuType == ChatWidget::MENU_CONTENT)//右键内容区域
	{
		menu->addAction(g_atMgr.getActionById(ACT_IM_VIEWCONTEXT, "", QVariant::fromValue<UIMsgInfo>(msg)));
	}
}
