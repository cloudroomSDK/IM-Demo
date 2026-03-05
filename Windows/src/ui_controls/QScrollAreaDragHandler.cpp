#include "stdafx.h"
#include "QScrollAreaDragHandler.h"

#define AUTO_SCROLL_DELAY		30
#define DRAG_CHCEK_DISTANCE		20
#define BUFF_DRAGPOS_COUNT		10

QScrollAreaDragHandler::QScrollAreaDragHandler(QObject *parent, QAbstractScrollArea *scrollArea) : QObject(parent)
{
	qRegisterMetaType<QScrollAreaDragHandler::DRAG_STATE>("QScrollAreaDragHandler::DRAG_STATE");

	QAbstractItemView *pItemView = dynamic_cast<QAbstractItemView*>(scrollArea);
	if ( pItemView!=NULL )
	{
		pItemView->setAutoScroll(false);
	}

	m_bDragEnable = true;
	m_bAutoScrollEnable = false;
	m_dragState = DRAG_NULL;
	m_scrollArea = scrollArea;
	m_bDragStarting = false;
	//m_scrollArea->installEventFilter(this);
	m_viewPort = scrollArea->viewport();
	m_viewPort->installEventFilter(this);
	connect(&m_autoScrollTimer, SIGNAL(timeout()), this, SLOT(ss_autoScrollTimeout()));
}

QScrollAreaDragHandler::~QScrollAreaDragHandler()
{
}

void QScrollAreaDragHandler::setState(DRAG_STATE newState)
{
	if ( newState==m_dragState )
		return;

	DRAG_STATE oldState = m_dragState;
	m_dragState = newState;

	//qDebug("DRAG_STATE:%d->%d", oldState, m_dragState);
	emit s_dragStateChanged(oldState, m_dragState);
}


void QScrollAreaDragHandler::monitorWidgetDragEvt(QWidget *pWidget)
{
	if ( pWidget==NULL )
		return;
	pWidget->installEventFilter(this);
}

void QScrollAreaDragHandler::unMonitorWidgetDragEvt(QWidget *pWidget)
{
	if ( pWidget==NULL )
		return;
	pWidget->removeEventFilter(this);
}


void QScrollAreaDragHandler::setDragEnable(bool bEnable)
{
	m_bDragEnable = bEnable;
}

void QScrollAreaDragHandler::setAutoScrollAfterDrag(bool bEnable)
{
	m_bAutoScrollEnable = bEnable;
}


bool QScrollAreaDragHandler::eventFilter(QObject *obj, QEvent *event)
{
	if ( obj==m_viewPort )
	{
		//为每个子窗口增加消息过滤
		if ( event->type() == QEvent::ChildAdded )
		{
			QChildEvent *pCEvt = static_cast<QChildEvent*>(event);
			if ( pCEvt->child()->isWidgetType() )
			{
				pCEvt->child()->installEventFilter(this);
			}
			return QObject::eventFilter(obj, event);
		}
	}

	switch(event->type())
	{
	case QEvent::MouseButtonPress:
		//qDebug("MouseButtonPress...");
		if ( m_bDragEnable )
		{
			m_lastMousePos = ((QMouseEvent*)event)->globalPos();
			m_dragInfo.clear();
			setState(DRAG_STARTING);
		}
		if ( m_dragState==DRAG_AUTOSCROLL_ING )
		{
			m_autoScrollTimer.stop();
			setState(DRAG_AUTOSCROLL_PAUSED);
			return true;
		}
		break;

	case QEvent::MouseMove:
		if ( !m_bDragEnable 
			|| m_dragState==DRAG_NULL
			|| m_bDragStarting //启动拖拽时，向目标控件发出的move消息，要透传给目标控件
			)
		{
			break;
		}
		else
		{
			QMouseEvent* pMsEvt = (QMouseEvent*)event;

			//检测是否达到启动滑动要求(DRAG_CHCEK_DISTANCE)
			QPoint newPos = pMsEvt->globalPos();
			QSize lastDragSize = QSize(newPos.x()-m_lastMousePos.x(), newPos.y()-m_lastMousePos.y());
			if (m_dragState==DRAG_STARTING || m_dragState == DRAG_AUTOSCROLL_PAUSED)
			{
				const int DRAG_CHECK_VALUE = DRAG_CHCEK_DISTANCE;
				if ( abs(lastDragSize.width())<=DRAG_CHECK_VALUE && abs(lastDragSize.height())<=DRAG_CHECK_VALUE )
					break;

				//达到拖拽要求后，伪造一个移出事件
				m_bDragStarting = true;
				QPoint glPos = QCursor::pos();
				QCursor::setPos(-1, -1);
 				QMouseEvent mvEvt(QEvent::MouseMove, QPoint(-1,-1), pMsEvt->button(), pMsEvt->buttons(), pMsEvt->modifiers());
 				QApplication::sendEvent(obj, &mvEvt);
				QCursor::setPos(glPos);
				m_bDragStarting = false;

				//进入拖拽状态
				setState(DRAG_ING);
				m_lastMousePos = newPos;
				break;
			}

			//滚动视图
			m_lastMousePos = newPos;
			scroll(lastDragSize.width(), lastDragSize.height());
			
			//记录最后n个位置信息
			if ( m_bAutoScrollEnable )
			{
				//qDebug("MouseMove time:%d, x:%d, y:%d", curTiem, newPos.x(), newPos.y());
				m_dragInfo.push_back(DragPos(QDateTime::currentMSecsSinceEpoch(), newPos));
				while ( m_dragInfo.size()>BUFF_DRAGPOS_COUNT )
				{
					m_dragInfo.pop_front();
				}
			}

			return true;
		}
		break;

	case QEvent::MouseButtonRelease:
		if ( m_dragState==DRAG_ING )
		{
			//允许自动滚动时，开启自动处理
			if ( m_bAutoScrollEnable )
			{
				m_dragInfo.push_back(DragPos(QDateTime::currentMSecsSinceEpoch(), ((QMouseEvent*)event)->globalPos()));
				startAutoScroll();
				setState(DRAG_AUTOSCROLL_ING);
				return true;
			}
        }

		//其它状态均进入拖拽结束状态
		{
			bool bDraging = (m_dragState>DRAG_STARTING);
			m_autoScrollTimer.stop();
			m_dragInfo.clear();
			setState(DRAG_NULL);
			if (bDraging)
			{
				return true;
			}
		}
		break;

	default: break;
	}

	return QObject::eventFilter(obj, event);
}

