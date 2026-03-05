#include "stdafx.h"
#include "ChatWidget.h"
#include "ChatItemDelegate.h"
#include "qevent.h"
#include "QSmoothScrollbar.h"
#include "QSuspendedScrollBar.h"

WidgetLoadMore_Mgr::WidgetLoadMore_Mgr(QWidget *parent) : QWidget(parent)
{
	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->setSpacing(0);
	layout->setContentsMargins(0, 0, 0, 0);
	QSpacerItem *item1 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Fixed);
	QSpacerItem *item2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Fixed);
	btnLoad = new QPushButton(this);
	btnLoad->setObjectName("btnLoadMore");
	btnLoad->setFocusPolicy(Qt::NoFocus);
	btnLoad->setCursor(Qt::PointingHandCursor);
	btnLoad->setText(tr("点击加载更多"));
	btnOpenHistory = new QPushButton(this);
	btnOpenHistory->setObjectName("btnOpenHistory");
	btnOpenHistory->setFocusPolicy(Qt::NoFocus);
	btnOpenHistory->setCursor(Qt::PointingHandCursor);
	btnOpenHistory->setText(tr("打开历史记录"));
	lbStateDesc = new QLabel(this);
	lbStateDesc->setObjectName("lbStateDesc");
	int h = CONTENT_TOP_MARGIN;
	m_movie = new QMovie(":/Res/IM/waitting.gif");
	m_movie->setScaledSize(QSize(h/2, h/2));
	lbWaiting = new QLabel(this);
	lbWaiting->setFocusPolicy(Qt::NoFocus);
	lbWaiting->setMovie(m_movie);

	layout->addSpacerItem(item1);
	layout->addWidget(lbStateDesc);
	layout->addWidget(btnOpenHistory);
	layout->addWidget(btnLoad);
	layout->addWidget(lbWaiting);
	layout->addSpacerItem(item2);
	this->setFixedHeight(h);
	setState(ChatWidget::DS_CANLOAD);
	this->setStyleSheet(" \
WidgetLoadMore_Mgr QPushButton{ \
	background: transparent; \
	color: #12B7F5; \
	border:0px; \
	border-radius: 0px; \
} \
WidgetLoadMore_Mgr QPushButton:hover{ \
	text-decoration: underline; \
	background: transparent; \
}");
}

WidgetLoadMore_Mgr::~WidgetLoadMore_Mgr()
{
	if (m_movie != NULL)
	{
		m_movie->stop();
		m_movie->deleteLater();
		m_movie = NULL;
	}
}

void WidgetLoadMore_Mgr::setState(ChatWidget::DATA_STATE state)
{
	m_state = state;

	lbStateDesc->setVisible(/*state == ChatWidget::DS_FULL*/false);
	btnOpenHistory->setVisible(/*state == ChatWidget::DS_FULL*/false);
	btnLoad->setVisible(state == ChatWidget::DS_CANLOAD);
	lbWaiting->setVisible(state == ChatWidget::DS_LOADING);
	
	if (state == ChatWidget::DS_FULL)
	{
		lbStateDesc->setText(tr("更多记录请查看历史"));
	}
	
	if (state == ChatWidget::DS_LOADING)
	{
		m_movie->start();
	}
	else
	{
		m_movie->stop();
	}
}

