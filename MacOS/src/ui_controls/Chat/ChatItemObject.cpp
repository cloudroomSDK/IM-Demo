#include "stdafx.h"
#include "ChatItemObject.h"
#include "ChatModel.h"
#include "ChatWidget.h"
#include "ChatItemObject_Html.h"
#include "ChatItemObject_File.h"

#define DrawBox 0
#define CONTENT_RADIUS 5.0
#define FACE_SIZE	QSize(ScreenScale(30), ScreenScale(30))	//头像大小
#define STATE_SIZE	QSize(ScreenScale(100), ScreenScale(20))	//状态栏大小
#define PADDINGS	QMargins(10, 10, 10, 10) //整个item四边边距
#define SPACING_H 12		//item内部元素水平间距
#define SPACING_V 7			//item内部元素垂直间距

ChatItemObject *ChatItemObject::CreateItemObj(const UIMsgInfo &info, const ChatStyle &style)
{
	ChatItemObject::UIType type = ChatItemObject::getUIType(info);
	switch (type)
	{
	case ChatItemObject::UT_HTML: return new ChatItemObject_Html(style);
	case ChatItemObject::UT_FILE: return new ChatItemObject_File(style);
	default: return new ChatItemObject(style);
	}
}

ChatItemObject::UIType ChatItemObject::getUIType(const UIMsgInfo &info)
{
	switch (info.contentType)
	{
	case CRIM_MsgType_Text:
	case CRIM_MsgType_Image:
	case CRIM_MsgType_At:
	case CRIM_MsgType_Custom:
		return UT_HTML;
	case CRIM_MsgType_File: return UT_FILE;
	case CRIM_MsgType_Video: return UT_VIDEO;
	case CRIM_MsgType_Audio: return UT_AUDIO;
		//case CRIM_MsgType_Quote: return UT_QUOTE;

		//通知类消息
	case CRIM_MsgType_FriendAppApproved:             /// 同意加好友申请通知
	case CRIM_MsgType_FriendAppRejected:             /// 拒绝加好友申请通知
	case CRIM_MsgType_FriendReq:             /// 加好友通知
	case CRIM_MsgType_FriendAdded:                   /// 添加好友通知
	case CRIM_MsgType_FriendDeleted:                 /// 删除好友通知
	case CRIM_MsgType_FriendRemarkSet:               /// 设置好友备注通知
	case CRIM_MsgType_BlackAdded:                    /// 加黑名单通知
	case CRIM_MsgType_BlackDeleted:                  /// 移除黑名单通知
	case CRIM_MsgType_ConversationOptChange:         /// 会话免打扰设置通知
	case CRIM_MsgType_UserInfoUpdated:               /// 个人信息变更通知
	case CRIM_MsgType_OANotification:                /// oa 通知
	case CRIM_MsgType_GroupCreated:                  /// 群创建通知
	case CRIM_MsgType_GroupInfoSet:                  /// 更新群信息通知
	case CRIM_MsgType_JoinGroupReq:          /// 申请加群通知
	case CRIM_MsgType_MemberQuit:                    /// 群成员退出通知
	case CRIM_MsgType_GroupAppAccepted:              /// 同意加群申请通知
	case CRIM_MsgType_GroupAppRejected:              /// 拒绝加群申请通知
	case CRIM_MsgType_GroupOwnerTransferred:         /// 群主更换通知
	case CRIM_MsgType_MemberKicked:                  /// 群成员被踢通知
	case CRIM_MsgType_MemberInvited:                 /// 邀请群成员通知
	case CRIM_MsgType_MemberEnter:                   /// 群成员进群通知
	case CRIM_MsgType_DismissGroup:                  /// 解散群通知
	case CRIM_MsgType_GroupMemberMutedNotification:
	case CRIM_MsgType_GroupMemberCancelMutedNotification:
	case CRIM_MsgType_GroupMutedNotification:
	case CRIM_MsgType_GroupCancelMutedNotification:
	case CRIM_MsgType_GroupMemberInfoSetNotification:
	case CRIM_MsgType_GroupMemberSetToAdminNotification:
	case CRIM_MsgType_GroupMemberSetToOrdinaryUserNotification:
	case CRIM_MsgType_GroupAnnouncement:             /// 群公告
	case CRIM_MsgType_GroupSetNameNotification:      /// 修改群名称
	case CRIM_MsgType_SuperGroupUpdateNotification:
	case CRIM_MsgType_BurnAfterReadingNotification:
	case CRIM_MsgType_OrganizationChangedNotification:
	case CRIM_MsgType_Business:                      /// 业务通知
	case CRIM_MsgType_Revoke:                        /// 撤回消息,3.0版本的消息撤回类型，兼容单人撤回，群撤回，新增群主管理员撤回成员信息
	case CRIM_MsgType_1V1HasReadReceipt:                /// 单聊已读回执
	case CRIM_MsgType_GroupHasReadReceipt:           /// 群已读回执
		return UT_HTML;


	default:
		return UT_NULL;
	}
}

