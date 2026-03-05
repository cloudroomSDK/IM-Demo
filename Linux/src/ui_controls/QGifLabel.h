#ifndef QGIFLABEL_H
#define QGIFLABEL_H

#include "QImageLabel.h"

class QGifLabel : public QImageLabel
{
	Q_OBJECT
	Q_PROPERTY(int speed READ speed WRITE setSpeed)

public:
	QGifLabel(QWidget *parent = 0);
	~QGifLabel();

	void setImageFile(const QString &fileName) override;

	void setSpeed(int speed);
	int speed() { return m_speed; }

protected:
	void showEvent(QShowEvent *e) override;
	void hideEvent(QHideEvent *e) override;
	QSize getImgTargetSize() const override;

protected:
	void updateTimer();

protected slots:
	void onTimerOut();

private:
	QTimer	m_timer;
	int		m_speed;
};


#endif // QGIFLABEL_H