void QScrollAreaDragHandler::startAutoScroll()
{
	for(int i=0; i<m_dragInfo.size(); i++)
	{
		DragPos &oldPos = m_dragInfo[i];
		//qDebug("pos%d: time:%d, x:%d, y:%d", i, firstPos._time, firstPos._pos.x(), firstPos._pos.y());
	}

	int cx=0, cy=0;
	int duration = 0;
	if ( m_dragInfo.size()>=2 )
	{
		DragPos &lasterPos = m_dragInfo[m_dragInfo.size()-2];
		DragPos &lastestPos = m_dragInfo[m_dragInfo.size()-1];
		duration = int(lastestPos._time - lasterPos._time);
		if ( duration<300 )
		{
			DragPos &firstPos = m_dragInfo[0];
			cx = lastestPos._pos.x() - firstPos._pos.x();
			cy = lastestPos._pos.y() - firstPos._pos.y();
			duration = int(lastestPos._time - firstPos._time);
		}
	}

	if ( duration<=0 )
	{
		duration = 10;
	}

	m_dragInfo.clear();
	m_autoScrollSpeed = QSize(cx*1000/duration, cy*1000/duration);
	m_autoScrollTimer.start(AUTO_SCROLL_DELAY);

}


QSize QScrollAreaDragHandler::scroll(int x, int y)
{
	QSize realScroll(0,0);
	//qDebug("scroll...");
	bool bUpdateEnab = m_scrollArea->updatesEnabled();
	m_scrollArea->setUpdatesEnabled(false);
	QScrollBar *phScroll =  m_scrollArea->horizontalScrollBar();
	if(phScroll)
	{  
		int xLast = phScroll->value();
		phScroll->setValue(xLast - x); 
		realScroll.setWidth(phScroll->value()-xLast);
	}

	QScrollBar *pvScroll =  m_scrollArea->verticalScrollBar();
	if(pvScroll)
	{  
		int yLast = pvScroll->value();
		pvScroll->setValue(yLast -y);
		realScroll.setHeight(pvScroll->value()-yLast);
	}
	m_scrollArea->setUpdatesEnabled(bUpdateEnab);
    //m_scrollArea->repaint();

	return realScroll;
}

void QScrollAreaDragHandler::ss_autoScrollTimeout()
{
	if ( m_dragState!=DRAG_AUTOSCROLL_ING )
		return;

	//qDebug("ss_autoScrollTimeout...");

	//计算水平方向滚动值
	int xScroll = m_autoScrollSpeed.width() * AUTO_SCROLL_DELAY / 1000;
	int yScroll = m_autoScrollSpeed.height() * AUTO_SCROLL_DELAY / 1000;
	if ( xScroll==0 && yScroll==0 )
	{
		m_autoScrollTimer.stop();
		setState(DRAG_NULL);
		return;
	}

	//滚动
	QSize scrollVal = scroll(xScroll, yScroll);

	//已经滚不动了
	if ( scrollVal==QSize(0,0) )
	{
		m_autoScrollTimer.stop();
		setState(DRAG_NULL);
		return;
	}

	//速度衰减8%；
	m_autoScrollSpeed = QSize(m_autoScrollSpeed.width()*92/100, m_autoScrollSpeed.height()*92/100);
}
