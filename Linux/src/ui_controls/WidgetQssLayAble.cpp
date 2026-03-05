#include "stdafx.h"
#include "WidgetQssLayAble.h"

WidgetQssLayAble::WidgetQssLayAble(QWidget *parent)
	: QWidget(parent)
{

}

WidgetQssLayAble::~WidgetQssLayAble()
{

}

void WidgetQssLayAble::paintEvent(QPaintEvent *event)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}


int  WidgetQssLayAble::layoutMargin() const
{
	QLayout *l = layout();
	if (l == NULL)
		return 0;
	return l->contentsMargins().left();
}

int WidgetQssLayAble::layoutMarginLeft() const
{
	QLayout *l = layout();
	if (l == NULL)
		return 0;
	return l->contentsMargins().left();
}

int WidgetQssLayAble::layoutMarginTop() const
{
	QLayout *l = layout();
	if (l == NULL)
		return 0;
	return l->contentsMargins().top();
}

int WidgetQssLayAble::layoutMarginRight() const
{
	QLayout *l = layout();
	if (l == NULL)
		return 0;
	return l->contentsMargins().right();
}

int WidgetQssLayAble::layoutMarginBottom() const
{
	QLayout *l = layout();
	if (l == NULL)
		return 0;
	return l->contentsMargins().bottom();
}

int WidgetQssLayAble::layoutSpacing() const
{
	QLayout *l = layout();
	if (l == NULL)
		return 0;
	return l->spacing();
}

int WidgetQssLayAble::layoutVSpacing() const
{
	QLayout *l = layout();
	QGridLayout *g = dynamic_cast<QGridLayout*>(l);
	if (g != NULL)
	{
		return g->verticalSpacing();
	}
	QFormLayout *f = dynamic_cast<QFormLayout*>(l);
	if (f != NULL)
	{
		return f->verticalSpacing();
	}
	return 0;
}

int WidgetQssLayAble::layoutHSpacing() const
{
	QLayout *l = layout();
	QGridLayout *g = dynamic_cast<QGridLayout*>(l);
	if (g != NULL)
	{
		return g->horizontalSpacing();
	}
	QFormLayout *f = dynamic_cast<QFormLayout*>(l);
	if (f != NULL)
	{
		return f->horizontalSpacing();
	}
	return 0;
}

void WidgetQssLayAble::setlayoutMargin(int v)
{
	QLayout *l = layout();
	if (l != NULL)
	{
		l->setContentsMargins(v, v, v, v);
	}
}

void WidgetQssLayAble::setlayoutMarginLeft(int v)
{
	QLayout *l = layout();
	if (l != NULL)
	{
		QMargins m = l->contentsMargins();
		m.setLeft(v);
		l->setContentsMargins(m);
	}
}
void WidgetQssLayAble::setlayoutMarginTop(int v)
{
	QLayout *l = layout();
	if (l != NULL)
	{
		QMargins m = l->contentsMargins();
		m.setTop(v);
		l->setContentsMargins(m);
	}
}
void WidgetQssLayAble::setlayoutMarginRight(int v)
{
	QLayout *l = layout();
	if (l != NULL)
	{
		QMargins m = l->contentsMargins();
		m.setRight(v);
		l->setContentsMargins(m);
	}
}
void WidgetQssLayAble::setlayoutMarginBottom(int v)
{
	QLayout *l = layout();
	if (l != NULL)
	{
		QMargins m = l->contentsMargins();
		m.setBottom(v);
		l->setContentsMargins(m);
	}
}
void WidgetQssLayAble::setlayoutSpacing(int v)
{
	QLayout *l = layout();
	if (l != NULL)
	{
		l->setSpacing(v);
	}
}
void WidgetQssLayAble::setlayoutVSpacing(int v)
{
	QLayout *l = layout();
	QGridLayout *g = dynamic_cast<QGridLayout*>(l);
	if (g != NULL)
	{
		g->setVerticalSpacing(v);
	}
	QFormLayout *f = dynamic_cast<QFormLayout*>(l);
	if (f != NULL)
	{
		f->setVerticalSpacing(v);
	}
}
void WidgetQssLayAble::setlayoutHSpacing(int v)
{
	QLayout *l = layout();
	QGridLayout *g = dynamic_cast<QGridLayout*>(l);
	if (g != NULL)
	{
		g->setHorizontalSpacing(v);
	}
	QFormLayout *f = dynamic_cast<QFormLayout*>(l);
	if (f != NULL)
	{
		f->setHorizontalSpacing(v);
	}
}
