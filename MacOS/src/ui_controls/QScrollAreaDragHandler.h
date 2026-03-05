#ifndef _QSCROLLAREADRAGHANDLER_H_
#define _QSCROLLAREADRAGHANDLER_H_


class QScrollAreaDragHandler: public QObject
{
	Q_OBJECT
public:
	QScrollAreaDragHandler(QObject *parent, QAbstractScrollArea *scrollArea);
	~QScrollAreaDragHandler();

	//是否支持拖拽功有(缺省true)
	void setDragEnable(bool bEnable);
	//是否拖拽结束时，自动滚动(缺省false)
	void setAutoScrollAfterDrag(bool bEnable);

	/*listWidget上itemWidget自动被监控，不用手工调用；
	  此接口为itemWidget上子窗口要实现拖动功能使用；
	*/
	void monitorWidgetDragEvt(QWidget *pWidget);
	void unMonitorWidgetDragEvt(QWidget *pWidget);

	enum DRAG_STATE 
	{ 
		DRAG_NULL,				//没有拖拽
		DRAG_STARTING,			//开始拖拽
		DRAG_ING,				//拖拽中
		DRAG_AUTOSCROLL_ING,	//拖拽后自动滚动中
		DRAG_AUTOSCROLL_PAUSED	//自动滚动时，被用户按住暂停中
	};
	DRAG_STATE getDragState() { return m_dragState; }

signals:
	void s_dragStateChanged(QScrollAreaDragHandler::DRAG_STATE oldST, QScrollAreaDragHandler::DRAG_STATE newST);

protected:
	virtual bool eventFilter(QObject *obj, QEvent *event);
	QSize scroll(int x, int y);
	void startAutoScroll();
	void setState(DRAG_STATE newState);

protected slots:
	void ss_autoScrollTimeout();

private:
	QAbstractScrollArea*	m_scrollArea;
	QWidget*				m_viewPort;
	bool			m_bDragEnable;
	bool			m_bAutoScrollEnable;

	bool			m_bDragChecking;
	bool			m_bDragStarting;
	DRAG_STATE		m_dragState;
	QWidget*		m_dragObj;

	struct DragPos
	{
		quint64	_time;
		QPoint	_pos;
		DragPos(quint64 time, const QPoint &pos)
		{
			_time = time;
			_pos = pos;
		}
	};
	QList<DragPos> m_dragInfo;	//以最后3个点做速度计算

	QPoint			m_lastMousePos;
	QSize			m_autoScrollSpeed;
	QTimer			m_autoScrollTimer;
};

#endif // _QSCROLLAREADRAGHANDLER_H_
