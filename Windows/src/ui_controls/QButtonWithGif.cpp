#include "stdafx.h"
#include "QButtonWithGif.h"
#include "QGifLabel.h"

QButtonWithGif::QButtonWithGif(QWidget *parent) : QPushButton(parent)
{
	m_lbGif = new QGifLabel(this);
	m_lbGif->hide();

	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->addStretch();
	layout->addWidget(m_lbGif);
}

void QButtonWithGif::setEnabled(bool bEnabled)
{
	QPushButton::setEnabled(bEnabled);
	if (!bEnabled)
	{
		m_lbGif->show();
	}
	else
	{
		m_lbGif->hide();
		this->repaint();
	}
}
