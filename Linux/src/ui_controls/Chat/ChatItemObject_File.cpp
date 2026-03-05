#include "stdafx.h"
#include "ChatItemObject_File.h"
#include "ChatWidget.h"

ChatItemObject_File::ChatItemObject_File(const ChatStyle &style, QObject *parent) : ChatItemObject(style, parent)
{
	m_fileState = UFS_NULL;
	m_progress = 0.0;
}

ChatItemObject_File::~ChatItemObject_File()
{
}

void ChatItemObject_File::setMsgInfo(const UIMsgInfo &info)
{
	ChatItemObject::setMsgInfo(info);
	updateFileState(info.fileState, info.fileProgress);
}

void ChatItemObject_File::updateFileState(UIFileState state, float progress)
{
	bool bChanged = !qFuzzyCompare(progress, m_progress) || (state != m_fileState);
	m_fileState = state;
	m_progress = progress;
	if (bChanged)
	{
		update();
	}
}

void ChatItemObject_File::onMouseButtonEvent(QMouseEvent *mouseEvent)
{
	ChatItemObject::onMouseButtonEvent(mouseEvent);
	if (!m_contentRt.isNull() && m_contentRt.contains(mouseEvent->pos()))//点击内容区域
	{
		if (mouseEvent->button() == Qt::LeftButton && mouseEvent->type() == QEvent::MouseButtonRelease && !m_bDragingOnClick)//左键点击
		{
			QUrl url = QUrl::fromLocalFile(getFilePathName());
			QVariantMap exParams;
			exParams["type"] = CURSOR_FILE;
			exParams["cursorText"] = url;
			emit s_contentClicked(true, exParams);
		}
		else if (mouseEvent->button() == Qt::RightButton && mouseEvent->type() == QEvent::MouseButtonRelease)//右键点击
		{
			QUrl url = QUrl::fromLocalFile(getFilePathName());
			QVariantMap exParams;
			exParams["selContent"] = true;	//右键时，保持选中状态
			exParams["type"] = CURSOR_FILE;
			exParams["cursorText"] = url;
			emit s_contentClicked(false, exParams);
		}
	}
}

void ChatItemObject_File::drawContent(QPainter *p, const QRect &rect)
{
	ChatItemObject::drawContent(p, rect);
	//绘制消息内容
	p->setBrush(Qt::white);
	p->drawRect(rect);

	QFontMetrics fmName(m_style.getFontStyle(ChatStyle::Medium));
	QFontMetrics fmDesc(m_style.getFontStyle(ChatStyle::Small));
    int fmNameHeight = fmName.height();
#ifdef MAC
    fmNameHeight += 2;
#endif
	QMargins contentMargin(ScreenScale(15), ScreenScale(15), ScreenScale(15), ScreenScale(15));
	QSize typePicSize(ScreenScale(50), ScreenScale(50));
	QRect typePicRect = QRect(rect.right() - (contentMargin.right() + typePicSize.width()), rect.top() + contentMargin.top(), typePicSize.width(), typePicSize.height());
	QRect nameRt = QRect(rect.left() + contentMargin.left(), typePicRect.top(),
		rect.width() - (typePicRect.width() + 2 * contentMargin.right() + contentMargin.left()), fmNameHeight * 2);
	QRect descRt1 = QRect(nameRt.left(), rect.bottom() - (fmDesc.height() + contentMargin.bottom()), nameRt.width(), fmDesc.height());

	drawFileName(p, nameRt);

	QTextOption option(Qt::AlignLeft | Qt::AlignTop);
	option.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
	p->save();
	QString strDesc1 = getFileStateDesc() + getFileProgressDesc();
	p->setFont(m_style.getFontStyle(ChatStyle::Small));
	p->setPen(m_style.getColorStyle(ChatStyle::ContentColor2));
	p->drawText(descRt1, strDesc1, option);
	p->restore();

	QPixmap img = getFileTypePic(typePicSize);
	p->drawPixmap(typePicRect.x() + (typePicRect.width() - img.width()) / 2, typePicRect.y() + (typePicRect.height() - img.height()) / 2, img);

	if (hasSelection())
	{
		QColor clr = m_style.getColorStyle(ChatStyle::SelectBkColor);
		clr.setAlpha(80);
		p->setBrush(clr);
		p->drawRect(rect);
	}
}