ChatItemObject::ChatItemObject(const ChatStyle &style, QObject *parent) : QObject(parent)
, m_style(style)
{
	m_bDragingOnClick = false;
	m_bShowTime = false;
	m_selectionStart = QPoint(0, 0);
	m_selectionEnd = QPoint(0, 0);
	m_bSelected = false;

	connect(m_style.getWaittingObj(), SIGNAL(frameChanged(int)), this, SLOT(slot_waittingAnimation(int)));

	updateStyle();
	setVisible(false);
}

ChatItemObject::~ChatItemObject()
{
}

//更新样式
void ChatItemObject::updateStyle()
{
	QFontMetrics fm(m_style.getFontStyle(ChatStyle::Medium));
	QFontMetrics fm2(m_style.getFontStyle(ChatStyle::Big));
	QFontMetrics fm3(m_style.getFontStyle(ChatStyle::Small));
	m_fontHeight_medium = fm.height();
	m_fontHeight_big = fm2.height();
	m_fontHeight_small = fm3.height();
}

void ChatItemObject::setMsgInfo(const UIMsgInfo &info)
{
	m_msgInfo = info;
	setVisible(m_bVisible);
}

void ChatItemObject::showTime(bool bShowTime)
{
	m_bShowTime = bShowTime;
	if (m_style.getViewMode() == ChatStyle::CHAT_LIST)
	{
		m_bShowTime = true;
	}
	setVisible(m_bVisible);
}

void ChatItemObject::slot_waittingAnimation(int)
{
	if (m_msgInfo.getMsgState() != CRIM_MsgStatus_Sending)
	{
		return;
	}
	update();
}

void ChatItemObject::setVisible(bool bVisible)
{
	m_bVisible = bVisible;
}

void ChatItemObject::update()
{
	if (m_bVisible)
	{
		emit s_needUpdate();
	}
}

void ChatItemObject::selectionStart(const QPoint &pos)
{
	m_selectionStart = pos;
	m_bSelected = updateSelection();
}

void ChatItemObject::selectionEnd(bool bDownDirection)
{
	checkSelectStart(bDownDirection);
	if (m_selectionStart.y() >= m_contentRt.top() && m_selectionStart.y() <= m_contentRt.bottom())
	{
		m_selectionEnd = bDownDirection ? m_contentRt.bottomRight() : m_contentRt.topLeft();
	}
	m_bSelected = updateSelection();
}

void ChatItemObject::selectionMove(const QPoint &pos, bool bDownDirection)
{
	checkSelectStart(bDownDirection);
	m_selectionEnd = pos;
	m_bSelected = updateSelection();
}

void ChatItemObject::selectionAll()
{
	m_selectionStart = m_contentRt.topLeft();
	m_selectionEnd = m_contentRt.bottomRight();
	m_bSelected = updateSelection();
}

