#include "stdafx.h"
#include "QFindEdit.h"

QFindEdit::QFindEdit(QWidget *parent)
	: CRLineEdit(parent)
, m_bEditFocus(true)
, m_bHaveResult(false)
{
	m_stringListmodel = new QStringListModel(this);
	m_viewFrame = new QFrame(this, Qt::Popup);
	m_viewFrame->hide();
	m_viewFrame->setObjectName("ViewFrame");
	QVBoxLayout *lay = new QVBoxLayout(m_viewFrame);
	lay->setContentsMargins(0,0,0,0);
	m_viewFrame->setFocusPolicy(Qt::NoFocus);
	m_viewFrame->setMinimumWidth(50);

	m_pListWnd = new QListView(this);
	m_pListWnd->setObjectName("FindListWnd");
	m_pListWnd->setEditTriggers(QAbstractItemView::NoEditTriggers);
	m_pListWnd->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_pListWnd->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_pListWnd->setSelectionMode(QAbstractItemView::SingleSelection);
	lay->addWidget(m_pListWnd);
	connect(this, SIGNAL(textEdited(const QString&)), this, SLOT(textEditedSlot(const QString&)));
	QObject::connect(m_pListWnd, SIGNAL(clicked(QModelIndex)), this, SLOT(clickedSlot(QModelIndex)));

	this->installEventFilter(this);
	m_pListWnd->installEventFilter(this);
}

QFindEdit::~QFindEdit()
{
}

QStringList& QFindEdit::stringList()
{
	return m_stringList;
}

void QFindEdit::showFindWnd(const QString& text)
{
	//效率较低,需要优化
	QStringList sl;  
	foreach(const QString &word, m_stringList)
	{  
		if (word.contains(text, Qt::CaseInsensitive))
		{  
			sl << word; 
		}  
	}

	m_bHaveResult = (sl.size() > 0);
	if (!m_bHaveResult) 
	{
		//没有找到
		sl << tr("无搜索结果...");
	}
	m_stringListmodel->setStringList(sl);  
	m_pListWnd->setModel(m_stringListmodel);  

	showPopup(rect());
	if (m_bHaveResult)
	{
		//默认高亮选中第一个
		QModelIndex index = m_pListWnd->model()->index(0, 0);
		m_pListWnd->setCurrentIndex(index);
	} 	
}

void QFindEdit::showPopup(const QRect& rect)
{
	const QRect screen = QApplication::desktop()->availableGeometry(this);
	Qt::LayoutDirection dir = this->layoutDirection();
	QPoint pos;
	int rh, w;
	int h = (m_pListWnd->sizeHintForRow(0) * qMin(15, m_pListWnd->model()->rowCount()) + 3) + 3; //最多显示15条
	QScrollBar *hsb = m_pListWnd->horizontalScrollBar();
	if (hsb && hsb->isVisible())
		h += m_pListWnd->horizontalScrollBar()->sizeHint().height();

	if (rect.isValid()) 
	{
		rh = rect.height();
		w = rect.width();
		pos = this->mapToGlobal(dir == Qt::RightToLeft ? rect.bottomRight() : rect.bottomLeft());
	} 
	else 
	{
		rh = this->height();
		pos = this->mapToGlobal(QPoint(0, this->height() - 2));
		w = this->width();
	}

	if (w > screen.width())
		w = screen.width();
	if ((pos.x() + w) > (screen.x() + screen.width()))
		pos.setX(screen.x() + screen.width() - w);
	if (pos.x() < screen.x())
		pos.setX(screen.x());

	int top = pos.y() - rh - screen.top() + 2;
	int bottom = screen.bottom() - pos.y();
	h = qMax(h, m_pListWnd->minimumHeight());
	if (h > bottom) {
		h = qMin(qMax(top, bottom), h);

		if (top > bottom)
			pos.setY(pos.y() - h - rh + 2);
	}
	m_viewFrame->setGeometry(pos.x(), pos.y(), w, h);
	//m_pListWnd->setGeometry(pos.x(), pos.y(), w, h);

	if (!m_viewFrame->isVisible())
	{
		m_viewFrame->show();
	}
	m_pListWnd->setFocus();
	//QTimer::singleShot(100, this, SLOT(delayFocusSet()));
}

void QFindEdit::delayFocusSet()
{
	this->activateWindow();
	this->setFocus();
}


void QFindEdit::textEditedSlot(const QString& text)
{
	QString strText = text.trimmed();
	if (!strText.isEmpty())
	{
		showFindWnd(strText);
	}
	else
	{
		hideFindWnd();
	}
}

void QFindEdit::clickedSlot(QModelIndex modelIndex)
{
	if (m_bHaveResult)
	{
		setText(m_pListWnd->model()->data(modelIndex).toString());
	}
	findFinished();
}

void QFindEdit::findFinished()
{
	hideFindWnd();
	emit s_findFinished(text());
}

void QFindEdit::hideFindWnd()
{
	m_viewFrame->hide();
	m_bHaveResult = false;
}

bool QFindEdit::eventFilter(QObject *o, QEvent *e)
{
	if ( o == this )
	{
		QEvent::Type evt = e->type();
		if ( evt==QEvent::KeyPress )
		{
			QKeyEvent *ke = static_cast<QKeyEvent *>(e);
			const int key = ke->key();

			switch(key)
			{
			case Qt::Key_Return:
			case Qt::Key_Enter:
				{
					//如果有默认项
					if (m_bHaveResult)
					{
						QModelIndex curIndex = m_pListWnd->currentIndex();
						setText(m_pListWnd->model()->data(curIndex).toString());
					}
					findFinished();
					return true;
				}
			case Qt::Key_Escape:
				{
					if ( m_viewFrame->isVisible() )
					{
						hideFindWnd();
						return true;
					}
					break;
				}
			}
		}//QEvent::KeyPress:
	}
 	else if ( o==m_pListWnd )
 	{
 		QEvent::Type evt = e->type();
 		if ( evt==QEvent::KeyPress || evt==QEvent::KeyRelease )
 		{
 			QKeyEvent *ke = static_cast<QKeyEvent *>(e);
 			const int key = ke->key();
 			if ( key!=Qt::Key_Down && key!=Qt::Key_Up && key!=Qt::Key_PageDown && key!=Qt::Key_PageUp)
 			{
 				QApplication::sendEvent(this, e);
 				return true;
 			}
 		}
 	}
	return false;
}
