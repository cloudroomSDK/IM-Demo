#include "stdafx.h"
#include "IMMsgWidget_Recv.h"
#include "ConversationBaseWidget.h"
#include "AutoDelMessageMgr.h"

#define MSG_MAX_COUNT 5000	//消息最大显示数量
#define LOAD_HIS_COUNT 50	//每次加载历史消息的条数

IMMsgWidget_Recv::IMMsgWidget_Recv(QWidget *parent) : IMMsgWidget(parent)
{
	this->setType(IMMsgWidget::MSGS_RECV);
	this->setChatViewMode(ChatStyle::CHAT_BUBBLE);//气泡显示模式
	this->setMaxCount(MSG_MAX_COUNT);

	m_unreadTipsWidget = new QWidget(this);
	m_unreadTipsWidget->setObjectName("recvUnreadTipsWidget");
	uiUnreadTips.setupUi(m_unreadTipsWidget);
	m_unreadTipsWidget->hide();
	connect(uiUnreadTips.btnUnreadAt, &QPushButton::clicked, this, &IMMsgWidget_Recv::slot_btnUnreadAtClicked);
	connect(uiUnreadTips.btnUnreadMsg, &QPushButton::clicked, this, &IMMsgWidget_Recv::slot_btnUnreadMsgClicked);
	connect(uiUnreadTips.btnCloseTips, &QPushButton::clicked, this, &IMMsgWidget_Recv::slot_closeUnreadTips);
	m_previewMsgWidget = new QPushButton(this);
	m_previewMsgWidget->setObjectName("MsgTipsWidget");
	m_previewMsgWidget->setCursor(Qt::PointingHandCursor);
	m_previewMsgWidget->hide();
	connect(m_previewMsgWidget, &QPushButton::clicked, this, &QListView::scrollToBottom);
	connect(this->verticalScrollBar(), &QScrollBar::valueChanged, this, &IMMsgWidget_Recv::slot_viewScrollValueChanged);
	connect(this, &ChatWidget::s_loadMoreClicked, this, &IMMsgWidget_Recv::slot_loadMoreClicked);
	connect(this, &ChatWidget::s_openHistoryClicked, this, &IMMsgWidget_Recv::slot_btnHisMsgClicked);
	connect(this, &ChatWidget::s_resendClicked, this, &IMMsgWidget_Recv::slot_resendClicked);

	uninit();
}

IMMsgWidget_Recv::~IMMsgWidget_Recv()
{
	slot_closeUnreadTips();
}

bool IMMsgWidget_Recv::isValidSession()
{
	if (m_baseWidget == NULL || m_baseWidget->getType() == CRIM_Conversation_Undefine)
	{
		return false;
	}
	return true;
}

void IMMsgWidget_Recv::init(ConversationBaseWidget *w)
{
	if (m_baseWidget != NULL)
	{
		uninit();
	}
	m_baseWidget = w;
	if (!isValidSession())
		return;

	m_conversationInfo = m_baseWidget->getInfo();
	m_cookie = makeUUID().toUtf8();
	updateConnection(true);
	slot_loadMoreClicked();
	//如果当前有@提示，但是没有未读@消息，则使用历史的最后一条
	if ((m_baseWidget->getInfo().groupAtType == CRIM_GAT_AtAllAtMe || m_baseWidget->getInfo().groupAtType == CRIM_GAT_AtMe)
		&& m_unreadAtMsg.size() == 0)
	{
		m_unreadAtMsg.append(m_hisLastAtMsgId);
	}
	if (m_unreadAtMsg.size() > 0)//at提示
	{
		showUnreadTips(TIPS_AT, m_unreadAtMsg.size());
	}
	else if (m_baseWidget->getInfo().unreadCount > 15)//如果超过15条消息未读,最大MSG_MAX_COUNT条，则新消息提示
	{
		int unreadCount = m_baseWidget->getInfo().unreadCount;
		if (unreadCount > MSG_MAX_COUNT)
		{
			unreadCount = MSG_MAX_COUNT;
		}
		showUnreadTips(TIPS_UNREADMSG, unreadCount);
	}
	else
	{
		slot_closeUnreadTips();
	}
	//this->scrollToBottom();
}

void IMMsgWidget_Recv::uninit()
{
	updateConnection(false);
	for (auto &revokedMsg : m_revokedMsgs.values())
	{
		revokedMsg.revokeTimer->stop();
	}
	m_revokedMsgs.clear();
	m_baseWidget = NULL;
	m_firstHisMsg = CRIMMessage();
	m_cookie.clear();
	m_hisLastAtMsgId.clear();
	m_unreadAtMsg.clear();
	m_firstUnreadMsgId.clear();
	m_bLoading = false;
	this->clear();
}