void ChatItemObject::selectionClear()
{
	m_selectionStart = QPoint(0, 0);
	m_selectionEnd = QPoint(0, 0);
	m_bSelected = updateSelection();
}

bool ChatItemObject::hasSelection() const
{
	return m_bSelected;
}

void ChatItemObject::checkSelectStart(bool bDownDirection)
{
	if (m_selectionStart.isNull())
	{
		m_selectionStart = bDownDirection ? m_contentRt.topLeft() : m_contentRt.bottomRight();
	}
	else
	{
		if (bDownDirection && m_selectionStart == m_contentRt.bottomRight())
		{
			m_selectionStart = m_contentRt.topLeft();
		}
		else if (!bDownDirection && m_selectionStart == m_contentRt.topLeft())
		{
			m_selectionStart = m_contentRt.bottomRight();
		}
	}
}

bool ChatItemObject::updateSelection()
{
	if (m_selectionStart == m_selectionEnd)
	{
		return false;
	}
	if (m_selectionStart.isNull() || m_selectionEnd.isNull())
	{
		return false;
	}
	bool bDownDirection = m_selectionStart.y() <= m_selectionEnd.y();
	if (m_selectionStart.y() <= m_contentRt.top() || m_selectionStart.y() >= m_contentRt.bottom())
	{
		if (m_selectionStart.y() <= m_contentRt.top())
		{
			return m_selectionEnd.y() >= m_contentRt.top();
		}
		else
		{
			return m_selectionEnd.y() <= m_contentRt.bottom();
		}
	}
	else
	{
		if (bDownDirection)
		{
			return (m_selectionStart.x() < m_contentRt.left()) && (m_selectionEnd.y() >= m_contentRt.bottom());
		}
		else
		{
			return (m_selectionStart.x() > m_contentRt.right()) && (m_selectionEnd.y() <= m_contentRt.top());
		}
	}
	return false;
}

void ChatItemObject::onMouseMoveEvent(QMouseEvent *mouseEvent, QCursor &cursor)
{
	if (!m_faceRt.isNull() && m_faceRt.contains(mouseEvent->pos()))//头像区域
	{
		cursor = Qt::PointingHandCursor;
	}
	else if (!m_stateRt.isNull() && m_stateRt.contains(mouseEvent->pos()))//状态区域
	{
		if (m_msgInfo.getMsgState() == CRIM_MsgStatus_Sending)
		{
			QToolTip::showText(QCursor::pos(), tr("发送中"));
		}
		else if (m_msgInfo.getMsgState() == CRIM_MsgStatus_SendFailure)
		{
			cursor = Qt::PointingHandCursor;
			QToolTip::showText(QCursor::pos(), tr("点击重新发送"));
		}
	}
	else if (!m_contentRt.isNull() && m_contentRt.contains(mouseEvent->pos()))//内容区域
	{
		cursor = Qt::PointingHandCursor;
	}
}

