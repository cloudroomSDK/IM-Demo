#include "stdafx.h"
#include "QSuspendedScrollBar.h"

#include <qevent.h>
#include <qqueue.h>
#include <qdatetime.h>
#include <qapplication.h>
#include <qmath.h>

//悬浮显示
QSuspendedScrollBar::QSuspendedScrollBar(Qt::Orientation t, QAbstractScrollArea* parent)
	: QScrollBar(parent)
{
	this->setObjectName("suspendedScrollBar");
	m_area = parent;
	this->setOrientation(t);//设置水平或者垂直滚动条
	this->setRange(0, 0);
	this->hide();

 	connect(parent->verticalScrollBar(), &QScrollBar::valueChanged, this, &QSuspendedScrollBar::slot_valueChange_scrollBar);
 	connect(parent->verticalScrollBar(), &QScrollBar::rangeChanged, this, &QSuspendedScrollBar::slot_rangeChanged);
 	connect(this, &QSuspendedScrollBar::valueChanged, parent->verticalScrollBar(), &QScrollBar::setValue);
	m_area->installEventFilter(this);
}

bool QSuspendedScrollBar::eventFilter(QObject *watched, QEvent *event)
{
	bool ret = QScrollBar::eventFilter(watched, event);
	if (watched == m_area)
	{
		QAbstractScrollArea* scrollArea = m_area;
		if (event->type() == QEvent::Resize)
		{
			int iX = scrollArea->width() - 8;
			this->setGeometry(iX, 1, 8, scrollArea->height() - 2);
		}
		else if (event->type() == QEvent::Enter)
		{
			if (this->maximum() > 0)
			{
				this->show();
			}
		}
		else if (event->type() == QEvent::Leave)
		{
			this->hide();
		}
	}
	return ret; 
}

void QSuspendedScrollBar::slot_valueChange_scrollBar(int value)
{
	this->setValue(value);
}

void QSuspendedScrollBar::slot_rangeChanged(int min, int max)
{
	this->setMinimum(min);
	this->setRange(0, max);
	this->setPageStep(0.75 * (this->height() + max));
	if (max <= 0)
	{
		this->hide();
	}
}