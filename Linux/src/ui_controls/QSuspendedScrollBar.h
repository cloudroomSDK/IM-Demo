#ifndef QSuspendedScrollBar_H
#define QSuspendedScrollBar_H
#include <qscrollbar.h>
#include <qtimer.h>

class QSuspendedScrollBar : public QScrollBar 
{
public:
	explicit QSuspendedScrollBar(Qt::Orientation t, QAbstractScrollArea* parent);

protected:
	virtual bool eventFilter(QObject *watched, QEvent *event);
protected slots :
	void slot_valueChange_scrollBar(int);
	void slot_rangeChanged(int min, int max);

private:
	QAbstractScrollArea *m_area;
};
#endif // MYCHATTEST_H