void ChatItemObject::onMouseButtonEvent(QMouseEvent *mouseEvent)
{
	if (!m_faceRt.isNull() && m_faceRt.contains(mouseEvent->pos()))//点击头像区域
	{
		if (mouseEvent->button() == Qt::LeftButton && mouseEvent->type() == QEvent::MouseButtonRelease)//左键点击
		{
			mouseEvent->accept();
			emit s_faceClicked(true);
		}
		else if (mouseEvent->button() == Qt::RightButton && mouseEvent->type() == QEvent::MouseButtonRelease)//右键点击
		{
			mouseEvent->accept();
			emit s_faceClicked(false);
		}
	}
	else if (!m_stateRt.isNull() && m_stateRt.contains(mouseEvent->pos()))//点击状态区域
	{
		//发送失败，左键点击
		if (mouseEvent->button() == Qt::LeftButton && mouseEvent->type() == QEvent::MouseButtonRelease && m_msgInfo.getMsgState() == CRIM_MsgStatus_SendFailure)
		{
			mouseEvent->accept();
			emit s_resendClicked();
		}
	}
	else if (!m_contentRt.isNull() && m_contentRt.contains(mouseEvent->pos()))//点击内容区域
	{
		if (mouseEvent->button() == Qt::RightButton)//右键点击
		{
			if (mouseEvent->type() == QEvent::MouseButtonPress)
			{
				//鼠标按下时，有选中的话，则拦截该消息
				if (hasSelection())
				{
					mouseEvent->accept();
				}
			}
			else if (mouseEvent->type() == QEvent::MouseButtonRelease)
			{
				mouseEvent->accept();
			}
		}
		else if (mouseEvent->button() == Qt::LeftButton)//左键点击
		{
			if (mouseEvent->type() == QEvent::MouseButtonPress)
			{
				m_bDragingOnClick = false;
				m_lastClickPos = mouseEvent->pos();
			}
			else if (mouseEvent->type() == QEvent::MouseButtonRelease)
			{
				//如果有拖动，则不处理
				m_bDragingOnClick = (m_lastClickPos - mouseEvent->pos()).manhattanLength() >= QApplication::startDragDistance();
			}
		}
	}
	else//点击其他区域
	{
		if (mouseEvent->type() == QEvent::MouseButtonRelease &&
			(mouseEvent->button() == Qt::LeftButton || mouseEvent->button() == Qt::RightButton))
		{
			mouseEvent->accept();
			emit s_areaClicked(mouseEvent->button() == Qt::LeftButton);
		}
	}
}

void ChatItemObject::updateSize(int viewWidth)
{
// 	if (m_viewWidth == viewWidth)
// 	{
// 		return;
// 	}

	if (m_msgInfo.contentType == CRIM_MsgType_NULL)
	{
		m_size = QSize(viewWidth, m_msgInfo.defHeightForNull);
		return;
	}

	QMargins padding = getPadding();
	if (m_msgInfo.isNormalMsg())//普通消息
	{
		//文本区域宽度=总宽度-（头像宽度+状态栏 + 四边边距 + 内间距 + 内容区域边距）
		int contentMaxWidth = viewWidth;
		contentMaxWidth -= FACE_SIZE.width() + SPACING_H;//头像宽度
		if (m_style.getViewMode() == ChatStyle::CHAT_BUBBLE)
		{
			contentMaxWidth -= STATE_SIZE.width() + SPACING_H;//状态图标
		}
		else
		{
			contentMaxWidth -= 80;
		}
		contentMaxWidth -= padding.left() + padding.right();//四边边距
		contentMaxWidth -= SPACING_H;//内间距
		//更新内容区域大小
		updateContentSize(contentMaxWidth);
		//计算单个item所需高度
		int itemHeight = 0;
		itemHeight += padding.top();//上边距
		if (isShowName())
		{
			itemHeight += m_fontHeight_small + SPACING_V; //名字
		}
		itemHeight += getContentSize().height(); //内容
		itemHeight += padding.bottom();//下边距
		m_size = QSize(viewWidth, itemHeight);
	}
	else if (m_msgInfo.isSystemMsg())//系统消息，只有一行消息内容
	{
		int contentMaxWidth = viewWidth;
		contentMaxWidth -= padding.left() + padding.right();//四边边距
		updateContentSize(contentMaxWidth);
		m_size = QSize(viewWidth, getContentSize().height() + padding.top() + padding.bottom());
	}

	//时间区域
	if (m_bShowTime && m_style.getViewMode() == ChatStyle::CHAT_BUBBLE)
	{
		m_size += QSize(0, m_fontHeight_small);
	}
}

Qt::AlignmentFlag ChatItemObject::getHAlignment()
{
	//列表模式居左
	if (m_style.getViewMode() == ChatStyle::CHAT_LIST)  return Qt::AlignLeft;

	//系统消息居中
	if (m_msgInfo.isSystemMsg()) return Qt::AlignHCenter;

	//我的消息靠右
	return m_msgInfo.isMyMsg() ? Qt::AlignRight : Qt::AlignLeft;
}

