#ifndef QSMOOTHSCROLLBAR_H
#define QSMOOTHSCROLLBAR_H
#include <qscrollbar.h>
#include <qtimer.h>

class QSmoothScrollbar : public QScrollBar 
{
public:
	explicit QSmoothScrollbar(QWidget* parent = nullptr);
	void enableScroll(bool bEnable);

protected:
	void wheelEvent(QWheelEvent* e) override;

private:
	double subDelta(double delta, int stepsLeft);

private slots:
	void slotSmoothMove();

private:
	QTimer* smoothMoveTimer;
	QWheelEvent* lastWheelEvent;

	int stepsTotal;
	QList<QPair<double, int>> stepsLeftQueue;
	bool m_bEnable;
};
#endif // MYCHATTEST_H