void ChatItemObject_File::drawFileName(QPainter *p, const QRect &rt)
{
	p->save();
	p->setFont(m_style.getFontStyle(ChatStyle::Medium));
	QFontMetrics fm(m_style.getFontStyle(ChatStyle::Medium));
	QString srcFileName = getFileName();
	QString fileName = fm.elidedText(srcFileName, Qt::ElideRight, rt.width() * 2 - fm.maxWidth());
	QColor normalColor = m_style.getColorStyle(ChatStyle::ContentColor);
	QColor highLighterColor = m_style.getColorStyle(ChatStyle::HighLighterColor);

	bool bHighlighter = false;
	if (!getMsgInfo().highlighterText.isEmpty())
	{
		bHighlighter = srcFileName.contains(getMsgInfo().highlighterText, Qt::CaseInsensitive);
	}
	
	if (bHighlighter)
	{
		int curIndex = 0;

		while (srcFileName.indexOf(getMsgInfo().highlighterText, curIndex, Qt::CaseInsensitive) != -1)
		{
			int startIndex = qMin(srcFileName.indexOf(getMsgInfo().highlighterText, curIndex, Qt::CaseInsensitive), fileName.size());
			int endIndex = qMin(startIndex + getMsgInfo().highlighterText.length(), fileName.size());

			QString subText = fileName.mid(startIndex, endIndex - startIndex);

			//QString normalText = fileName.mid(curIndex, startIndex - curIndex);
			drawSubText(p, rt, fileName, curIndex, startIndex - curIndex, normalColor);
			//QString highlighterText = fileName.mid(startIndex, endIndex - startIndex);
			drawSubText(p, rt, fileName, startIndex, endIndex - startIndex, highLighterColor);
		
			curIndex = endIndex;
			if (endIndex - startIndex != getMsgInfo().highlighterText.length())
			{
				break;
			}
		}
		if (curIndex < fileName.size() -1)
		{
			drawSubText(p, rt, fileName, curIndex, fileName.size() - curIndex, normalColor);
		}
	}
	else
	{
// 		p->setPen(Qt::red);
// 		p->drawRect(rt);
		QTextOption option(Qt::AlignLeft | Qt::AlignTop);
		option.setWrapMode(QTextOption::WrapAnywhere);
		p->setPen(normalColor);
		p->drawText(rt, fileName, option);
	}
	p->restore();
}

void ChatItemObject_File::drawSubText(QPainter *p, const QRect &rt, const QString &text, int index, int len, const QColor &color)
{
	QFontMetrics fm(m_style.getFontStyle(ChatStyle::Medium));
	QString drawText = text.mid(index, len);
	if (drawText.isEmpty())
	{
		return;
	}
	
	QString preText = text.mid(0, index);
	int colIndex = fm.width(preText);
	int rowIndex = 0;
	if (colIndex > rt.width())
	{
		int wrapWidth = 0;
		for (int i = 0; i < preText.size(); i++)
		{
			int w = fm.width(preText.at(i));
			if (wrapWidth + w > rt.width())
			{
				break;
			}
			wrapWidth += w;
		}
		colIndex -= wrapWidth;
		rowIndex = 1;
	}
	else
	{
		int drawColIndex = colIndex;
		for (int i = 0; i < drawText.size(); i++)
		{
			int w = fm.width(drawText.at(i));
			if (drawColIndex + w > rt.width())
			{
				QString firstDrawText = drawText.mid(0, i);
				QRect firstDrawRect = QRect(rt.left() + colIndex, rt.top(), drawColIndex, fm.height());
				p->setPen(color);
				p->drawText(firstDrawRect, firstDrawText, Qt::AlignLeft | Qt::AlignTop);
				drawText = drawText.remove(0, i);

				colIndex = 0;
				rowIndex = 1;
				break;
			}
			drawColIndex += w;
		}
	}
	QRect drawRect = rt;
	drawRect.setLeft(rt.left() + colIndex);
	drawRect.setTop(rt.top() + (fm.height() *rowIndex));

	QTextOption option(Qt::AlignLeft | Qt::AlignTop);
	option.setWrapMode(QTextOption::WrapAnywhere);
	p->setPen(color);
	p->drawText(drawRect, drawText, option);
}

QString ChatItemObject_File::getFilePathName()
{
	return getMsgInfo().getLocFilePath();
}

QString ChatItemObject_File::getFileName()
{
	return getMsgInfo().getFileName();
}

QString ChatItemObject_File::getFileSize()
{
	qint64 fileSize = getMsgInfo().getFileSize();
	if (fileSize > 1024 * 1024 * 1024)
	{
		return QString::number(fileSize / (1024.0 * 1024 * 1024), 'f', 0) + QString("G");
	}
	else if (fileSize >  1024 * 1024)
	{
		return QString::number(fileSize / (1024.0 * 1024), 'f', 0) + QString("M");
	}
	else if (fileSize > 1024)
	{
		return QString::number(fileSize / 1024.0, 'f', 0) + QString("K");
	}
	return QString::number(fileSize, 'f', 0) + QString("B");
}