QMargins ChatItemObject::getPadding()
{
	QMargins padding = PADDINGS;
	int exPadding = m_style.getViewMode() == ChatStyle::CHAT_BUBBLE?30:0;
	Qt::AlignmentFlag hAlignment = getHAlignment();
	if (hAlignment == Qt::AlignLeft)
	{
		padding.setRight(padding.right() + exPadding);
	}
	else if (hAlignment == Qt::AlignRight)
	{
		padding.setLeft(padding.left() + exPadding);
	}

	return padding;
}

bool ChatItemObject::isShowFace()
{
	//气泡模式才显示头像
	//return m_style.getViewMode() == ChatStyle::CHAT_BUBBLE;
	return true;
}

bool ChatItemObject::isShowName()
{
	//气泡模式下，群聊不显示自己的名字，单聊不显示自己和对方的名字
	if (m_style.getViewMode() == ChatStyle::CHAT_BUBBLE)
	{
		bool is1V1Msg = m_msgInfo.sessionType == CRIM_Conversation_1v1;
		if (is1V1Msg || m_msgInfo.isMyMsg())
		{
			return false;
		}
	}
	return true;
}

QString ChatItemObject::getTimeStr()
{
	QDateTime dt = m_msgInfo.getSendTime();
	QString timeStr = dt.toString("hh:mm:ss");
	if (dt.date() != QDate::currentDate())
	{
		timeStr = dt.toString("yyyy/MM/dd hh:mm:ss");
	}
	return timeStr;
}

