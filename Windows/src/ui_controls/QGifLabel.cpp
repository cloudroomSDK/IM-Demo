#include "stdafx.h"
#include "QGifLabel.h"

QGifLabel::QGifLabel(QWidget *parent) : QImageLabel(parent)
{
	connect(&m_timer, &QTimer::timeout, this, &QGifLabel::onTimerOut);
	m_speed = 3;
}

QGifLabel::~QGifLabel()
{
}

void QGifLabel::setSpeed(int speed)
{
	m_speed = speed;
	updateTimer();
}

void QGifLabel::setImageFile(const QString &fileName)
{
	QImageLabel::setImageFile(fileName);
	QImageLabel::setAngle(0);
	
	updateTimer();
}

void QGifLabel::showEvent(QShowEvent *e)
{
	QImageLabel::showEvent(e);
	updateTimer();
}

void QGifLabel::hideEvent(QHideEvent *e)
{
	QImageLabel::hideEvent(e);
	updateTimer();
}

QSize QGifLabel::getImgTargetSize() const
{
	return QImageLabel::getImgTargetSize();
}


void QGifLabel::updateTimer()
{
	if (this->isVisible() && m_speed!=0)
	{
		m_timer.start(25);
	}
	else
	{
		m_timer.stop();
	}
}


void QGifLabel::onTimerOut()
{
	int ang = (m_angle + m_speed) % 360;
	this->setAngle(ang);
}