void IMMsgWidget_Recv::updateConnection(bool bConnect)
{
	if (bConnect)
	{
		connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_getHistoryMsgListSuccess, this, &IMMsgWidget_Recv::slot_getHistoryMsgListSuccess, Qt::UniqueConnection);
		connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_getHistoryMsgListFailed, this, &IMMsgWidget_Recv::slot_getHistoryMsgListFailed, Qt::UniqueConnection);
		connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_sendMsgSuccess, this, &IMMsgWidget_Recv::slot_sendMsgSuccess, Qt::UniqueConnection);
		connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_sendMsgFailed, this, &IMMsgWidget_Recv::slot_sendMsgFailed, Qt::UniqueConnection);
		connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_sendMsgProgress, this, &IMMsgWidget_Recv::slot_sendMsgProgress, Qt::UniqueConnection);
		connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_recvNewMsgs, this, &IMMsgWidget_Recv::slot_recvNewMsgs, Qt::UniqueConnection);
		connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_recvMsgRevoked, this, &IMMsgWidget_Recv::slot_recvMsgRevoked, Qt::UniqueConnection);
		connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_revokeMsgSuccess, this, &IMMsgWidget_Recv::slot_revokeMsgSuccess, Qt::UniqueConnection);
		connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_msgDeleted, this, &IMMsgWidget_Recv::slot_msgDeleted, Qt::UniqueConnection);
		connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_markMsgAsReadByMsgIDSuccess, this, &IMMsgWidget_Recv::slot_markMsgAsReadByMsgIDSuccess, Qt::UniqueConnection);
		connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_recv1v1ReadReceipt, this, &IMMsgWidget_Recv::slot_recv1v1ReadReceipt, Qt::UniqueConnection);
		connect(AutoDelMessageMgr::getInstance(), &AutoDelMessageMgr::s_msgsDurationProgress, this, &IMMsgWidget_Recv::slot_msgsDurationProgress, Qt::UniqueConnection);
	}
	else
	{
		disconnect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_getHistoryMsgListSuccess, this, &IMMsgWidget_Recv::slot_getHistoryMsgListSuccess);
		disconnect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_getHistoryMsgListFailed, this, &IMMsgWidget_Recv::slot_getHistoryMsgListFailed);
		disconnect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_sendMsgSuccess, this, &IMMsgWidget_Recv::slot_sendMsgSuccess);
		disconnect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_sendMsgFailed, this, &IMMsgWidget_Recv::slot_sendMsgFailed);
		disconnect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_sendMsgProgress, this, &IMMsgWidget_Recv::slot_sendMsgProgress);
		disconnect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_recvNewMsgs, this, &IMMsgWidget_Recv::slot_recvNewMsgs);
		disconnect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_recvMsgRevoked, this, &IMMsgWidget_Recv::slot_recvMsgRevoked);
		disconnect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_revokeMsgSuccess, this, &IMMsgWidget_Recv::slot_revokeMsgSuccess);
		disconnect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_msgDeleted, this, &IMMsgWidget_Recv::slot_msgDeleted);
		disconnect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_markMsgAsReadByMsgIDSuccess, this, &IMMsgWidget_Recv::slot_markMsgAsReadByMsgIDSuccess);
		disconnect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_recv1v1ReadReceipt, this, &IMMsgWidget_Recv::slot_recv1v1ReadReceipt);
		disconnect(AutoDelMessageMgr::getInstance(), &AutoDelMessageMgr::s_msgsDurationProgress, this, &IMMsgWidget_Recv::slot_msgsDurationProgress);
	}
}