ChatWidget::ChatWidget(QWidget *parent) : QListView(parent)
, m_model(NULL)
, m_loadMoreWidget(NULL)
{
	this->setSelectionMode(QAbstractItemView::NoSelection);
	this->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
	this->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
	//支持平滑滚动
	this->setVerticalScrollBar(new QSmoothScrollbar(parent));
	this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	this->setMouseTracking(true);
	this->setAutoScroll(false);
	//支持浮动显示
	QSuspendedScrollBar *suspendedScrollBar = new QSuspendedScrollBar(Qt::Vertical, this);

	m_maxCount = -1;
	m_selRowStart = -1;
	m_selStartPos = QPoint(-1, -1);
	m_loadMoreWidget = new WidgetLoadMore_Mgr(this->viewport());
	connect(m_loadMoreWidget->btnLoad, &QPushButton::clicked, this, &ChatWidget::s_loadMoreClicked);
	connect(m_loadMoreWidget->btnOpenHistory, &QPushButton::clicked, this, &ChatWidget::s_openHistoryClicked);
	m_timer.setSingleShot(true);
	connect(&m_timer, &QTimer::timeout, this, &ChatWidget::updateUI);
	m_model = new ChatModel(this);
	connect(m_model, &ChatModel::s_faceLeftClicked, this, &ChatWidget::slot_faceLeftClicked);
	connect(m_model, &ChatModel::s_faceRightClicked, this, &ChatWidget::slot_faceRightClicked);
	connect(m_model, &ChatModel::s_contentLeftClicked, this, &ChatWidget::slot_contentLeftClicked);
	connect(m_model, &ChatModel::s_contentRightClicked, this, &ChatWidget::slot_contentRightClicked);
	connect(m_model, &ChatModel::s_areaRightClicked, this, &ChatWidget::slot_areaRightClicked);
	connect(this->verticalScrollBar(), &QScrollBar::valueChanged, this, &ChatWidget::updateShowItems);

	this->setModel(m_model);
	this->setItemDelegate(new ChatItemDelegate);

	setDataState(DS_NO_MOREDATA);
//	setMaxCount(5000);
	this->verticalScrollBar()->installEventFilter(this);
}

ChatWidget::~ChatWidget()
{

}

void ChatWidget::setChatViewMode(ChatStyle::ChatViewMode mode)
{
	m_style.setViewMode(mode);
	//更新样式到内部元素
	m_model->updateStyle();
	//刷新界面
	updateUI();
}

void ChatWidget::setDataState(DATA_STATE state)
{
	DATA_STATE oldState = m_loadMoreWidget->getState();
	QSmoothScrollbar *bar = static_cast<QSmoothScrollbar*>(this->verticalScrollBar());
	if (bar != NULL)
	{
		//加载中不能滚动
		bar->enableScroll(state != DS_LOADING);
	}
	m_model->setTopMarginVisible(state != ChatWidget::DS_NO_MOREDATA);
	m_loadMoreWidget->setVisible(state != ChatWidget::DS_NO_MOREDATA);
	m_loadMoreWidget->setState(state);
	if (oldState == DS_LOADING &&state != DS_LOADING)
	{
		this->verticalScrollBar()->setValue(this->verticalScrollBar()->value() - m_loadMoreWidget->height());
	}
}

int ChatWidget::insertMsg(const UIMsgInfo &msg, bool insertFirst)
{
	QList<UIMsgInfo> lst;
	lst.append(msg);
	return insertMsg(lst, insertFirst);
}

int ChatWidget::insertMsg(const QList<UIMsgInfo> &msgs, bool insertFirst)
{
	QList<UIMsgInfo> newMsgs;
	for (auto &msgInfo : msgs)
	{
		//过滤已经写入的数据
		if (m_model->containsMsg(msgInfo.clientMsgID))
		{
			continue;
		}

		newMsgs.append(msgInfo);
	}
	if (newMsgs.size() == 0)
	{
		return newMsgs.size();
	}

	int maxCount = getMaxCount() == -1 ? (getCount() + newMsgs.count()) : getMaxCount();
	if (insertFirst)
	{
		QModelIndex index = indexAt(QPoint(0, this->verticalScrollBar()->value()));
		//如果是往前追加，并且超过最大数量，则只添加最后的部分数据
		int needAddCount = qMin(maxCount - getCount(), newMsgs.count());
		if (needAddCount > 0)
		{
			QList<UIMsgInfo> insertMsgs = newMsgs.mid(newMsgs.count() - needAddCount, needAddCount);
			m_model->insertMsg(insertMsgs, insertFirst);
			//滚动到之前显示位置
			index = index.sibling(index.row() + needAddCount + 1, index.column());
			scrollTo(index, PositionAtTop);
		}
	}
	else
	{
		//如果是往后追加，并且超过最大数量，则删除最前面几条数据
		if (newMsgs.count() > maxCount)
		{
			newMsgs = newMsgs.mid(newMsgs.count() - maxCount, maxCount);
		}
		else if (getCount() + newMsgs.count() > maxCount)
		{
			int needDelCount = getCount() + newMsgs.count() - maxCount;
			m_model->delFisrtItems(needDelCount);
		}
		m_model->insertMsg(newMsgs, insertFirst);
	}
	if (this->verticalScrollBar()->maximum() == 0 && this->verticalScrollBar()->minimum() == this->verticalScrollBar()->maximum())
	{
		//滚动条不会触发显示区域变化，需要手动触发显示区域
		updateShowItems();
	}
	return newMsgs.size();
}