void ChatItemObject::updateDrawRect(const QRect &rect)
{
	if (m_msgInfo.contentType == CRIM_MsgType_NULL)
	{
		return;
	}

	//是否显示头像
	bool bShowFace = isShowFace();
	//是否显示昵称
	bool bShowName = isShowName();

	QRect faceRt, nameRt, contentRt, timeRt, stateRt, stateCountDownRt;
	QRect drawRect = rect;
	//除去四边边距
	drawRect -= getPadding();
	//时间区域
	if (m_bShowTime && m_style.getViewMode() == ChatStyle::CHAT_BUBBLE)
	{
		//重新计算区域，使其居中显示
		timeRt = QRect(drawRect.left(), drawRect.top(), drawRect.width(), m_fontHeight_small);
		int timeMargin = qMax(timeRt.left(), m_size.width() - drawRect.right());
		timeRt.setLeft(timeMargin);
		timeRt.setRight(m_size.width() - timeMargin);

		drawRect.setTop(drawRect.top() + timeRt.height());
	}
	Qt::AlignmentFlag hAlignment = getHAlignment();
	if (m_msgInfo.isNormalMsg())//普通消息
	{
		if (hAlignment == Qt::AlignRight)//靠右
		{
			//头像
			if (bShowFace)
			{
				QSize faceSize = FACE_SIZE;
				faceRt = QRect(QPoint(drawRect.right() - faceSize.width(), drawRect.top()), faceSize);
			}
			//名称
			if (bShowName)
			{
				int nameRight = !faceRt.isNull() ? faceRt.left() - SPACING_H : drawRect.right();
				nameRt = QRect(drawRect.left(), drawRect.top(), nameRight - drawRect.left(), m_fontHeight_small);
			}
			//内容
			{
				int contentLeft = (!faceRt.isNull() ? faceRt.left() - SPACING_H : drawRect.right()) - getContentSize().width();
				int contentTop = (!nameRt.isNull() ? nameRt.bottom() + SPACING_V : drawRect.top());
				contentRt = QRect(contentLeft, contentTop, getContentSize().width(), getContentSize().height());
				stateRt = QRect(contentRt.left() - SPACING_H - STATE_SIZE.width(), contentRt.top() + (contentRt.height() - STATE_SIZE.height()) / 2, STATE_SIZE.width(), STATE_SIZE.height());
				
			}
		}
		else if (hAlignment == Qt::AlignLeft)//靠左
		{
			//头像
			if (bShowFace)
			{
				QSize faceSize = FACE_SIZE;
				faceRt = QRect(drawRect.topLeft(), faceSize);
			}
			//名称
			if (bShowName)
			{
				int nameLeft = !faceRt.isNull() ? faceRt.right() + SPACING_H : drawRect.left();
				nameRt = QRect(nameLeft, drawRect.top(), drawRect.right() - nameLeft, m_fontHeight_small);
				//列表模式的时间摆在右上角
				if (m_style.getViewMode() == ChatStyle::CHAT_LIST)
				{
					// 				QFontMetrics fm(m_style.getFontStyle(ChatStyle::Small));
					// 				QSize timeSize(fm.width(getTimeStr()), nameRt.height());
					QSize timeSize(150, nameRt.height());
					nameRt.setWidth(nameRt.width() - timeSize.width());
					timeRt = QRect(QPoint(nameRt.right(), nameRt.top()), timeSize);
				}
			}
			//内容
			{
				int contentLeft = !faceRt.isNull() ? faceRt.right() + SPACING_H : drawRect.left();
				int contentTop = (!nameRt.isNull() ? nameRt.bottom() + SPACING_V : drawRect.top());
				contentRt = QRect(contentLeft, contentTop, getContentSize().width(), getContentSize().height());
				stateRt = QRect(contentRt.right() + SPACING_H, contentRt.top() + (contentRt.height() - STATE_SIZE.height()) / 2, STATE_SIZE.width(), STATE_SIZE.height());
			}
		}
		//是否显示状态栏
		//列表模式，不显示
		if (m_style.getViewMode() == ChatStyle::CHAT_LIST)
		{
			stateRt = QRect();
		}
		//不是发送中、发送失败状态、没有倒计时，不显示
		if (m_msgInfo.getMsgState() != CRIM_MsgStatus_Sending && m_msgInfo.getMsgState() != CRIM_MsgStatus_SendFailure && m_msgInfo.countDown == -1)
		{
			stateRt = QRect();
		}
	}
	else if (m_msgInfo.isSystemMsg())//系统消息，只有一行消息内容
	{
		contentRt = QRect(drawRect.topLeft(), getContentSize());
		if (hAlignment == Qt::AlignRight)//靠右
		{
			contentRt.moveLeft(drawRect.right() - contentRt.width());
		}
		else if (hAlignment == Qt::AlignHCenter)//居中
		{
			contentRt.moveLeft(contentRt.x() + (drawRect.width() - contentRt.width()) / 2);
		}
	}
	
	m_faceRt = faceRt;
	m_nameRt = nameRt;
	m_contentRt = contentRt;
	m_stateRt = stateRt;
	m_timeRt = timeRt;
}