void IMMsgWidget_Recv::showUnreadTips(TIPS_TYPE type, int count)
{
	bool bVisible = false;
	while (1)
	{
		if (type == TIPS_AT)
		{
			QString txt;
			if (m_unreadAtMsg.size() == 1)
			{
				QString msgId = m_unreadAtMsg.at(0);
				UIMsgInfo info = this->getMsgInfo(msgId);
				if (info.clientMsgID.isEmpty())
				{
					break;
				}
				txt = info.getSenderName();
			}
			else if (m_unreadAtMsg.size() > 1)
			{
				txt = tr("%1人").arg(m_unreadAtMsg.size());
			}

			if (txt.isEmpty())
			{
				break;
			}
			txt += tr("提到你");
			uiUnreadTips.btnUnreadAt->setText(txt);
			uiUnreadTips.btnUnreadAt->show();
			uiUnreadTips.btnUnreadMsg->hide();
		}
		else
		{
			QString txt = tr("%1条新消息").arg(count);
			uiUnreadTips.btnUnreadMsg->setProperty("unreadCount", count);
			uiUnreadTips.btnUnreadMsg->setText(txt);
			uiUnreadTips.btnUnreadMsg->show();
			uiUnreadTips.btnUnreadAt->hide();
		}
		bVisible = true;
		break;
	}

	adjustDlgSize(m_unreadTipsWidget);
	m_unreadTipsWidget->move(this->width() - m_unreadTipsWidget->width(), 0);
	m_unreadTipsWidget->setVisible(bVisible);
}

bool IMMsgWidget_Recv::canRevoke(const UIMsgInfo &msg)
{
	//不是自己发送的，不能撤回
	if (msg.sendID != CRIMManager_Qt::instance()->getUserID())
	{
		return false;
	}
	//没有发送成功的，不能撤回
	if (msg.status != CRIM_MsgStatus_SendSuccess)
	{
		return false;
	}
	//超过2分钟的，不能撤回
	if (msg.getSendTime().secsTo(QDateTime::currentDateTime()) > 2 * 60)
	{
		return false;
	}

	//通知消息，不能撤回
	if (msg.isNotificationMsg()
		|| msg.contentType == CRIM_MsgType_Revoke 
		|| msg.contentType == CRIM_MsgType_1V1HasReadReceipt
		|| msg.contentType == CRIM_MsgType_GroupHasReadReceipt
		|| msg.contentType == CRIM_MsgType_Typing)
	{
		return false;
	}
	return true;
}

bool IMMsgWidget_Recv::isSessionMsg(const CRIMMessage &msg, AppendMsgType type)
{
	if (m_baseWidget == NULL || m_baseWidget->getType() == CRIM_Conversation_Undefine)
	{
		return false;
	}
	//本地插入的消息，不过滤
	if (type == LOC_MSG)
	{
		return true;
	}
	return IMMsgWidget::isSessionMsg(msg);
}

void IMMsgWidget_Recv::appendMsg(const QList<CRIMMessage> &msgs, AppendMsgType type)
{
	QList<UIMsgInfo> newMsgList;
	for (auto &msgInfo : msgs)
	{
		// 非本地插入的消息需要过滤
		if (!isSessionMsg(msgInfo, type))
		{
			continue;
		}
		//阅后即焚
		if (type == HIS_MSG)
		{
			//如果立即删除，则过滤消息
			if (burnMsg(msgInfo))
			{
				continue;
			}
		}
		newMsgList.append(msgInfo);
	}
	appendMsg_inner(newMsgList, msgs.size(), type);
}

void IMMsgWidget_Recv::appendMsg(const QList<UIMsgInfo> &msgs, AppendMsgType type)
{
	QList<UIMsgInfo> newMsgList;
	for (auto &msgInfo : msgs)
	{
		// 非本地插入的消息需要过滤
		if (!isSessionMsg(msgInfo, type))
		{
			continue;
		}
		//阅后即焚
		if (type == HIS_MSG)
		{
			//如果立即删除，则过滤消息
			if (burnMsg(msgInfo))
			{
				continue;
			}
		}
		newMsgList.append(msgInfo);
	}
	appendMsg_inner(newMsgList, msgs.size(), type);
}

