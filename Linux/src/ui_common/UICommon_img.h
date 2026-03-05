#ifndef __COMMON_IMG_H__
#define __COMMON_IMG_H__


bool IsPicHasAlphaValue(const QImage &img);
bool IsPicHasAlphaValue_fast(const QImage &img);

bool setIndexImageTransparent(QImage &image, const QPoint &ponit);
QImage getImageFromFile(const QString &fileName, const QSize &sz = QSize(0, 0), Qt::AspectRatioMode asMode = Qt::KeepAspectRatio);
#ifdef QT_SVG_LIB
QPixmap svgRes2Pixmap(const QString &svgRes, const QSize &size = QSize(0, 0));
QPixmap svgData2Pixmap(const QByteArray &svgDat, const QSize &size = QSize(0, 0));
#endif

#endif //_COMMON_H__INCLUDED_