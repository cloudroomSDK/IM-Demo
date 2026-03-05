#include "stdafx.h"
#include "ChatItemObject_Html.h"
#include "ChatWidget.h"
#include "FaceHelper.h"

ChatItemObject_Html::ChatItemObject_Html(const ChatStyle &style, QObject *parent) : ChatItemObject(style, parent)
{
	m_imgState = UFS_NULL;
    m_imgProgress = 0.0;
	m_selectionStart = -1;
	m_selectionEnd = -1;
	m_curOffsetPt = QPoint(0, 0);
	QTextOption opt;
	opt.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
	m_doc = new QTextDocument(this);
	m_doc->setDefaultTextOption(opt);
	m_docHighLighter = new DocumentHighLighter(m_doc);
	updateStyle();
}

ChatItemObject_Html::~ChatItemObject_Html()
{
	for (auto it = m_gifMovies.begin(); it != m_gifMovies.end(); it++)
	{
		QMovie *movie = it.value();
		movie->stop();
		movie->deleteLater();
	}
	m_gifMovies.clear();
	
	m_docHighLighter->deleteLater();
	m_docHighLighter = NULL;
	m_doc->deleteLater();
	m_doc = NULL;
}

void ChatItemObject_Html::updateStyle()
{
	ChatItemObject::updateStyle();
	m_doc->setDefaultFont(m_style.getFontStyle(ChatStyle::Medium));
}

void ChatItemObject_Html::setMsgInfo(const UIMsgInfo &info)
{
	ChatItemObject::setMsgInfo(info);
	m_docHighLighter->setHighlighterText(info.highlighterText, m_style.getColorStyle(ChatStyle::HighLighterColor));
	QString htmlText = info.getDisplayContent();
	m_doc->setHtml(htmlText);
	QTextCursor cursor(m_doc);
	cursor.movePosition(QTextCursor::End);
	m_docPosition = cursor.position();

	updateFileState(info);
}

void ChatItemObject_Html::updateFileState(const UIMsgInfo &info)
{
	bool bChanged = !qFuzzyCompare(info.fileProgress, m_imgProgress) || (info.fileState != m_imgState);
	m_imgState = info.fileState;
	m_imgProgress = info.fileProgress;
	if (bChanged)
	{
		//处理表情、图片资源
		QTextBlock currentBlock = m_doc->begin();
		while (currentBlock.isValid())
		{
			for (QTextBlock::iterator it = currentBlock.begin(); !(it.atEnd()); ++it)
			{
				QTextFragment currentFragment = it.fragment();
				QTextCharFormat  currentFormat = currentFragment.charFormat();
				QTextImageFormat tf = currentFormat.toImageFormat();
				if (!tf.isValid()) continue;

				QUrl url(tf.name());
				updateResource(url);
			}
			currentBlock = currentBlock.next();
		}
	}
}

void ChatItemObject_Html::updateContentSize(int contentMaxWidth)
{
	int margin = 0;
	if (needDrawBubble())
	{
		if (m_msgInfo.isNormalMsg())
		{
			margin = 10;
		}
		else if (m_msgInfo.isSystemMsg())
		{
			margin = 10;
		}
	}
	
	m_doc->setDocumentMargin(margin);
	m_doc->setTextWidth(contentMaxWidth);
	m_contentSize = QSize(m_doc->idealWidth(), m_doc->size().height());
}

void ChatItemObject_Html::setVisible(bool bVisible)
{
	ChatItemObject::setVisible(bVisible);
	for (auto &movie : m_gifMovies.values())
	{
		if (bVisible)
		{
			movie->start();
		}
		else
		{
			movie->stop();
		}
	}
}