void IMMsgWidget_Recv::appendMsg_inner(const QList<UIMsgInfo> &msgs, int orgMsgSize, AppendMsgType type)
{
	bool bFirstLoadHis = (type == HIS_MSG) && (this->getCount() == 0);
	int insertedCount = this->insertMsg(msgs, type == HIS_MSG);

	//更新可加载状态
	//数据满了，切换到不能加载更多数据的状态
	if (this->getCount() >= this->getMaxCount())
	{
		this->setDataState(ChatWidget::DS_FULL);
	}
	else if (type == HIS_MSG)//如果写入的是历史消息
	{
		//并且写入的数据小于加载数或者加载全部时，则没有更多的历史记录可以加载
		if (orgMsgSize < m_curLoadHisCount || m_curLoadHisCount == -1)
		{
			this->setDataState(ChatWidget::DS_NO_MOREDATA);
		}
		else//如果写入的数据等于加载数，则有更多的历史记录可以加载
		{
			this->setDataState(ChatWidget::DS_CANLOAD);
		}
	}
	
	//滚动条处理
	if (type == LOC_MSG)//本地消息
	{
		//强制滚动到底部
		this->scrollToBottom();
	}
	else if (type == HIS_MSG)//历史消息
	{
		//@我的消息，只保存最新的一条
		QString m_atMsgId;
		for (auto &uiMsg : msgs)
		{
			if (uiMsg.isAtMe())
			{
				m_atMsgId = uiMsg.getMsgID();
			}
			
		}
		if (m_hisLastAtMsgId.isEmpty())
		{
			m_hisLastAtMsgId = m_atMsgId;
		}
		//第一次加载滚动到最底部
		if (bFirstLoadHis)
		{
			this->scrollToBottom();
		}
	}
	else if (type == NEW_MSG)//新消息
	{
		for (auto &uiMsg : msgs)
		{
			//@我的消息
			if (uiMsg.isAtMe())
			{
				m_unreadAtMsg.append(uiMsg.getMsgID());
				//显示@提示
				if (this->isVisible())
				{
					showUnreadTips(TIPS_AT, m_unreadAtMsg.size());
				}
			}

			//记录第一条未读消息
			if (uiMsg.sendID != CRIMManager_Qt::instance()->getUserID())
			{
				if (!this->isVisible())
				{
					if (m_firstUnreadMsgId.isEmpty())
					{
						m_firstUnreadMsgId = uiMsg.getMsgID();
					}
				}
			}
			
			//图片，语音，视频，文件, 100MB以内自动下载
			bool bDownload = (uiMsg.getFileSize() <= 100*1024*1024); 
			if (bDownload)
			{
				downloadFile(uiMsg.clientMsgID);
			}
		}
		
		//滚动条没有在最后面，显示新消息提示
		if (this->verticalScrollBar()->value() != this->verticalScrollBar()->maximum() && msgs.size()>0 )
		{
			//加上省略符
			const UIMsgInfo &lastInfo = msgs.last();
			//正常消息才提示
			if (lastInfo.isNormalMsg())
			{
				QString previewText = lastInfo.getMsgPreviewText();
				previewText = lastInfo.getSenderName() + ":" + previewText;
				QFontMetrics fm(m_previewMsgWidget->font());
				previewText = fm.elidedText(previewText, Qt::ElideRight, m_previewMsgWidget->width());
				m_previewMsgWidget->setText(previewText);
				m_previewMsgWidget->show();
			}
		}
		else
		{
			//滚动到底部
			this->scrollToBottom();
		}
	}
	//更新当前页的消息状态
	updateCurPageMsgsState();
}

//加载历史消息
void IMMsgWidget_Recv::slot_getHistoryMsgListSuccess(const QList<CRIMMessage> &msgs, bool isEnd, const QString &cookie)
{
	QVariantMap params = JsonQStrToQVaraint(cookie).toMap();
	QString usrCookie = params["userCookie"].toString();
	QVariantMap ext = params["ext"].toMap();
	if (m_cookie != usrCookie)
	{
		return;
	}

	bool bJumpToFirst = ext["jumpToFirst"].toBool();
	m_bLoading = false;
	if (msgs.size() <= 0)
	{
		this->setDataState(ChatWidget::DS_NO_MOREDATA);
		return;
	}

	m_firstHisMsg = msgs.first();
	appendMsg(msgs, HIS_MSG);
	if (bJumpToFirst)
	{
		this->scrollToTop();
	}
}

void IMMsgWidget_Recv::slot_getHistoryMsgListFailed(int code, const QString &error, const QString &cookie)
{
	QVariantMap params = JsonQStrToQVaraint(cookie).toMap();
	QString usrCookie = params["userCookie"].toString();
	QVariantMap ext = params["ext"].toMap();
	if (m_cookie != usrCookie)
	{
		return;
	}

	m_bLoading = false;
	this->setDataState(ChatWidget::DS_NO_MOREDATA);
}

//收到新的消息
void IMMsgWidget_Recv::slot_recvNewMsgs(const QList<CRIMMessage> &msgs)
{
	appendMsg(msgs, NEW_MSG);
}

void IMMsgWidget_Recv::removeMsg_inner(const QString &msgId)
{
	//删除消息
	this->removeMsg(msgId);
	//取消阅后即焚
	AutoDelMessageMgr::getInstance()->cancelByMsgId(m_baseWidget->getConversationID(), msgId);
}

