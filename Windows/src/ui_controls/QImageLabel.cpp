#include "stdafx.h"
#include "QImageLabel.h"

QImageLabel::QImageLabel(QWidget *parent)
	: QLabel(parent)
{

}

QImageLabel::~QImageLabel()
{

}

void QImageLabel::setImageFile(const QString &fileName)
{
	m_fileName = fileName;

	//svg
	if (fileName.endsWith(".svg", Qt::CaseInsensitive))
	{
		this->setPixmap(QPixmap());
		m_svgRender.load(m_fileName);
	}
	else
	{
		this->setPixmap(QPixmap(m_fileName));
		m_svgRender.load(QByteArray());
	}
	this->update();
}

void QImageLabel::setAngle(int n)
{
	m_angle = n;
	this->update();
}

QSize QImageLabel::imgDefualtSize() const
{
	QSize sz(0,0);
	if (m_svgRender.isValid())
	{
		sz = m_svgRender.defaultSize();
	}
	else
	{
		const QPixmap *pix = this->pixmap();
		if (pix != nullptr)
		{
			sz = pix->size();
		}
	}
	return sz;
}


QSize QImageLabel::sizeHint() const
{
	return imgDefualtSize();
}

QSize QImageLabel::minimumSizeHint() const
{
	return QSize(0, 0);
}

void QImageLabel::paintEvent(QPaintEvent *e)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

	QRect dstRt = getImgTargetRt();

	QPainter painter(this);
	painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
	painter.setRenderHint(QPainter::HighQualityAntialiasing, true);

	//变换
	painter.translate(dstRt.left() + dstRt.width() / 2.0, dstRt.top() + dstRt.height() / 2.0);
	painter.rotate(m_angle);
	painter.translate(-dstRt.left() - dstRt.width() / 2.0, -dstRt.top() - dstRt.height() / 2.0);

	//作图
	if (m_svgRender.isValid())
	{
		m_svgRender.render(&painter, dstRt);
	}
	else
	{
		const QPixmap *pix = this->pixmap();
		if (pix != nullptr)
		{
			painter.drawPixmap(dstRt, *pix);
		}
	}
}

QSize QImageLabel::getImgTargetSize() const
{
	QRect lbRt = contentsRect();
	QSize imgSz = imgDefualtSize();
	QSize rslt = imgSz.scaled(lbRt.size(), Qt::KeepAspectRatio);
	return rslt;
}


QRect QImageLabel::getImgTargetRt() const
{
	QRect lbRt = contentsRect();
	QPoint lbCenter = lbRt.center();
	QSize imgSz = getImgTargetSize();

	QPoint imgPos;
	//对齐方式
	Qt::Alignment align = this->alignment();
	if (align & Qt::AlignLeft)
	{
		imgPos.setX(lbRt.left());
	}
	else if (align & Qt::AlignRight)
	{
		imgPos.setX(lbRt.right() - imgSz.width());
	}
	else
	{
		imgPos.setX(lbCenter.x() - imgSz.width() / 2);
	}

	if (align & Qt::AlignTop)
	{
		imgPos.setY(lbRt.top());
	}
	else if (align & Qt::AlignBottom)
	{
		imgPos.setY(lbRt.bottom() - imgSz.height());
	}
	else
	{
		imgPos.setY(lbCenter.y() - imgSz.height() / 2);
	}
	return QRect(imgPos, imgSz);
}