void ChatWidget::removeMsg(const QString &msgId)
{
	m_model->removeMsg(msgId);
}

void ChatWidget::updateMsg(const UIMsgInfo &msg)
{
	m_model->updateMsg(msg);
}

const UIMsgInfo& ChatWidget::getMsgInfo(const QString &msgId) const
{
	return m_model->getMsgInfo(msgId);
}

const UIMsgInfo& ChatWidget::getMsgInfo(const QModelIndex &index) const
{
	return m_model->getMsgInfo(index.row());
}

const UIMsgInfo& ChatWidget::getFirstMsg() const
{
	return m_model->getMsgInfo(1);
}

const UIMsgInfo& ChatWidget::getLastMsg() const
{
	return m_model->getMsgInfo(m_model->rowCount() - 1);
}

bool ChatWidget::containsMsg(const QString &msgId) const
{
	return m_model->containsMsg(msgId);
}

void ChatWidget::clear()
{
	m_model->clear();
	setDataState(DS_NO_MOREDATA);
}

void ChatWidget::updateResource(const QString &msgId, const QUrl &resUrl)
{
	m_model->updateResource(msgId, resUrl);
}

void ChatWidget::scrollToMsg(const QString &msgId)
{
	QModelIndex index = m_model->getIndex(msgId);
	if (index.row() < 0)
	{
		return;
	}
	scrollTo(index, PositionAtTop);
}

bool ChatWidget::isMsgAfterCurrentShow(const QString &msgId)
{
	QModelIndex index = m_model->getIndex(msgId);
	if (index.row() < 0)
	{
		return false;
	}
	int srtartIndex, endIndex;
	m_model->getItemsShow(srtartIndex, endIndex);
	if (srtartIndex < 0)
	{
		return false;
	}
	return index.row() >= srtartIndex;
}

QList<UIMsgInfo> ChatWidget::getCurrentShowMsgs()
{
	QList<UIMsgInfo> rlst;
	int srtartIndex, endIndex;
	m_model->getItemsShow(srtartIndex, endIndex);
	if (srtartIndex < 0 || endIndex < 0)
	{
		return rlst;
	}
	for (int i = srtartIndex; i <= endIndex; i++)
	{
		rlst.append(m_model->getMsgInfo(i));
	}
	return rlst;
}

void ChatWidget::mousePressEvent(QMouseEvent *event)
{
	event->ignore();
	QListView::mousePressEvent(event);
	if (event->button() == Qt::LeftButton)
	{
		onLeftButtonClicked(event, false);
	}
	else if (event->button() == Qt::RightButton)
	{
		//如果消息被处理，则忽略
		if (event->isAccepted())
		{
			return;
		}
		clearSelection();
	}
	event->accept();
}

void ChatWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
	QListView::mouseDoubleClickEvent(event);
	if (event->button() == Qt::LeftButton)
	{
		onLeftButtonClicked(event, true);
	}
}

void ChatWidget::onLeftButtonClicked(QMouseEvent *event, bool bDoubleClick)
{
	clearSelection();
	m_selRowStart = getRowForPos(event->pos());
// 	if (m_selRowStart < 0)
// 	{
// 		return;
// 	}

	m_selStartPos = event->pos();
	if (bDoubleClick)
	{
		m_model->selectionAll(m_selRowStart);
		m_curSelItem.push_back(m_selRowStart);
	}
}