//消息撤回成功
void IMMsgWidget_Recv::slot_revokeMsgSuccess(const QString &conversationId, const QString &msgId, const QString &cookie)
{
	if (!isValidSession() || m_baseWidget->getConversationID() != conversationId)
	{
		return;
	}

	//不在ui中
	if (this->getMsgInfo(msgId).clientMsgID.isEmpty())
	{
		return;
	}
	UIMsgInfo info;
	info.fromJson(cookie);

	//不是我发送的消息
	if (info.sendID != CRIMManager_Qt::instance()->getUserID())
	{
		return;
	}
	//已经撤回
	if (m_revokedMsgs.contains(msgId))
	{
		return;
	}

	UIMsgInfo srcInfo = info;
	CRIMMsgType srcContentType = info.contentType;
	//更新msg类型为撤回
	info.updateMsgType(CRIM_MsgType_Revoke);

	RevokedMsgInfo_UI &revokeInfo = m_revokedMsgs[msgId];
	revokeInfo.srcMsgInfo = srcInfo;
	revokeInfo.revokeTimer = QSharedPointer<QTimer>(new QTimer);
	revokeInfo.revokeTimer->setSingleShot(true);
	revokeInfo.revokeTimer->setProperty("msgId", msgId);
	connect(revokeInfo.revokeTimer.data(), &QTimer::timeout, this, &IMMsgWidget_Recv::slot_revokeMsgTimeout);
	if (srcContentType == CRIM_MsgType_Text || srcContentType == CRIM_MsgType_At || srcContentType == CRIM_MsgType_Quote)
	{
		//添加重新编辑按钮，并且启动计时，3分钟后不能编辑
		QString reEditAnchor = QString("reEdit_%1").arg(msgId);
		info.setDisplayContent(info.getDisplayContent() + QString("<a href=\"%1\"><span style = \"text-decoration: none; color:#2384C8;\">  %2</span></a>").arg(reEditAnchor).arg(tr("重新编辑")));
		//3分钟后更新界面不能编辑
		revokeInfo.revokeTimer->start(3 * 60 * 1000);
	}

	//先删除后插入，保证提示在最后一条
	this->removeMsg_inner(msgId);
	this->insertMsg(info);
}

//消息被对方撤回
void IMMsgWidget_Recv::slot_recvMsgRevoked(const CRIMRevokedInfo &info)
{
	onMsgRevoked(info.clientMsgID);
}

//对方撤回消息
void IMMsgWidget_Recv::onMsgRevoked(const QString &msgId, const UIMsgInfo &revokeNotifyMsg)
{
	//界面上没有，则退出
	UIMsgInfo info = this->getMsgInfo(msgId);
	if (info.clientMsgID.isEmpty())
	{
		//如果是通知消息，则先写入一条数据
		if (!revokeNotifyMsg.clientMsgID.isEmpty())
		{
			appendMsg(QList<UIMsgInfo>() << revokeNotifyMsg, NEW_MSG);
		}
		return;
	}
	//已经在我撤回的列表中，则不处理，防止状态不对
	if (m_revokedMsgs.contains(msgId))
	{
		return;
	}
	//已经是撤回状态不处理
	if (info.contentType == CRIM_MsgType_Revoke)
	{
		return;
	}
	//更新消息类型为撤回
	info.updateMsgType(CRIM_MsgType_Revoke);
	//先删除后写入，保证数据在最后
	removeMsg_inner(msgId);
	this->insertMsg(info);
}

void IMMsgWidget_Recv::slot_revokeMsgTimeout()
{
	QTimer *revokeTimer = static_cast<QTimer*>(sender());
	QString msgId = revokeTimer->property("msgId").toString();
	auto findIt = m_revokedMsgs.find(msgId);
	if (findIt == m_revokedMsgs.end())
	{
		return;
	}

	//按照撤回消息类型重新更新一次内容，去除编辑按钮
	UIMsgInfo info = this->getMsgInfo(msgId);
	if (!info.clientMsgID.isEmpty())
	{
		info.updateMsgType(info.contentType);
		this->updateMsg(info);
	}
	m_revokedMsgs.erase(findIt);
}

//消息被删除
void IMMsgWidget_Recv::slot_msgDeleted(const CRIMMessage &msg)
{
	if (!IMMsgWidget::isSessionMsg(msg))
	{
		return;
	}
	removeMsg_inner(msg.clientMsgID);
}

