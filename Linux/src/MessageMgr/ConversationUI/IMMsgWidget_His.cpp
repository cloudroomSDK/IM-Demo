#include "stdafx.h"
#include "IMMsgWidget_His.h"

#define LOAD_HIS_COUNT 50	//每次加载历史消息的条数
IMMsgWidget_His::IMMsgWidget_His(QWidget *parent) : IMMsgWidget(parent)
{
	this->setType(IMMsgWidget::MSGS_HIS);
	this->setChatViewMode(ChatStyle::CHAT_LIST);//列表显示模式

	connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_getHistoryMsgListSuccess, this, &IMMsgWidget_His::slot_getHistoryMsgListSuccess);
	connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_getHistoryMsgListFailed, this, &IMMsgWidget_His::slot_getHistoryMsgListFailed);
	connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_getHistoryMsgListReverseSuccess, this, &IMMsgWidget_His::slot_getHistoryMsgListSuccess);
	connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_getHistoryMsgListReverseFailed, this, &IMMsgWidget_His::slot_getHistoryMsgListFailed);

	m_loadHisCount = 0;
	m_cookie = makeUUID().toUtf8(); 
	m_reverseCookie = makeUUID().toUtf8();
	this->verticalScrollBar()->installEventFilter(this);
}

IMMsgWidget_His::~IMMsgWidget_His()
{

}

void IMMsgWidget_His::loadHistroyMsgs(const UIMsgInfo &fromMsg, const QString &highlighterText)
{
	clear();

	m_highlighterText = highlighterText;
	m_fromMsgId = fromMsg.clientMsgID;
	if (fromMsg.clientMsgID.isEmpty())
	{
		loadData(fromMsg, LOAD_FRONT, LOAD_HIS_COUNT, true);
	}
	else
	{
		m_firstMsg = m_lastMsg = fromMsg; 
		this->insertMsg(QList<UIMsgInfo>() << fromMsg);
		loadData(fromMsg, LOAD_BOTH, LOAD_HIS_COUNT / 2, true);
	}
}

void IMMsgWidget_His::loadData(const UIMsgInfo &msg, LoadType type, int count, bool bFisrtLoad)
{
	if (m_loadHisCount > 0)
	{
		return;
	}

	m_loadHisCount = type == LOAD_BOTH ? 2 : 1;
	QVariantMap params;
	params["fisrtLoad"] = bFisrtLoad;
	if (type == LOAD_FRONT || type == LOAD_BOTH)
	{
		params["userCookie"] = m_cookie;
		QByteArray jsonCookie = QJsonDocument::fromVariant(params).toJson();
		CRIMManager_Qt::instance()->getHistoryMsgList(m_conversationInfo.conversationID, msg.clientMsgID, count, jsonCookie);
	}
	if (type == LOAD_BACK || type == LOAD_BOTH)
	{
		params["userCookie"] = m_reverseCookie;
		QByteArray jsonCookie = QJsonDocument::fromVariant(params).toJson();
		CRIMManager_Qt::instance()->getHistoryMsgListReverse(m_conversationInfo.conversationID, msg.clientMsgID, count, jsonCookie);
	}
}

void IMMsgWidget_His::slot_getHistoryMsgListSuccess(const QList<CRIMMessage> &msg, bool isEnd, const QString &cookie)
{
	QVariantMap params = QJsonDocument::fromJson(cookie.toUtf8()).toVariant().toMap();
	QString usrCookie = params["userCookie"].toString();
	if (m_cookie != usrCookie && m_reverseCookie != usrCookie)
	{
		return;
	}

	onLoadHistRslt();
	if (msg.size() <= 0)
	{
		return;
	}
	bool insertToFirst = m_cookie == usrCookie;
	if (insertToFirst)
	{
		m_firstMsg = msg.first();
	}
	else
	{
		m_lastMsg = msg.last();
	}
	appendMsg(msg, insertToFirst);
	
	if (m_loadHisCount == 0 && !m_fromMsgId.isEmpty() && params["fisrtLoad"].toBool())
	{
		slot_scrollToFromMsg();
		//QTimer::singleShot(500, this, SLOT(slot_scrollToFromMsg()));
	}
}

void IMMsgWidget_His::slot_getHistoryMsgListFailed(int code, const QString &error, const QString &cookie)
{
	QVariantMap params = QJsonDocument::fromJson(cookie.toUtf8()).toVariant().toMap();
	QString usrCookie = params["userCookie"].toString();
	if (m_cookie != usrCookie && m_reverseCookie != usrCookie)
	{
		return;
	}

	onLoadHistRslt();
}

void IMMsgWidget_His::onLoadHistRslt()
{
	m_loadHisCount--;
}

void IMMsgWidget_His::slot_scrollToFromMsg()
{
	this->scrollToMsg(m_fromMsgId);
}

bool IMMsgWidget_His::isSessionMsg(const CRIMMessage &msg)
{
	if (UIMsgInfo::isSystemMsg((msg.contentType)))
	{
		return false;
	}
	return IMMsgWidget::isSessionMsg(msg);
}

void IMMsgWidget_His::appendMsg(const QList<CRIMMessage> &msgs, bool insertFirst)
{
	QList<UIMsgInfo> newMsgList;
	for (auto &msgInfo : msgs)
	{
		// 过滤不支持的类型
		if (!isSessionMsg(msgInfo))
		{
			continue;
		}
		UIMsgInfo uiMsgInfo = msgInfo;
		uiMsgInfo.highlighterText = m_highlighterText;
		newMsgList.append(uiMsgInfo);
	}
	this->insertMsg(newMsgList, insertFirst);
}

bool IMMsgWidget_His::eventFilter(QObject *obj, QEvent *ev)
{
	bool ret = IMMsgWidget::eventFilter(obj, ev);
	if (obj == this->verticalScrollBar() && ev->type() == QEvent::Wheel)
	{
		bool bScrollTop = this->verticalScrollBar()->value() == 0;
		bool bScrollBottom = this->verticalScrollBar()->value() == this->verticalScrollBar()->maximum();
		UIMsgInfo msgInfo;
		if (bScrollTop)
		{
			msgInfo = m_firstMsg;
		}
		else if (bScrollBottom)
		{
			msgInfo = m_lastMsg;
		}
		if (!msgInfo.clientMsgID.isEmpty())
		{
			static QTime lastWheelEvent;
			static int wheelEventStep;
			//鼠标滚动
			QWheelEvent *wheelEvent = dynamic_cast<QWheelEvent *>(ev);

			//超过时间范围,重置状态
			if (lastWheelEvent.elapsed() <= 0 || lastWheelEvent.elapsed() > 1000)
			{
				wheelEventStep = 0;
				lastWheelEvent.start();
				return false;
			}
			else//1秒钟内的滚动
			{
				wheelEventStep += qAbs(wheelEvent->delta()) / 8 / 15;//鼠标滚动的步数
				//3步才加载数据
				if (wheelEventStep >= 3)
				{
					loadData(msgInfo, bScrollTop ? LOAD_FRONT : LOAD_BACK, LOAD_HIS_COUNT);
				}
			}
		}
	}
	return ret;
}