void ChatWidget::mouseMoveEvent(QMouseEvent *event)
{
	QListView::mouseMoveEvent(event);
	if ((event->buttons() & Qt::LeftButton) != Qt::LeftButton)
	{
		return;
	}
	if (m_model->rowCount() <= 0)
	{
		return;
	}
	if (m_selStartPos.x() < 0 || m_selStartPos.y() < 0)
	{
		return;
	}
	QPoint curPos = event->pos();
	if ((m_selStartPos - curPos).manhattanLength() <= QApplication::startDragDistance())
	{
		return;
	}

	int selRowStart = m_selRowStart;
	int selRowEnd = getRowForPos(event->pos());
	if (selRowStart < 0 && selRowEnd < 0)
	{
		return;
	}
	bool bDownDirection = m_selStartPos.y() < curPos.y();
	if (selRowEnd < 0)
	{
		selRowEnd = bDownDirection ? m_model->getLastShow() : m_model->getFisrtShow();
	}
	if (selRowStart < 0)
	{
		selRowStart = bDownDirection ? m_model->getFisrtShow() : m_model->getLastShow();
	}
	QList<int> newSelItems;
	for (int rowIndex = qMin(selRowStart, selRowEnd); rowIndex <= qMax(selRowStart, selRowEnd); rowIndex++)
	{
		if (bDownDirection)
		{
			newSelItems.push_back(rowIndex);
		}
		else
		{
			newSelItems.push_front(rowIndex);
		}
	}
	//如果已经有选中的项，则需要查找需要清空选中的项
	if (m_curSelItem.size() > 0)
	{
		for (auto rowIndex : m_curSelItem)
		{
			if (!newSelItems.contains(rowIndex))
			{
				m_model->selectionClear(rowIndex);
			}
		}
	}
	else
	{
		//如果没有选中的项，则设置第一项的起始位置
		m_model->selectionStart(newSelItems[0], m_selStartPos);
	}

	//处理正在选中的项
	m_model->selectionMove(selRowEnd, curPos, bDownDirection);
	//如果有多个选中项，处理第一项选中结束位置和全选中间的项
	if (newSelItems.count() >= 2)
	{
		m_model->selectionEnd(newSelItems[0], bDownDirection);

		for (int i = 1; i < newSelItems.count() - 1; i++)
		{
			m_model->selectionAll(newSelItems[i]);
		}
	}
	m_curSelItem = newSelItems;
}

void ChatWidget::resizeEvent(QResizeEvent *e)
{
	QListView::resizeEvent(e);
	//m_loadMoreWidget->setGeometry(0,0, this->width(), CONTENT_TOP_MARGIN);
	m_timer.start(100);
}

void ChatWidget::showEvent(QShowEvent *e)
{
	QListView::showEvent(e);
	updateShowItems();
}

void ChatWidget::hideEvent(QHideEvent *e)
{
	QListView::hideEvent(e);
	updateShowItems();
}

//view大小刷新时
void ChatWidget::updateGeometries()
{
	bool oldScrollBottom = this->verticalScrollBar()->value() == this->verticalScrollBar()->maximum();
	QListView::updateGeometries();
	bool newScrollBottom = this->verticalScrollBar()->value() == this->verticalScrollBar()->maximum();
	//如果大小改变前滚动条在底部，大小改变后不在底部，需要强制跳转到底部（主要是为了解决在隐藏状态下，滚动条状态不会自动在底部的情况）
// 	if (oldScrollBottom && !newScrollBottom)
// 	{
// 		this->verticalScrollBar()->setValue(this->verticalScrollBar()->maximum());
// 	}
	//当大小改变时，需要调整加载控件的位置（主要是为了解决在隐藏状态下，第一项y坐标会改变，不是0的位置）
	if (m_loadMoreWidget != NULL && m_model != NULL)
	{
		QModelIndex index = m_model->getIndex(0);
		if (index.isValid())
		{
			QRect rt = this->visualRect(index);
			if (!rt.isNull())
			{
				m_loadMoreWidget->setGeometry(0, rt.y(), this->width(), CONTENT_TOP_MARGIN);
			}
		}
	}
}

void ChatWidget::updateUI()
{
	m_model->updateAllSizeHint();
	updateShowItems();
}