QPixmap ChatItemObject_File::getFileTypePic(const QSize &sz)
{
	QSize bkSize = sz;
	//传输中
	if (m_fileState != UFS_NORMAL && m_fileState != UFS_NORECV)
	{
		QPixmap bkPix = m_style.getSvgRes(":/Res/IM/File/trans.svg", bkSize);

		QPainter painter(&bkPix);
		painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
		
		QSize ringSize = QSize(ScreenScale(20), ScreenScale(20));
		QRect ringRt = QRect(QPoint((bkSize.width() - ringSize.width()) / 2, (bkSize.height() - ringSize.height()) / 2), ringSize);
		drawRing(&painter, ringRt, m_progress);

		QString statePixName = ":/Res/IM/File/pause.svg";
		QSize statePixSize = QSize(ScreenScale(6), ScreenScale(6));
		if (m_fileState == UFS_RECV_PAUSED || m_fileState == UFS_RECV_FAILED)
		{
			statePixName = ":/Res/IM/File/download.svg";
			statePixSize = QSize(ScreenScale(11), ScreenScale(11));
		}
		else if (m_fileState == UFS_SEND_PAUSED || m_fileState == UFS_SEND_FAILED)
		{
			statePixName = ":/Res/IM/File/upload.svg";
			statePixSize = QSize(ScreenScale(11), ScreenScale(11));
		}

		QPixmap statePix = m_style.getSvgRes(statePixName, statePixSize);
		painter.drawPixmap(ringRt.x() + (ringRt.width() - statePixSize.width()) / 2, ringRt.y() + (ringRt.height() - statePixSize.height())/ 2, statePix);
		return bkPix;
	}
	
	QFileInfo fi(getFileName());
	QMimeDatabase db;
	QMimeType mime = db.mimeTypeForFile(fi);
	if (mime.name().startsWith("image/")) 
	{
		return m_style.getSvgRes(":/Res/IM/File/jpg.svg", bkSize);
	}
	else if (mime.name().startsWith("text/"))
	{
		return m_style.getSvgRes(":/Res/IM/File/txt.svg", bkSize);
	}
	else if (mime.name().startsWith("audio/"))
	{
		return m_style.getSvgRes(":/Res/IM/File/audio.svg", bkSize);
	}
	else if (mime.name().startsWith("video/"))
	{
		return m_style.getSvgRes(":/Res/IM/File/video.svg", bkSize);
	}
	
	QString strSuffix = fi.suffix();
	if (strSuffix.contains("doc") || strSuffix.contains("docx"))
	{
		return m_style.getSvgRes(":/Res/IM/File/word.svg", bkSize);
	}
	else if (strSuffix.contains("ppt") || strSuffix.contains("pptx"))
	{
		return m_style.getSvgRes(":/Res/IM/File/ppt.svg", bkSize);
	}
	else if (strSuffix.contains("xls") || strSuffix.contains("xlsx"))
	{
		return m_style.getSvgRes(":/Res/IM/File/excel.svg", bkSize);
	}
	else if (strSuffix.contains("pdf"))
	{
		return m_style.getSvgRes(":/Res/IM/File/pdf.svg", bkSize);
	}
	else if (strSuffix.contains("rar") || strSuffix.contains("zip") || strSuffix.contains("7z"))
	{
		return m_style.getSvgRes(":/Res/IM/File/zip.svg", bkSize);
	}
	return m_style.getSvgRes(":/Res/IM/File/default.svg", bkSize);
}

QString ChatItemObject_File::getFileStateDesc()
{
	return getFileSize();
}

QString ChatItemObject_File::getFileProgressDesc()
{
	switch (m_fileState)
	{
	case ChatPublic::UFS_SENDING: return tr("(发送中)");
	case ChatPublic::UFS_SEND_PAUSED: return tr("(发送中)");
	case ChatPublic::UFS_SEND_FAILED: return tr("(发送失败)");
	case ChatPublic::UFS_NORECV: return tr("(未接收)");
	case ChatPublic::UFS_RECVING: return tr("(接收中)");
	case ChatPublic::UFS_RECV_PAUSED: return tr("(接收中)");
	case ChatPublic::UFS_RECV_FAILED: return tr("(接收失败)");
	default: return QString();
	}
}

ChatItemObject::SelectectObj ChatItemObject_File::getSelectedObject(bool bGetAllText)
{
	SelectectObj rsltObj;
	if (!hasSelection())
	{
		return rsltObj;
	}
	//text
	rsltObj.text = tr("[文件]");

	QString locFilePath = m_msgInfo.getLocFilePath();
	if (QFile::exists(locFilePath))
	{
		//url
		rsltObj.url = QUrl::fromLocalFile(locFilePath);
	}
	return rsltObj;
}