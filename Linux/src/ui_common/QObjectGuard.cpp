#include "stdafx.h"
#include "QObjectGuard.h"

QObjectGuard::~QObjectGuard()
{
	this->Release();
}

void QObjectGuard::Release()
{
	QObject *old = GetObject(true);
	if ( old!=NULL )
	{
		old->deleteLater();
		if (old->isWidgetType())
		{
			QWidget *p = static_cast<QWidget*>(old);
			p->close();
		}
	}
}


void QObjectGuard::SetObject(QObject *pObject)
{
	this->Release();
	if ( pObject==NULL )
		return;

	m_pObject = pObject;
	connect(pObject, SIGNAL(destroyed(QObject*)), this, SLOT(slot_ObjectDestroyed(QObject*)));
	emit s_changed();
}


QObject *QObjectGuard::GetObject(bool bPeakUp)
{
	QObject *tmp = m_pObject;
	if ( bPeakUp && m_pObject!=NULL )
	{
		disconnect(m_pObject);
		m_pObject = NULL;
		emit s_changed();
	}
	return tmp;
}

void QObjectGuard::slot_ObjectDestroyed(QObject* pObj)
{
	if ( m_pObject!=pObj )
		return;
	m_pObject = NULL;
	emit s_changed();
}