void ChatItemObject::draw(QPainter *p, const QRect &rect)
{
	if (m_msgInfo.contentType == CRIM_MsgType_NULL)
	{
		return;
	}

	p->setPen(Qt::NoPen);
#if DrawBox
	p->setPen(Qt::red);
	p->drawRect(rect);
#endif
	updateDrawRect(rect);
	if (m_faceRt.isNull() && m_nameRt.isNull() && m_contentRt.isNull())
	{
		return;
	}

	//列表模式
	if (m_style.getViewMode() == ChatStyle::CHAT_LIST)
	{
		QRect rt(m_nameRt.left(), rect.bottom(), m_timeRt.right() - m_nameRt.left(), 1);
		p->save(); 
		p->setBrush(m_style.getColorStyle(ChatStyle::SysMsgBkColor));
		p->drawRect(rt);
		p->restore();
	}

	//绘制时间
	if (!m_timeRt.isNull())
	{
		p->save();
		p->setFont(m_style.getFontStyle(ChatStyle::Small));
		p->setPen(m_style.getColorStyle(ChatStyle::TimeColor));
		QTextOption option(m_style.getViewMode() == ChatStyle::CHAT_LIST ? (Qt::AlignRight | Qt::AlignVCenter) : Qt::AlignCenter);
		p->drawText(m_timeRt, getTimeStr(), option);
#if DrawBox
		p->setPen(Qt::red);
		p->drawRect(m_timeRt);
#endif
		p->restore();
	}

	//绘制头像
	if (!m_faceRt.isNull())
	{
		p->save();
		p->drawPixmap(m_faceRt.topLeft(), m_msgInfo.getSenderFace(FACE_SIZE));
#if DrawBox
		p->setPen(Qt::red);
		p->drawRect(m_faceRt);
#endif
		p->restore();
	}

	//绘制昵称
	if (!m_nameRt.isNull())
	{
		p->save();
		p->setFont(m_style.getFontStyle(ChatStyle::Small));
		p->setPen(m_style.getColorStyle(ChatStyle::NameColor));
		QTextOption option(getHAlignment() | Qt::AlignVCenter);
		option.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
		p->drawText(m_nameRt, m_msgInfo.getSenderName(), option);
#if DrawBox
		p->setPen(Qt::red);
		p->drawRect(m_nameRt);
#endif
		p->restore();
	}

	//绘制消息内容
	if (!m_contentRt.isNull())
	{
		p->save();
		//绘制内容
#if DrawBox
		p->setPen(Qt::red);
		p->drawRect(m_contentRt);
#endif
		drawContent(p, m_contentRt);
		p->restore();
	}
	//绘制状态区域
	if (!m_stateRt.isNull())
	{
		p->save();

		int stateSpacing = SPACING_H;
		QPixmap sendStatePic;
 		if (m_msgInfo.getMsgState() == CRIM_MsgStatus_Sending)//发送中
 		{
 			sendStatePic = m_style.getWaittingObj()->currentPixmap();
 		}
 		else if (m_msgInfo.getMsgState() == CRIM_MsgStatus_SendFailure)//发送失败
		{
			sendStatePic = m_style.getSendFailedPic();
		}
		//靠右，从右往左绘制，绘制顺序倒计时、发送状态
		if (getHAlignment() == Qt::AlignRight)
		{
			QPoint drawPos(m_stateRt.topRight());
			//发送状态
			if (!sendStatePic.isNull())
			{
				drawPos.setX(drawPos.x() - sendStatePic.width());
				p->drawPixmap(drawPos, sendStatePic);
				drawPos.setX(drawPos.x() - stateSpacing);
			}
			//倒计时
			if (m_msgInfo.countDown >= 0)
			{
				QString drawTxt = QString::number(m_msgInfo.countDown);
				QRect drawTxtRt(m_stateRt.x(), m_stateRt.y(), drawPos.x() - m_stateRt.x(), m_stateRt.height());
				p->setFont(m_style.getFontStyle(ChatStyle::Small));
				p->setPen(m_style.getColorStyle(ChatStyle::StateColor2));
				QTextOption option(Qt::AlignRight | Qt::AlignVCenter);
				option.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
				p->drawText(drawTxtRt, drawTxt, option);
			}
		}
		else//靠左，从左往右绘制，绘制顺序倒计时、发送状态
		{
			QPoint drawPos(m_stateRt.topLeft());
			//发送状态
			if (!sendStatePic.isNull())
			{
				p->drawPixmap(drawPos, sendStatePic);
				drawPos.setX(drawPos.x() + sendStatePic.width()+ stateSpacing);
			}
			//倒计时
			if (m_msgInfo.countDown >= 0)
			{
				QString drawTxt = QString::number(m_msgInfo.countDown);
				QRect drawTxtRt(drawPos.x(), drawPos.y(), m_stateRt.right()-drawPos.x(), m_stateRt.height());
				p->setFont(m_style.getFontStyle(ChatStyle::Small));
				p->setPen(m_style.getColorStyle(ChatStyle::StateColor2));
				QTextOption option(Qt::AlignLeft | Qt::AlignVCenter);
				option.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
				p->drawText(drawTxtRt, drawTxt, option);
			}
		}


#if DrawBox
 		p->setPen(Qt::red);
 		p->drawRect(m_stateRt);
#endif
		p->restore();
	}
}

