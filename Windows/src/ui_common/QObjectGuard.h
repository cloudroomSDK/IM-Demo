#ifndef QOBJECT_GUARD_H
#define QOBJECT_GUARD_H

class QObjectGuard : public QObject
{
	Q_OBJECT
public:
	QObjectGuard() { m_pObject=NULL; }
	~QObjectGuard();

	void SetObject(QObject *pObject);
	QObject *GetObject(bool bPeakUp=false);
	void Release();

signals:
	void s_changed();

protected slots:
	void slot_ObjectDestroyed(QObject* pObj);

protected:
	QObject *m_pObject;
};

#endif