//已读对方消息成功
void IMMsgWidget_Recv::slot_markMsgAsReadByMsgIDSuccess(const QString &conversationID, const QStringList &msgIDs, const QString &cookie)
{
	if (!isValidSession() || m_baseWidget->getConversationID() != conversationID)
	{
		return;
	}
	for (auto &msgId : msgIDs)
	{
		UIMsgInfo uiMsg = this->getMsgInfo(msgId);
		if (uiMsg.clientMsgID.isEmpty())
		{
			continue;
		}
		if (!uiMsg.isMyMsg())
		{
			//更新本地消息已读状态、消息已读时间
			uiMsg.isRead = true;
			uiMsg.attachedInfoElem.hasReadTime = CRIMManager_Qt::instance()->getCurrentSvrTime();
			this->updateMsg(uiMsg);

			//阅后即焚
			burnMsg(uiMsg);
		}
	}
}

//消息已读回执，我发送的消息被读
void IMMsgWidget_Recv::slot_recv1v1ReadReceipt(const QList<CRIMReadReceiptInfo> &list)
{
	if (!isValidSession() || m_baseWidget->getType() != CRIM_Conversation_1v1)
	{
		return;
	}

	for (auto &info : list)
	{
		if (info.sessionType != m_baseWidget->getType() || info.userID != m_baseWidget->getID())
		{
			continue;
		}
		for (auto &msgId : info.msgIDList)
		{
			UIMsgInfo msg = this->getMsgInfo(msgId);
			if (msg.clientMsgID.isEmpty())
			{
				continue;
			}
			//收到已读回执，更新消息已读状态、消息已读时间
			msg.isRead = true;
			msg.attachedInfoElem.hasReadTime = CRIMManager_Qt::instance()->getCurrentSvrTime();
			this->updateMsg(msg);

			//阅后即焚消息处理
			if (msg.attachedInfoElem.isPrivateChat)
			{
				burnMsg(msg);
			}
		}
	}
}

//发送消息成功
void IMMsgWidget_Recv::slot_sendMsgSuccess(const CRIMMessage &msg, const QString &cookie)
{
	//呼叫消息按照新消息处理，这里处理的是300（新呼叫），303（呼叫取消），304（呼叫挂断）
	if (msg.customElem.isInviteType())
	{
		appendMsg(QList<UIMsgInfo>() << msg, NEW_MSG);
		return;
	}
	if (!this->containsMsg(msg.clientMsgID))
	{
		return;
	}

	UIMsgInfo uiInfo = msg;
	this->updateMsg(uiInfo);
}

//发送消息失败
void IMMsgWidget_Recv::slot_sendMsgFailed(const QString &msgID, int code, const QString &error, const QString &cookie)
{
	UIMsgInfo info = this->getMsgInfo(msgID);
	if (info.clientMsgID.isEmpty())
	{
		return;
	}
	info.status = CRIM_MsgStatus_SendFailure;
	info.fileState = UFS_SEND_FAILED;
	this->updateMsg(info);
}

//发送消息进度
void IMMsgWidget_Recv::slot_sendMsgProgress(const QString &msgID, int progress, const QString &cookie)
{
	UIMsgInfo info = this->getMsgInfo(msgID);
	if (info.clientMsgID.isEmpty())
	{
		return;
	}
	info.status = CRIM_MsgStatus_Sending;
	info.fileState = UFS_SENDING;
	info.fileProgress = (float)progress / 100.0;
	this->updateMsg(info);
}

void IMMsgWidget_Recv::slot_viewScrollValueChanged(int value)
{
	if (!isValidSession())
	{
		return;
	}

	if (value == this->verticalScrollBar()->maximum())
	{
		m_previewMsgWidget->hide();
	}
	//如果当前显示的是未读消息提示，并且当前滚动到了第一条未读行，则隐藏提示
	if (m_unreadTipsWidget->isVisible() && uiUnreadTips.btnUnreadMsg->isVisible())
	{
		if (this->isMsgAfterCurrentShow(m_firstUnreadMsgId))
		{
			m_unreadTipsWidget->hide();
		}
	}
	//更新当前页的消息状态
	updateCurPageMsgsState();
}

void IMMsgWidget_Recv::showEvent(QShowEvent *e)
{
	IMMsgWidget::showEvent(e);
	updateCurPageMsgsState();
}

void IMMsgWidget_Recv::resizeEvent(QResizeEvent *e)
{
	IMMsgWidget::resizeEvent(e);
	int viewHeight = ScreenScale(25);
	m_previewMsgWidget->setGeometry(0, this->height() - viewHeight, this->width(), viewHeight);

	m_unreadTipsWidget->move(this->width() - m_unreadTipsWidget->width(), 0);
}