void ChatItemObject::drawContent(QPainter *p, const QRect &rect)
{
	p->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
	//圆角矩形边框设置
	if (m_style.getViewMode() == ChatStyle::CHAT_BUBBLE)
	{
		QPainterPath roundPath;
		roundPath.addRoundedRect(rect, CONTENT_RADIUS, CONTENT_RADIUS);
		p->setClipPath(roundPath);
	}
	//是否需要绘制气泡
	if (needDrawBubble())
	{
		//绘制气泡背景
		p->setPen(Qt::NoPen);
		p->setFont(m_style.getFontStyle(ChatStyle::Medium));
		QColor bkColor = m_style.getColorStyle(ChatStyle::ContentBkColor);
		if (getHAlignment() == Qt::AlignRight)
		{
			bkColor = m_style.getColorStyle(ChatStyle::ContentBkColor2);
		}
		else if (getMsgInfo().isSystemMsg())
		{
			bkColor = m_style.getColorStyle(ChatStyle::SysMsgBkColor);
		}
		p->setBrush(bkColor);
		// 		if (m_itemObj->getMsgInfo().isSystemMsg())
		// 		{
		// 			//系统消息需要左右扩展26的宽度
		// 			QRect rt = QRect(rect.topLeft() - QPoint(26, 0), rect.size() + QSize(26 * 2, 0));
		// 			//系统类消息需要自定义圆角矩形，清空矩形边框设置
		// 			//p->setClipPath(QPainterPath(), Qt::NoClip);
		// 			p->drawRect(rt);
		// 		}
		//		else
		{
			p->drawRect(rect/*, CONTENT_RADIUS, CONTENT_RADIUS*/);
		}
	}
}

bool ChatItemObject::needDrawBubble()
{
	if (m_style.getViewMode() == ChatStyle::CHAT_LIST)
	{
		return false;
	}
	return getMsgInfo().contentType == CRIM_MsgType_Text || getMsgInfo().contentType == CRIM_MsgType_At
		|| getMsgInfo().contentType == CRIM_MsgType_Revoke || getMsgInfo().contentType == CRIM_MsgType_Quote
		|| getMsgInfo().contentType == CRIM_MsgType_1V1HasReadReceipt || getMsgInfo().contentType == CRIM_MsgType_GroupHasReadReceipt
		|| getMsgInfo().contentType == CRIM_MsgType_Custom
		|| (getMsgInfo().isNotificationMsg());
}

void ChatItemObject::makeImageRoundBound(QImage &img, const QSize &contentSz)
{
	if (img.isNull())
		return;

	QImage rslt(img.size(), QImage::Format_ARGB32);
	rslt.fill(Qt::transparent);
	QPainter painter(&rslt);
	painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform, true);

	//配置裁剪区域
	QRectF rt(0.0, 0.0, img.width(), img.height());
	if (!contentSz.isEmpty())
	{
		rt.setSize(contentSz);
	}
	QPainterPath roundPath;
	roundPath.addRoundedRect(rt, CONTENT_RADIUS, CONTENT_RADIUS);
	painter.setClipPath(roundPath);

	//填充边缘为透明色
	painter.drawImage(0, 0, img);
	rslt.swap(img);
	return;
}

void ChatItemObject::drawRing(QPainter *painter, const QRect &rt, float progress)
{
	//绘制进度条
	painter->save();
	painter->setPen(QPen(QColor(81, 81, 81), 3, Qt::SolidLine, Qt::RoundCap));
	painter->drawEllipse(rt);
	painter->restore();

	int angleCurrent = -360.0 * progress;//顺时针
	int startAngle = 90 * 16;//从90度开始
	int spanAngle = angleCurrent * 16;
	painter->save();
	painter->setPen(QPen(Qt::white, 3, Qt::SolidLine, Qt::RoundCap));
	painter->drawArc(rt, startAngle, spanAngle);
	painter->restore();
}