void ChatWidget::updateShowItems()
{
	//显示当前视图下的item，（当前ui显示并且父窗口没有最小化）
	if (this->isVisible() && !this->topLevelWidget()->isMinimized())
	{
		int startIndex = indexAt(QPoint(0, 0)).row();
		int endIndex = indexAt(QPoint(0, this->rect().height())).row();
		if (startIndex <= 0)
		{
			startIndex = 0;
		}
		else
		{
			startIndex -= 1;
		}
		if (endIndex == -1 || endIndex >= m_model->rowCount() - 1)
		{
			endIndex = m_model->rowCount() - 1;
		}
		else
		{
			endIndex += 1;
		}
		if (startIndex != -1 && endIndex != -1)
		{
			m_model->setItemsShow(startIndex, endIndex);
		}
	}
	else
	{
		m_model->setItemsShow(-1, -1);
	}
}

void ChatWidget::clearSelection()
{
	for (auto rowIndex : m_curSelItem)
	{
		m_model->selectionClear(rowIndex);
		//item->selectionClear();
	}
	m_curSelItem.clear();
	m_selRowStart = -1;
	m_selStartPos = QPoint(-1, -1);
}

int ChatWidget::getRowForPos(const QPoint &pos)
{
	QModelIndex index = this->indexAt(pos);
	return index.row(); 
}

void ChatWidget::enterEvent(QEvent *event)
{
	QListView::enterEvent(event);
}

void ChatWidget::leaveEvent(QEvent *event)
{
	QListView::leaveEvent(event);
}

bool ChatWidget::eventFilter(QObject *obj, QEvent *ev)
{
	bool ret = QListView::eventFilter(obj, ev);
	if (obj == this->verticalScrollBar() && ev->type() == QEvent::Wheel)
	{
		if (this->verticalScrollBar()->value() == 0)
		{
			static QTime lastWheelEvent;
			static int wheelEventStep;
			//鼠标滚动
			QWheelEvent *wheelEvent = dynamic_cast<QWheelEvent *>(ev);
			wheelEventStep += wheelEvent->delta() / 8 / 15;//鼠标滚动的步数
			//超过时间范围,重置状态
			if (lastWheelEvent.elapsed() <= 0 || lastWheelEvent.elapsed() > 1000)
			{
				wheelEventStep = 0;
				lastWheelEvent.start();
				return false;
			}
			else//1秒钟内的滚动
			{
				//6步才加载数据
				if (wheelEventStep >= 5 && (m_loadMoreWidget->getState() == DS_CANLOAD) && m_loadMoreWidget->isVisible())
				{
					emit s_loadMoreClicked();
				}
			}
		}
	}
	return ret;
}

void ChatWidget::slot_faceLeftClicked(const UIMsgInfo &msg)
{
	emit s_faceClicked(msg);
}

void ChatWidget::slot_faceRightClicked(const UIMsgInfo &msg)
{
	emit s_rightClicked(MENU_FACE, msg);
}

void ChatWidget::slot_contentLeftClicked(int row, const UIMsgInfo &info, const QVariantMap &exParams)
{
	CursorPosType cursorType = (CursorPosType)exParams["type"].toInt();
	QString cursorText = exParams["cursorText"].toString();
	emit s_contentClicked(cursorType, cursorText, info);
}

void ChatWidget::slot_contentRightClicked(int row, const UIMsgInfo &info, const QVariantMap &exParams)
{
	bool selContent = exParams.value("selContent").toBool();
	if (selContent)
	{
		clearSelection();
		m_curSelItem.append(row);
		m_model->selectionAll(row);
	}
	emit s_rightClicked(MENU_CONTENT, info, exParams);
}

void ChatWidget::slot_areaRightClicked()
{
	emit s_rightClicked(MENU_OTHERAREA);
}

void ChatWidget::copy(const QStringList &msgIDs)
{
	QList<int> selItems;
	for (auto &msgId : msgIDs)
	{
		QModelIndex index = m_model->getIndex(msgId);
		if (index.row() < 0)
		{
			continue;
		}
		selItems.append(index.row());
	}
	m_model->copyItem(selItems);
}

void ChatWidget::copyCurSelect()
{
	m_model->copyItem(m_curSelItem);
}
