#include "stdafx.h"
#include "UICommon_img.h"

bool IsPicHasAlphaValue(const QImage &img)
{
	if ( !img.hasAlphaChannel() )
		return false;
	if ( img.isNull() )
		return false;

	QImage img32 = img.convertToFormat(QImage::Format_ARGB32);
	const QRgb* pRgb = (const QRgb*)img32.constBits();
	int pixCount = img32.width() * img32.height();
	for(int i=0; i<pixCount; i++, pRgb++)
	{
		if ( qAlpha(*pRgb)!=0xFF )
			return true;
	}

	return false;
}

bool IsPicHasAlphaValue_fast(const QImage &img)
{
	if ( !img.hasAlphaChannel() )
		return false;
	if ( img.isNull() )
		return false;

	//取个十字架
	QSize imgSz = img.size();
	int midx = imgSz.width()/2;
	int midy = imgSz.height()/2;
	for(int x=0; x<imgSz.width(); x++)
	{
		QRgb pixCol = img.pixel(x, midy);
		if ( qAlpha(pixCol)!=0xFF )
			return true;
	}
	for(int y=0; y<imgSz.height(); y++)
	{
		QRgb pixCol = img.pixel(midx, y);
		if ( qAlpha(pixCol)!=0xFF )
			return true;
	}
	return false;
}


bool setIndexImageTransparent(QImage &image, const QPoint &ponit)  
{
	//判断所取的点是否在图片内部   
	if( !image.valid( ponit ) )
		return false;

	//获取对应点颜色
	QRgb replaceRgb = image.pixel( ponit );

	//得到颜色表
	QVector<QRgb> rgbVector = image.colorTable();
	//查找颜色
	int indx = rgbVector.indexOf(replaceRgb);
	if ( indx<0 )
		return false;

	//替换颜色
	QRgb transRGB = QColor(0,0,0,0).rgba();
	image.setColor(indx, transRGB);
	return true;
} 

QImage getImageFromFile(const QString &fileName, const QSize &sz, Qt::AspectRatioMode asMode)
{
	QImageReader imgReader(fileName);
	QSize imgSize = imgReader.size();
	if (imgSize.isEmpty())
	{
		//文件后缀和实际格式可能不一致，需要根据内容获取格式
		imgReader.setDecideFormatFromContent(true);
		imgReader.setFileName(fileName);
		imgSize = imgReader.size();
	}

	if (!sz.isEmpty() && imgSize != sz)
	{
		imgSize.scale(sz, asMode);
		imgReader.setScaledSize(imgSize);
	}

	QImage pic = imgReader.read();
	return pic;
}

#ifdef QT_SVG_LIB

QPixmap svgRes2Pixmap(const QString &svgFileName, const QSize &size /*= QSize(0, 0)*/)
{
	if (!svgFileName.endsWith(".svg", Qt::CaseInsensitive) || size.isEmpty())
		return QPixmap(svgFileName);

	QByteArray dat = ReadDataFromFile(svgFileName);
	return svgData2Pixmap(dat, size);
}

QPixmap svgData2Pixmap(const QByteArray &svgDat, const QSize &size)
{
	if (size.isEmpty())
	{
		return QPixmap();
	}

	QPixmap pixmap(size);
	pixmap.fill(Qt::transparent);
	QPainter p(&pixmap);

	static QSvgRenderer svgRender;
	svgRender.load(svgDat);
	QSize svgBox = svgRender.viewBox().size();
	svgBox.scale(size, Qt::KeepAspectRatio);

	QPoint offset((size.width() - svgBox.width()) / 2, (size.height() - svgBox.height()) / 2);
	QRect rt(offset, svgBox);
	svgRender.render(&p, rt);
	return pixmap;
}

#endif