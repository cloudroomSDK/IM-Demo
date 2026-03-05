#include "stdafx.h"
#include "QClickedLabel.h"

QClickedLabel::QClickedLabel(QWidget *parent) : QLabel(parent)
{
}


QClickedLabel::~QClickedLabel()
{
}

void QClickedLabel::mouseReleaseEvent(QMouseEvent *ev)
{
	emit clicked(this);

}

void QClickedLabel::mouseDoubleClickEvent(QMouseEvent *ev)
{
	emit doubleClick(this);
}