void ChatItemObject_Html::updateResource(const QUrl &imgUrl)
{
	ChatItemObject::updateResource(imgUrl);

	//最大显示尺寸
	QSize imgMaxSize = m_msgInfo.faceUrls.contains(imgUrl) ?
		m_style.getResourceMaxSize(ChatStyle::RESTYPE_FACE) : m_style.getResourceMaxSize(ChatStyle::RESTYPE_IMAGE);
	//文件不存在
	if (!QFile::exists(imgUrl.toLocalFile()))
	{
		QImage img = getDefaultImage(imgMaxSize);
		//makeImageRoundBound(img);
		m_doc->addResource(QTextDocument::ImageResource, imgUrl, img);
		return;
	}

	if (imgUrl.fileName().contains(".gif"))
	{
		QMovie *movie = m_gifMovies.value(imgUrl);
		if (movie == NULL)
		{
			movie = new QMovie(imgUrl.toLocalFile());
			connect(movie, &QMovie::frameChanged, this, &ChatItemObject_Html::slot_gifAnimation);
			m_gifMovies[imgUrl] = movie;
		}
		QSize picSize = movie->scaledSize();
		picSize.scale(imgMaxSize, Qt::KeepAspectRatio);
		movie->setScaledSize(picSize);
		movie->jumpToFrame(0);
		m_doc->addResource(QTextDocument::ImageResource, imgUrl, movie->currentImage());
		if (this->isVisible())
		{
			movie->start();
		}
	}
	else
	{
		QImage img = getImageFromFile(imgUrl.toLocalFile(), imgMaxSize);
		m_doc->addResource(QTextDocument::ImageResource, imgUrl, img);
	}
	this->update();
}

QImage ChatItemObject_Html::getDefaultImage(const QSize &imgSize)
{
	QImage img(imgSize, QImage::Format_ARGB32);
	img.fill(QColor(209, 209, 209, 255));
	if (m_imgState == UFS_NORECV)
	{
		QPainter painter(&img);
		painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);

		QString str = tr("图片未下载");
		painter.drawText(QRect(QPoint(), imgSize), str, QTextOption(Qt::AlignCenter));
	}
	else if (m_imgState == UFS_RECVING)
	{
		QPainter painter(&img);
		painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
		int sz = imgSize.width() > imgSize.height() ? imgSize.height() : imgSize.width();
		QSize ringSize(sz * 0.2, sz*0.2);
		QRect ringRt = QRect(QPoint((imgSize.width() - ringSize.width()) / 2, (imgSize.height() - ringSize.height()) / 2), ringSize);
		drawRing(&painter, ringRt, m_imgProgress);

		QSize statePixSize = ringSize / 2;
		QPixmap statePix = m_style.getSvgRes(":/Res/IM/File/download.svg", statePixSize);
		painter.drawPixmap(ringRt.x() + (ringRt.width() - statePix.width()) / 2, ringRt.y() + (ringRt.height() - statePix.height()) / 2, statePix);
	}
	else if (m_imgState == UFS_RECV_FAILED)
	{
		QPainter painter(&img);
		painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
		int sz = imgSize.width() > imgSize.height() ? imgSize.height() : imgSize.width();
		QSize failedImgSize(sz * 0.8, sz*0.8);
		QPixmap failedPix = m_style.getSvgRes(":/Res/IM/File/image_failed.svg", failedImgSize);
		painter.drawPixmap((imgSize.width() - failedPix.width()) / 2, (imgSize.height() - failedPix.height()) / 2, failedPix);
	}
	return img;
}

