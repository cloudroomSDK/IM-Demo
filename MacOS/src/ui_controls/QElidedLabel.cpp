#include "stdafx.h"
#include "QElidedLabel.h"

QElidedLabel::QElidedLabel(QWidget *parent) : QLabel(parent)
{
	m_elideMode = Qt::ElideRight;
}


QElidedLabel::~QElidedLabel()
{
}

void QElidedLabel::setElideMode(Qt::TextElideMode elideMode) 
{
	m_elideMode = elideMode;
	update();
}

void QElidedLabel::setText(const QString &str)
{
    if(m_orgText == str)
        return;
    
	m_orgText = str;
	updateElidedText();
}

void QElidedLabel::resizeEvent(QResizeEvent *e) 
{
	QLabel::resizeEvent(e);
    
	updateElidedText();
}

void QElidedLabel::updateElidedText()
{
	const QFontMetrics fm(fontMetrics());
	QString cachedElidedText = fm.elidedText(m_orgText,
		m_elideMode,
		width());
    
	QLabel::setText(cachedElidedText);
}

QSize QElidedLabel::sizeHint() const
{
	const QFontMetrics fm(fontMetrics());
    QSize sz(fm.width(m_orgText) + 1, QLabel::sizeHint().height());
    return sz;
}