void IMMsgWidget_Recv::loadHisData(int loadCount, const QVariantMap &ext)
{
	if (m_bLoading)
	{
		return;
	}
	m_bLoading = true;
	this->setDataState(ChatWidget::DS_LOADING);
	m_curLoadHisCount = loadCount;
	
	QVariantMap params;
	params["ext"] = ext;
	params["userCookie"] = m_cookie;
	QByteArray jsonCookie = QJsonDocument::fromVariant(params).toJson();
	CRIMManager_Qt::instance()->getHistoryMsgList(m_baseWidget->getConversationID(), m_firstHisMsg.clientMsgID, m_curLoadHisCount, jsonCookie);
}

void IMMsgWidget_Recv::slot_loadMoreClicked()
{
	loadHisData(LOAD_HIS_COUNT);
}

void IMMsgWidget_Recv::slot_btnHisMsgClicked()
{

}

//重新发送
void IMMsgWidget_Recv::slot_resendClicked(const UIMsgInfo &msg)
{
	//不是好友不支持重新发送
	if (m_baseWidget->getType() == CRIM_Conversation_1v1 && !CRIMManager_Qt::instance()->isFriend(m_baseWidget->getInfo().userID))
	{
		return;
	}
	UIMsgInfo msgInfo = msg;
	msgInfo.status = CRIM_MsgStatus_Sending;
	msgInfo.fileState = UFS_SENDING;
	msgInfo.fileProgress = (float)0.0;
	this->updateMsg(msgInfo);

	QString recvUid = m_baseWidget->getType() == CRIM_Conversation_1v1 ? m_baseWidget->getInfo().userID : "";
	QString recvGid = m_baseWidget->getType() == CRIM_Conversation_Group ? m_baseWidget->getInfo().groupID : "";

	CRIMOfflinePushInfo pushInfo;
	if (m_baseWidget->getType() == CRIM_Conversation_1v1)
	{
		pushInfo.title = msg.senderNickname;
		pushInfo.desc =  msg.getMsgPreviewText();
	}
	else
	{
		pushInfo.title = m_baseWidget->getInfo().showName();
		pushInfo.desc = msg.senderNickname + ": " + msg.getMsgPreviewText();
	}
	CRIMManager_Qt::instance()->sendMsg(msg, recvUid, recvGid, pushInfo);
}

void IMMsgWidget_Recv::contentClickEvent(CursorPosType type, const QString &cursorText, const UIMsgInfo &msg)
{
	//重新编辑
	if (type == CURSOR_ANCHOR && cursorText.contains("reEdit_"))
	{
		QString msgId = msg.clientMsgID;
		auto findIt = m_revokedMsgs.find(msgId);
		if (findIt == m_revokedMsgs.end())
		{
			return;
		}
		emit s_reEdit(findIt.value().srcMsgInfo);
	}
	else
	{
		IMMsgWidget::contentClickEvent(type, cursorText, msg);
	}
}

void IMMsgWidget_Recv::onMenuExec(QMenu *menu, ChatWidget::MenuType menuType, const UIMsgInfo &msg)
{
	//右键头像
	if (menuType == ChatWidget::MENU_FACE)
	{
		//@功能
		if (m_baseWidget->getType() == CRIM_Conversation_Group && msg.sendID != CRIMManager_Qt::instance()->getUserID())
		{
			menu->addAction(g_atMgr.getActionById(GMEM_ACT_AT_USER, m_baseWidget->getID(), msg.sendID));
		}
	}
	else if (menuType == ChatWidget::MENU_CONTENT)//右键内容区域
	{
		if (canRevoke(msg))
		{
			QAction *act_revoke = new QAction(menu);
			connect(act_revoke, &QAction::triggered, this, &IMMsgWidget_Recv::slot_revokMessageClicked);
			act_revoke->setProperty("msgInfo", QVariant::fromValue(msg));
			act_revoke->setText(tr("撤回"));
			menu->addAction(act_revoke);
		}
	}
}

void IMMsgWidget_Recv::slot_revokMessageClicked()
{
	QAction *act_revoke = static_cast<QAction*>(sender());
	if (act_revoke == NULL)
	{
		return;
	}
	UIMsgInfo msg = act_revoke->property("msgInfo").value<UIMsgInfo>();
	CRIMManager_Qt::instance()->revokeMsg(m_baseWidget->getConversationID(), msg.getMsgID(), msg.toJson());
}

void IMMsgWidget_Recv::slot_btnUnreadAtClicked()
{
	QString msgId = m_unreadAtMsg.first();
	this->scrollToMsg(msgId);
	m_unreadAtMsg.removeFirst();
	if (m_unreadAtMsg.size() >0)
	{
		showUnreadTips(TIPS_AT, m_unreadAtMsg.size());
	}
	else
	{
		slot_closeUnreadTips();
	}
}