void ChatItemObject_Html::drawContent(QPainter *p, const QRect &rect)
{
	ChatItemObject::drawContent(p, rect);

	//绘制消息内容
	p->translate(rect.topLeft());
	m_curOffsetPt = rect.topLeft();
	// draw selection
	QAbstractTextDocumentLayout::PaintContext ctx;
	QAbstractTextDocumentLayout::Selection sel;

	if (hasSelection()) 
	{
		sel.cursor = QTextCursor(m_doc);
		sel.cursor.setPosition(getSelectionStart());
		sel.cursor.setPosition(getSelectionEnd(), QTextCursor::KeepAnchor);
	}

	//sel.format.setBackground(m_selectMode != 0 ? QColor(51,153,255): Qt::NoBrush);
	//sel.format.setForeground(m_selectMode != 0 ? Qt::white : QColor(51,51,51));
	sel.format.setBackground(m_style.getColorStyle(ChatStyle::SelectBkColor));
	sel.format.setForeground(m_style.getColorStyle(ChatStyle::SelectColor));
	ctx.selections.append(sel);
	ctx.palette.setColor(QPalette::Text, m_style.getColorStyle(ChatStyle::ContentColor));

	// draw text
	m_doc->documentLayout()->draw(p, ctx);
}

void ChatItemObject_Html::slot_gifAnimation(int numbr)
{
	QMovie* movie = qobject_cast<QMovie*>(sender());
	m_doc->addResource(QTextDocument::ImageResource, QUrl::fromLocalFile(movie->fileName()), movie->currentImage());
	this->update();
}

int ChatItemObject_Html::cursorFromPos(const QPoint &pos, bool fuzzy) const
{
	QPoint p = pos - m_curOffsetPt;
	return m_doc->documentLayout()->hitTest(p, fuzzy ? Qt::FuzzyHit : Qt::ExactHit);
}

void ChatItemObject_Html::selectionStart(const QPoint &pos)
{
	int cur = cursorFromPos(pos);
	if (cur >= 0)
	{
		m_selectionStart = cur;
	}
}

void ChatItemObject_Html::selectionMove(const QPoint &pos, bool bDownDirection)
{
	int cur = cursorFromPos(pos);
	if (cur >= 0)
	{
		m_selectionEnd = cur;
		if (m_selectionStart == -1)
		{
			m_selectionStart = bDownDirection ? 0 : m_docPosition;
		}
	}
}

void ChatItemObject_Html::selectionEnd(bool bDownDirection)
{
	m_selectionEnd = bDownDirection ? m_docPosition : 0;
	if (m_selectionStart == -1)
	{
		m_selectionStart = bDownDirection ? 0 : m_docPosition;
	}
}

void ChatItemObject_Html::selectionAll()
{
	m_selectionStart = 0;
	m_selectionEnd = m_docPosition;
}

void ChatItemObject_Html::selectionClear()
{
	m_selectionStart = -1;
	m_selectionEnd = -1;
}

bool ChatItemObject_Html::hasSelection() const
{
	return m_selectionEnd >= 0 && m_selectionStart >= 0 && m_selectionEnd != m_selectionStart;
}

void ChatItemObject_Html::getSelectedText(bool bSelectAll, QString &rsltHtml, QString &rsltText)
{
	QString strHtml, strText;
	if (bSelectAll)
	{
		strHtml = m_doc->toHtml();
		strText = m_doc->toPlainText();
	}
	else
	{
		QTextCursor cursor(m_doc);
		cursor.setPosition(getSelectionStart());
		cursor.setPosition(getSelectionEnd(), QTextCursor::KeepAnchor);
		strHtml = cursor.selection().toHtml();
		strText = cursor.selection().toPlainText();
    }
	strText = strText.replace(0xFFFC, "");//去除无效符号

	//如果包含表情资源，则需要加表情资源替换成unicode编码
	if (m_msgInfo.faceUrls.size() > 0)
	{
		QString tmpStrHtml = ChatPublic::replaceLocRes2Face(strHtml);
		QTextDocument d;
		d.setHtml(tmpStrHtml);
		strText = d.toPlainText();
	}
	if (m_msgInfo.contentType == CRIM_MsgType_Image)
	{
		strText = tr("[图片]");
	}

	rsltHtml = strHtml;
	rsltText = strText;
}

