#ifndef QIMAGELABEL_H
#define QIMAGELABEL_H

#include <QLabel>

class QImageLabel : public QLabel
{
	Q_OBJECT

	Q_PROPERTY(QString imageFile READ imageFile WRITE setImageFile)
	Q_PROPERTY(int angle READ angle WRITE setAngle)

public:
	QImageLabel(QWidget *parent);
	~QImageLabel();

	virtual void setImageFile(const QString &fileName);
	QString imageFile() { return m_fileName; }

	void setAngle(int n);
	int angle() { return m_angle; }

protected:
	void paintEvent(QPaintEvent *e) override;
	QSize sizeHint() const override;
	QSize minimumSizeHint() const override;
	virtual QSize getImgTargetSize() const;
	virtual QRect getImgTargetRt() const;

protected:
	QSize imgDefualtSize() const;

protected:
	QSvgRenderer	m_svgRender;
	QString			m_fileName;
	int				m_angle{ 0 };
	Qt::AspectRatioMode m_aspectMode{ Qt::KeepAspectRatio };
};

#endif // QIMAGELABEL_H
