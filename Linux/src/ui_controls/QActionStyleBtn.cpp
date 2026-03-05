#include "stdafx.h"
#include "QActionStyleBtn.h"

QActionStyleBtn::QActionStyleBtn(QWidget *parent) : QToolButton(parent)
{
	m_type = 0;
	m_pAction = NULL;
	m_isHover = false;
	m_isPressed = false;
	m_bWaitting = false;
	m_movie = new QMovie(this);
	connect(this, SIGNAL(toggled(bool)), this, SLOT(OnToggled(bool)));
	connect(m_movie, SIGNAL(frameChanged(int)), this, SLOT(slot_gifAnimation(int)));
}

QActionStyleBtn::QActionStyleBtn(QWidget *parent, QAction *pAction, int type) : QActionStyleBtn(parent)
{
	SetAction(pAction, type);
}

void QActionStyleBtn::SetAction(QAction *pAction, int bindType)
{
	if (m_pAction != NULL)
	{
		this->disconnect(m_pAction);
		m_pAction->disconnect(this);
	}
	m_type = bindType;
	m_pAction = pAction;

	OnActionChanged();
	connect(m_pAction, SIGNAL(changed()), this, SLOT(OnActionChanged()));
	if ((m_type&BIND_CLICK) == BIND_CLICK)
	{
		connect(this, SIGNAL(clicked(bool)), m_pAction, SIGNAL(triggered(bool)), Qt::UniqueConnection);
		connect(this, SIGNAL(clicked(bool)), m_pAction, SLOT(toggle()), Qt::UniqueConnection);
	}
	else if ((m_type&BIND_DCLICK) == BIND_DCLICK)
	{
		connect(this, SIGNAL(doubleClicked()), m_pAction, SIGNAL(triggered()), Qt::UniqueConnection);
	}
}

 
QActionStyleBtn::~QActionStyleBtn()
{
}

void QActionStyleBtn::OnActionChanged()
{
	if ( m_type & BIND_NAME )
	{	
		this->setText(m_pAction->text());
	}
// 	if ( m_type & BIND_ICON )
// 	{
// 		this->setIcon(m_pAction->icon());
// 	}
	if ( m_type & BIND_ENABLE ) this->setEnabled(m_pAction->isEnabled());
	if ( m_type & BIND_VISIABLE ) this->setVisible(m_pAction->isVisible());
	if ( m_type & BIND_TOOLTIP ) this->setToolTip(m_pAction->toolTip());
	if ( m_type & BIND_CHECK_STATE )
	{
		this->setCheckable(m_pAction->isCheckable());
		this->setChecked(m_pAction->isChecked());
	}
}

void QActionStyleBtn::enterEvent(QEvent *e)
{
	QToolButton::enterEvent(e);
	setHover(true);
}

void QActionStyleBtn::leaveEvent(QEvent *e)
{
	QToolButton::leaveEvent(e);
	setHover(false);
}

void QActionStyleBtn::mousePressEvent(QMouseEvent *e)
{
	QToolButton::mousePressEvent(e);
	setPressed(true);
}

void QActionStyleBtn::mouseReleaseEvent(QMouseEvent *e)
{
	setPressed(false);
	QToolButton::mouseReleaseEvent(e);
}

void QActionStyleBtn::setHover(bool b)
{
	if (!QToolButton::isEnabled())
		return;

	m_isHover = b;
	WidgetStyleUpdate(this);
}

void QActionStyleBtn::setPressed(bool b)
{
	if (!QToolButton::isEnabled())
		return;

	m_isPressed = b;
	WidgetStyleUpdate(this);
}

void QActionStyleBtn::setWaitting(bool bWaitting)
{
	m_bWaitting = bWaitting;
	WidgetStyleUpdate(this);
}

void QActionStyleBtn::setCustomIcon(const QString &icon)
{
	if (icon == m_customIcon)
	{
		return;
	}
	if (icon.endsWith(".gif"))
	{
		if (m_movie->fileName().isEmpty())
		{
			m_movie->setFileName(icon);
		}
		if (m_movie->state() == QMovie::NotRunning)
		{
			m_movie->start();
		}
	}
	else
	{
		m_movie->stop();
		m_movie->setFileName("");
	}
	m_customIcon = icon;
	this->setIcon(QIcon(m_customIcon));
}

QString QActionStyleBtn::customIcon() const
{
	return m_customIcon;
}

void QActionStyleBtn::OnToggled(bool checked)
{
	WidgetStyleUpdate(this);
}

void QActionStyleBtn::mouseDoubleClickEvent(QMouseEvent *event)
{
	emit doubleClicked();
	return QWidget::mouseDoubleClickEvent(event);
}

void QActionStyleBtn::slot_gifAnimation(int i)
{
	this->setIcon(m_movie->currentPixmap());
}


void QActionStyleBtn::setEnabled(bool b)
{
	QToolButton::setEnabled(b);

	if (!b)
	{
		m_isHover = false;
		m_isPressed = false;
	}

	WidgetStyleUpdate(this);
}

void QActionStyleBtn::setDisabled(bool b)
{
	setEnabled(!b);
}