ChatItemObject::SelectectObj ChatItemObject_Html::getSelectedObject(bool bGetAllText)
{
	SelectectObj rsltObj;
	if (!hasSelection())
	{
		return rsltObj;
	}


	getSelectedText(bGetAllText, rsltObj.html, rsltObj.text);
	QString locFilePath = m_msgInfo.getLocFilePath();
	if (QFile::exists(locFilePath))
	{
		//url
		rsltObj.url = QUrl::fromLocalFile(locFilePath);
		//image
		rsltObj.image = locFilePath;
	}
	return rsltObj;
}

int ChatItemObject_Html::getSelectionStart() const
{
	return qMin(m_selectionStart, m_selectionEnd);
}

int ChatItemObject_Html::getSelectionEnd() const
{
	return qMax(m_selectionStart, m_selectionEnd);
}

void ChatItemObject_Html::onMouseMoveEvent(QMouseEvent *mouseEvent, QCursor &cursor)
{
	ChatItemObject::onMouseMoveEvent(mouseEvent, cursor);
	if (!m_contentRt.isNull() && m_contentRt.contains(mouseEvent->pos()))//内容区域
	{
		QString cursorTxt;
		CursorPosType type = getCursorType(mouseEvent->pos(), cursorTxt);
		cursor = type != CURSOR_NULL ? Qt::PointingHandCursor : Qt::ArrowCursor;
	}
}

void ChatItemObject_Html::onMouseButtonEvent(QMouseEvent *mouseEvent)
{
	ChatItemObject::onMouseButtonEvent(mouseEvent);
	if (!m_contentRt.isNull() && m_contentRt.contains(mouseEvent->pos()))//点击内容区域
	{
		if (mouseEvent->button() == Qt::LeftButton && mouseEvent->type() == QEvent::MouseButtonRelease && !m_bDragingOnClick)//左键点击
		{
			QString cursorTxt;
			CursorPosType type = getCursorType(mouseEvent->pos(), cursorTxt);

			QVariantMap exParams;
			exParams["type"] = type;
			exParams["cursorText"] = cursorTxt;
			emit s_contentClicked(true, exParams);
		}
		else if (mouseEvent->button() == Qt::RightButton && mouseEvent->type() == QEvent::MouseButtonRelease)//右键点击
		{	
			QString cursorText;
			CursorPosType type = getCursorType(mouseEvent->pos(), cursorText);

			QVariantMap exParams;
			exParams["selContent"] = !hasSelection();//右键时，没有选中，需要全选
			if (type != CURSOR_NULL)
			{
				exParams["type"] = type;
				exParams["cursorText"] = cursorText;
			}
			emit s_contentClicked(false, exParams);
		}
	}
}

QString ChatItemObject_Html::getImageName(const QPoint &pos)
{
	QString imageName;
	int position = cursorFromPos(pos, false);
	for (QTextBlock::Iterator itr = m_doc->firstBlock().begin(); itr != m_doc->firstBlock().end(); ++itr)
	{
		if (itr.fragment().contains(position) && itr.fragment().charFormat().isImageFormat())
		{
			QTextImageFormat imgFmt = itr.fragment().charFormat().toImageFormat();
			imageName = imgFmt.name();
			if (imageName.contains(":/Res/IM/Face"))
			{
				continue;
			}
			// 			QUrl u(imageName);
			// 			if (u.isLocalFile())
			// 			{
			// 				imageName = u.toLocalFile();
			// 			}
			return imageName;
		}
	}
	return "";
}

QString ChatItemObject_Html::getAnchorText(const QPoint &pos)
{
	QPoint p = pos - m_curOffsetPt;
	QString anchor = m_doc->documentLayout()->anchorAt(p);
	return anchor;
}

CursorPosType ChatItemObject_Html::getCursorType(const QPoint &pos, QString &rslt)
{
	rslt = getAnchorText(pos);
	if (!rslt.isEmpty())
	{
		return CURSOR_ANCHOR;
	}
	rslt = getImageName(pos);
	if (!rslt.isEmpty())
	{
		return CURSOR_IMAGE;
	}
	return CURSOR_NULL;
}