void IMMsgWidget_Recv::slot_btnUnreadMsgClicked()
{
	//如果已经加载的数据量不够，则需要加载全部的数据并跳转
	int unreadCount = uiUnreadTips.btnUnreadMsg->property("unreadCount").toInt();
	if (this->getCount() < unreadCount)
	{
		int needLoadDataCount = unreadCount;
		QVariantMap v;
		v["jumpToFirst"] = true;
		loadHisData(needLoadDataCount, v);
	}
	if (m_firstUnreadMsgId.isEmpty())
	{
		this->scrollToTop();
	}
	else
	{
		this->scrollToMsg(m_firstUnreadMsgId);
	}
	slot_closeUnreadTips();
}

void IMMsgWidget_Recv::slot_closeUnreadTips()
{
	m_hisLastAtMsgId.clear();
	m_unreadAtMsg.clear();
	m_firstUnreadMsgId.clear();
	m_unreadTipsWidget->hide();
}

void IMMsgWidget_Recv::updateCurPageMsgsState()
{
	const CRIMConversationInfo &conversationInfo = m_baseWidget->getInfo();
	QList<UIMsgInfo> curMsgs = this->getCurrentShowMsgs();
	QStringList markReadIds;
	QList<AutoDelMessageMgr::DelInfo> burnInfos;
	for (auto &uiMsg : curMsgs)
	{
		//标记其他的人的消息已读
		if (!uiMsg.isMyMsg(false) && uiMsg.isNormalMsg() && !uiMsg.hasRead() && uiMsg.getMsgState() == CRIM_MsgStatus_SendSuccess)
		{
			markReadIds.append(uiMsg.clientMsgID);
			//立即修改界面本地状态，防止反复标记已读
			uiMsg.isRead = true;
			this->updateMsg(uiMsg);
		}
	}
	//标记已读
	if (markReadIds.size() > 0)
	{
		CRIMManager_Qt::instance()->markMsgAsReadByMsgID(m_baseWidget->getConversationID(), markReadIds);
	}
}

bool IMMsgWidget_Recv::burnMsg(const UIMsgInfo &uiMsg)
{
	//只处理阅后即焚消息
	if (!uiMsg.attachedInfoElem.isPrivateChat)
	{
		return false;
	}
	//只处理文本、引用、at、图片消息
	if (uiMsg.contentType != CRIMSDK::CRIM_MsgType_Text
		&& uiMsg.contentType != CRIMSDK::CRIM_MsgType_Quote
		&& uiMsg.contentType != CRIMSDK::CRIM_MsgType_At
		&& uiMsg.contentType != CRIMSDK::CRIM_MsgType_Image)
	{
		return false;
	}
	//只处理已读的消息
	if (!uiMsg.hasRead())
	{
		return false;
	}
	//阅后即焚：由接收端标记已读成功后删除消息，当次没成功删除的消息，下次加载历史消息时继续删除
	AutoDelMessageMgr::DelInfo delInfo;
	delInfo.conversationID = m_baseWidget->getConversationID();
	delInfo.msgId = uiMsg.clientMsgID;

	//计算剩余时间
	int burnDuration = uiMsg.attachedInfoElem.burnDuration <= 0 ? DEFUALT_BURNDURATION : uiMsg.attachedInfoElem.burnDuration;
	burnDuration = burnDuration - (CRIMManager_Qt::instance()->getCurrentSvrTime() - uiMsg.attachedInfoElem.hasReadTime) / 1000;
	delInfo.duration = burnDuration > 0 ? burnDuration : 0;
	AutoDelMessageMgr::getInstance()->burnMsg(delInfo);

	//是否立即删除
	bool bInstantDel = burnDuration <= 0;
	return bInstantDel;
}

void IMMsgWidget_Recv::slot_msgsDurationProgress()
{
	if (!isValidSession())
	{
		return;
	}
	QList<AutoDelMessageMgr::DelInfo> lstDelInfo = AutoDelMessageMgr::getInstance()->getInfosByConversation(m_baseWidget->getConversationID());
	for (auto &delinfo : lstDelInfo)
	{
		UIMsgInfo msg = this->getMsgInfo(delinfo.msgId);
		if (msg.clientMsgID.isEmpty())
		{
			continue;
		}
		msg.countDown = delinfo.duration;
		this->updateMsg(msg);
	}
}
