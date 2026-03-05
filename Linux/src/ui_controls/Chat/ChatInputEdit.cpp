#include "stdafx.h"
#include "ChatInputEdit.h"
#include "ChatPublic.h"
#include "mypicswidget.h"
#include "FaceHelper.h"

#define  IMG_MAXSIZE ScreenScale(150)
#define  MAX_IM_FILE_SIZE 200

ChatInputEdit::ChatInputEdit(QWidget *parent)
	: QTextBrowser(parent)
{
	//自定义系统菜单
	setContextMenuPolicy(Qt::CustomContextMenu);
	
	//自定义菜单信号处理
	connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(customContextMenuRequestedSlot(const QPoint&)));
	
	m_leftSelImg = false;
	m_clearEnable = true;
	m_bKeySkip_Tab = false;
	this->viewport()->installEventFilter(this);
	QObject *textInterface = new DocumentTextObject(this);
	textInterface->setParent(this);
	this->document()->documentLayout()->registerHandler(DocumentTextObject::getObjectType(), textInterface);
}


ChatInputEdit::~ChatInputEdit()
{
	clear();
	MypicsWidget::destroyInstance();
}

void ChatInputEdit::insertImage(const QImage& image)
{
	QString strName = save(image);
	if (strName.isEmpty())
	{
		return;
	}
	insertImage(strName);
}

void ChatInputEdit::insertImage(const QString &imagePathName)
{
	QFileInfo fi(imagePathName);
	if (fi.size() > (MAX_IM_FILE_SIZE*1024*1024))
	{
		CRMsgBox::information(this, tr("提示"), tr("无法发送超过%1MB的图片!").arg(MAX_IM_FILE_SIZE));
		return;
	}
	QUrl u = QUrl::fromLocalFile(imagePathName);
	QString newFileName = u.toString();
	this->replaceImg(newFileName);
	// 	QTextImageFormat imageFormat;
	// 	imageFormat.setName(newFileName);
	// 	ui.msgSendWidget->textCursor().insertImage(imageFormat);
	//只能使用这种方式写入图片，不然连续的相同图片遍历时，会当成一个处理
	this->insertHtml(QString("<img src=\"%1\"/>").arg(newFileName));
	this->setFocus();
}

void ChatInputEdit::insertFile(const QString &filePathName)
{
	QFileInfo fi(filePathName);
	if (fi.size() > 100 * 1024 * 1024)//最大100M
	{
		CRMsgBox::information(this, tr("提示"), tr("无法发送超过100M的文件!"));
		return;
	}
	QUrl resUrl = QUrl::fromLocalFile(fi.absoluteFilePath());
	if (getResources().contains(resUrl.toString()))
	{
		return;
	}
	QTextImageFormat imageFormat;
	imageFormat.setName(resUrl.toString());
	imageFormat.setToolTip(fi.fileName());
	imageFormat.setProperty(PFMT_ISFILE, true);
	this->replaceImg(imageFormat.name(), CFT_FILE);
	this->textCursor().insertImage(imageFormat);
}

QImage ChatInputEdit::createFileImage(const QString &strFile)
{
	//获取文件图标
	QFileInfo fi(strFile);
	QFileIconProvider icon_provider;
	icon_provider.setOptions(QFileIconProvider::DontUseCustomDirectoryIcons);
	QIcon fileIcon = icon_provider.icon(fi);

	//图标目标大小（获取的图片可能大于目标大小，需要等比缩小）
	QSize dstIconSize(32, 32);
	QPixmap fileTypePic = fileIcon.pixmap(dstIconSize);
	QSize fileTypeOrgSz = fileTypePic.size();
	dstIconSize = fileTypeOrgSz.scaled(dstIconSize, Qt::KeepAspectRatio);

	//生成elidedText的文本串
	int maxW = 100;
	QMargins margin(6, 3, 6, 3);
	QFontMetrics fm(this->font());
	QString newFileName = fm.elidedText(fi.fileName(), Qt::ElideRight, maxW - margin.left() - margin.right());
	int newFileNameWidth = fm.width(newFileName);

	//生成目标图像背景
	QSize imgSize(qMax(dstIconSize.width(), newFileNameWidth) + margin.left() + margin.right(), 60);
	QImage rslt(imgSize, QImage::Format_ARGB32);
	rslt.fill(QColor(Qt::transparent));

	QPainter painter(&rslt);
	painter.setFont(this->font());
	painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform, true);

	//draw入icon
	QRect picRect = QRect((imgSize.width() - dstIconSize.width()) / 2, margin.top(), dstIconSize.width(), dstIconSize.height());
	painter.drawPixmap(picRect, fileTypePic);

	//draw入文件名
	QRect textRect = QRect(margin.left(), margin.top(), imgSize.width() - margin.left()-margin.right(), imgSize.height()-margin.top() - margin.bottom());
	int drawFlags = Qt::TextSingleLine | Qt::AlignBottom | ((newFileNameWidth < imgSize.width()) ? Qt::AlignHCenter : Qt::AlignLeft);
	painter.drawText(textRect, drawFlags, newFileName);
	return rslt;
}

//复制
QMimeData *ChatInputEdit::createMimeDataFromSelection() const
{
	QMimeData *data = QTextBrowser::createMimeDataFromSelection();
	return data;
}

//粘贴
void ChatInputEdit::insertFromMimeData(const QMimeData * source)
{
	if (source->hasImage())  //IMG
	{
		//粘贴图片
		QImage imageContent = source->imageData().value<QImage>();

		QString strName = save(imageContent);
		if (! strName.isEmpty())
		{
			QUrl u = QUrl::fromLocalFile(strName);
			QString tmp = u.toString();
			replaceImg(tmp);
			this->insertHtml(QString("<img src=\"%1\"/>").arg(tmp));
		}

		return;
	}
	
	//HTML
	QString sourceHtml;
	try
	{
		if (source->hasHtml())
		{
			sourceHtml = source->html();
		}
	}
	catch (...)
	{
		return;
	}
	 
	if (!sourceHtml.isEmpty())//剪切板内容是富文本
	{
		QStringList picUrls;
		QString strHtml = ChatPublic::parseFullHtml(sourceHtml, picUrls);
		//添加图片资源到编辑框缓存
		for (auto &url : picUrls)
		{
			replaceImg(url);
		}
		//添加表情资源到编辑框缓存
		QList<QUrl> faceUrls;
		strHtml = ChatPublic::replaceFace2LocRes(strHtml, faceUrls);
		for (auto &url : faceUrls)
		{
			replaceImg(url.toString());
		}
		//设置内容
		this->insertHtml(strHtml);
	}
	else if (source->hasUrls())//剪切板内容是文件
	{
		//粘贴本地文件
		QList<QUrl> urls = source->urls();
		for (auto &url : urls)
		{
			if (url.isLocalFile())
			{
				insertFile(url.toLocalFile());
			}
		}
	}
	else if (source->hasText() && !source->text().isEmpty())//剪切板内容是文本
	{
		QString text = source->text();
		QList<QUrl> faceUrls;
		text = replaceFace2LocRes(text, faceUrls);
		//如果有表情资源，则替换后需要以富文本方式写入
		if (faceUrls.size()> 0)
		{
			//添加表情资源到编辑框缓存
			for (auto &url : faceUrls)
			{
				replaceImg(url.toString());
			}
			this->insertHtml(text);
		}
		else
		{
			this->insertPlainText(text);
		}
	}
}

void ChatInputEdit::customContextMenuRequestedSlot(const QPoint& pos)
{
	if (getFormatTypeFromPos(pos) == CFT_IMG || m_leftSelImg)
	{
		selImage(pos);
		QMenu *pMenu = createStandardContextMenu(pos);
		pMenu->setAttribute(Qt::WA_TranslucentBackground);
		QList<QAction*> al = pMenu->actions();

		QAction *pSaveAction = new QAction(tr("图片另存为..."),pMenu);
		connect(pSaveAction, SIGNAL(triggered()), this, SLOT(saveImageAs()) );
		pMenu->addAction(pSaveAction);

		pMenu->exec(mapToGlobal(pos));
		delete pSaveAction;

		m_leftSelImg = false;
		QTextCursor cursor = textCursor();
		cursor.clearSelection();
		this->setTextCursor(cursor);
	} 
	else
	{
		QMenu *pMenu = createStandardContextMenu(pos);
		pMenu->setAttribute(Qt::WA_TranslucentBackground);
		QList<QAction*> al = pMenu->actions();

		for (int i=0; i<al.size(); ++i)
		{
			QAction* pAt = al[i];
			QString txt = pAt->text();
			if (txt.contains("Ctrl+V", Qt::CaseInsensitive))
			{
				pAt->setEnabled(canPaste());
			}
		}
		pMenu->exec(mapToGlobal(pos));
	}
}

void ChatInputEdit::mousePressEvent(QMouseEvent * e)
{
	if ( e->button() == Qt::LeftButton )
	{
		m_leftSelImg = false;
	}
	if ( e->button() == Qt::LeftButton && selImage(e->pos()))	
	{
		m_leftSelImg = true;
	}
	QTextBrowser::mousePressEvent(e);
}

void ChatInputEdit::mouseMoveEvent(QMouseEvent *e)
{
    QTextBrowser::mouseMoveEvent(e);
	if (getFormatTypeFromPos(e->pos()) == CFT_IMG)
	{	
		QToolTip::showText( mapToGlobal(e->pos()), tr("双击查看原图") );		
	}
	else
	{
		QToolTip::hideText();
	}
}

void ChatInputEdit::mouseDoubleClickEvent(QMouseEvent * e)
{
	QTextCursor tc;
	if (!getTextCorsor(e->pos(), tc))
	{
		QTextBrowser::mouseDoubleClickEvent(e);
		return;
	}

	QTextCharFormat format = tc.charFormat();
	CharFormatType type = getFormatType(format);
	if (type == CFT_IMG)
	{
		QString image = format.toImageFormat().name();
		QUrl u(image);
		if (u.isLocalFile())
		{
			image = u.toLocalFile();
		}
		MypicsWidget::showInstance(image, this);
	}
// 	else if (type == CFT_FILE)
// 	{
// 		QString image = format.toImageFormat().name();
// 	}
	else
	{
		QTextBrowser::mouseDoubleClickEvent(e);
	}
}

void ChatInputEdit::resizeEvent(QResizeEvent * e)
{
	if ( isReadOnly() && (e->oldSize().width()!=e->size().width()) )
	{
		scaledResources();
		return;
	}
    QTextBrowser::resizeEvent(e);
}

bool ChatInputEdit::getTextCorsor(const QPoint& pos, QTextCursor &cursor)
{
	int hvalue = horizontalScrollBar()->value();
	int vvalue = verticalScrollBar()->value();
	QPoint realPos(pos.x()+hvalue, pos.y()+vvalue);
	int position = document()->documentLayout()->hitTest(realPos, Qt::ExactHit);
	if(position < 0) return false;

	cursor = textCursor();
	cursor.setPosition(position);
	if(cursor.atEnd())
		return false;

	cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, 1);//移动结束位置
	return true;
}


bool ChatInputEdit::selImage(const QPoint& pos)
{
	QTextCursor cursor;
	if ( !getTextCorsor(pos, cursor) )
		return false;

	QTextCharFormat format = cursor.charFormat();
	CharFormatType cft = getFormatType(format);
	if (cft != CFT_IMG)
		return false;

	this->setTextCursor(cursor);
	return true;
}

ChatInputEdit::CharFormatType ChatInputEdit::getFormatTypeFromPos(const QPoint& pos)
{
	QTextCursor cursor;
	if ( !getTextCorsor(pos, cursor) )
		return CFT_UNDEFINE;

	QTextCharFormat format = cursor.charFormat();
	return getFormatType(format);
}

ChatInputEdit::CharFormatType ChatInputEdit::getFormatType(const QTextCharFormat &charFormat) const 
{
	if (charFormat.isImageFormat())
	{
		QTextImageFormat tf = charFormat.toImageFormat();
		QString strName = tf.name();
		//表情
		if (strName.contains("file::/Res"))
			return CFT_FACE;
		//文件
		if (tf.property(PFMT_ISFILE).toBool())
			return CFT_FILE;
		//图片
		return CFT_IMG;
	}
	else if (charFormat.isCharFormat())
	{
		if (charFormat.objectType() == DocumentTextObject::getObjectType())
		{
			return CFT_CUSTOM_TEXT;
		}
		return CFT_TEXT;
	}
	return CFT_UNDEFINE;
}

QMap<QString, ChatInputEdit::CharFormatType> ChatInputEdit::getResources()
{
	QMap<QString, ChatInputEdit::CharFormatType> resList;
	QTextDocument *pDocument = document();
	QTextBlock currentBlock = pDocument->begin();
	while (currentBlock.isValid())
	{
		for (QTextBlock::iterator it = currentBlock.begin(); !(it.atEnd()); ++it)
		{
			QTextFragment currentFragment = it.fragment();
			QTextCharFormat  currentFormat = currentFragment.charFormat();
			QTextImageFormat tf = currentFormat.toImageFormat();
			
			if (!tf.isValid()) continue;

			resList[tf.name()] = getFormatType(currentFormat);
		}
		currentBlock = currentBlock.next();
	}
	return resList;
}

void ChatInputEdit::scaledResources()
{
	QMap<QString, ChatInputEdit::CharFormatType> resList = getResources();
	for (auto it = resList.begin(); it != resList.end(); it++)
	{
		replaceImg(it.key(), it.value());
	}
	this->setLineWrapColumnOrWidth(this->lineWrapColumnOrWidth());
}

void ChatInputEdit::replaceImg(const QString& strName, ChatInputEdit::CharFormatType type)
{
	QString imageName(strName);
	QUrl u(imageName);
	if (u.isLocalFile())
	{
		imageName = u.toLocalFile();
		if (type == CFT_FILE)
		{
			QImage img = createFileImage(imageName);
			document()->addResource(QTextDocument::ImageResource, QUrl(strName), img);
			return;
		}
		else
		{
			if (imageName.contains(".gif"))//如果是本地的gif图片，则添加到map管理
			{
				addGifToMap(imageName);
				return;
			}
		}
	} 

	QImageReader imgReader(imageName, "jpg;png;bmp");
	QSize imgSize = imgReader.size();
	if (imgSize.isEmpty())
	{
		//文件后缀和实际格式可能不一致，需要根据内容获取格式
		imgReader.setDecideFormatFromContent(true);
		imgReader.setFileName(imgReader.fileName());
		imgSize = imgReader.size();
	}
	if (imgSize.isEmpty())
	{
		return;
	}

	if (qMax(imgSize.width(), imgSize.height()) > IMG_MAXSIZE)
	{
		imgSize.scale(QSize(IMG_MAXSIZE, IMG_MAXSIZE), Qt::KeepAspectRatio);
		imgReader.setScaledSize(imgSize);
	}
	document()->addResource(QTextDocument::ImageResource, QUrl(strName), imgReader.read());
}

void ChatInputEdit::saveImageAs()
{
	QTextCharFormat format = textCursor().charFormat();
	QString imageName = format.toImageFormat().name();
	if (imageName.isEmpty()) return;

	QUrl u(imageName);
	if (u.isLocalFile())
	{
		imageName = u.toLocalFile();
	}

	QString filters = tr("PNG (*.png);;JPG (*.jpg);;BMP (*.bmp)");
	QString fileName = QFileDialog::getSaveFileName(this, tr("另存为..."),"D:/", filters);
	if (fileName.isEmpty()) 
		return;

	//重名,则删除
	QDir dir;
	if ( dir.exists(fileName) )
		dir.remove(fileName);

	QImage img(imageName);
	if (!(img.save(fileName, NULL, 95)))
	{
		CRMsgBox::information(this, tr("错误"), tr("保存图片失败！"));
	}
}

bool ChatInputEdit::canPaste()
{
	const QMimeData *md = QApplication::clipboard()->mimeData();
	QList<QUrl> lurl = md->urls();
	if (!lurl.empty())
	{
		if (lurl[0].isLocalFile())
		{
			if (checkStringPicPath(lurl[0].toLocalFile()))
			{
				return true;
			}
		}
	}
	if (md->hasHtml() && !md->html().isEmpty())
	{
		return true;
	}
	if (md->hasText() && !md->text().isEmpty())
	{
		return true;
	}
	if (md->hasImage())
	{
		return true;
	}
	return false;
}

bool ChatInputEdit::checkStringPicPath(const QString& txt)
{
	QFileInfo fi(txt);
	if (!fi.exists())
	{
		return false;
	}
	QString suff = fi.suffix();   
	if (suff.compare("png", Qt::CaseInsensitive) == 0 || 
		suff.compare("jpg", Qt::CaseInsensitive) == 0 ||
		suff.compare("jpeg", Qt::CaseInsensitive) == 0 ||
		suff.compare("bmp", Qt::CaseInsensitive) == 0)
	{
		return true;
	}
	return false;
}

void ChatInputEdit::setCursorAtEnd()
{
	if(!( this->textCursor().atEnd()) )
	{
		QTextCursor c =  this->textCursor();
		c.movePosition(QTextCursor::End);
		this->setTextCursor(c);
	}
}

void ChatInputEdit::setPlaceholderText(const QString& placeholderText)
{
//	QTextBrowser::setPlaceholderText(placeholderText);
	if (m_placeholderText != placeholderText)
	{
		m_placeholderText = placeholderText;
		if (!hasFocus())
		{
			this->viewport()->update();
			this->update();
		}
	}
}

void ChatInputEdit::addGifToMap(const QString &fileName)
{
	if (!fileName.endsWith(".gif", Qt::CaseInsensitive))
		return;

	//如果已经在map中
	if (m_urlMovieMap.find(fileName) != m_urlMovieMap.end())
	{
		return;
	}
	QMovie *movie = new QMovie(fileName);
	if (fileName.contains("/Res/"))
	{
		movie->setScaledSize(QSize(ScreenScale(28), ScreenScale(28)));//默认显示28*28的表情
	}
	movie->jumpToFrame(0);

	document()->addResource(QTextDocument::ImageResource, QUrl::fromLocalFile(movie->fileName()), movie->currentPixmap()); 
	connect(movie, SIGNAL(frameChanged(int)), this, SLOT(slot_gifAnimation(int)));  
	movie->start();
	m_urlMovieMap[fileName] = movie;
}

void ChatInputEdit::slot_gifAnimation(int)
{
	QMovie* movie = qobject_cast<QMovie*>(sender());
	document()->addResource(QTextDocument::ImageResource, QUrl::fromLocalFile(movie->fileName()), movie->currentPixmap()); 
	this->viewport()->update();
}

void ChatInputEdit::focusInEvent(QFocusEvent *e)
{
	QTextBrowser::focusInEvent(e);
	this->viewport()->update();
	//this->update();
}

void ChatInputEdit::focusOutEvent(QFocusEvent *e)
{
	QTextBrowser::focusOutEvent(e);
	this->viewport()->update();
	//this->update();
}

bool ChatInputEdit::eventFilter(QObject *obj, QEvent *ev)
{
	if (obj == this->viewport() && ev->type() == QEvent::Paint)
	{
		QPainter p(this->viewport());
		if (toPlainText().isEmpty())
		{
			if (!hasFocus() && !m_placeholderText.isEmpty())
			{
				QFontMetrics fm = fontMetrics();
				Qt::Alignment va = QStyle::visualAlignment(this->layoutDirection(), QFlag(this->alignment()));

				QPalette pal = palette();
				QColor col = pal.text().color();
				col.setAlpha(128);
				p.setPen(col);
				QString elidedText = fm.elidedText(m_placeholderText, Qt::ElideRight, this->rect().width());
				QRect rt = getTxtRect();
				p.drawText(getTxtRect(), va, elidedText);
				return true;
			}
			
		}
	}
	return QTextBrowser::eventFilter(obj, ev);
}
void ChatInputEdit::keyPressEvent(QKeyEvent *e)
{
	if(m_bKeySkip_Tab && (e->key() == Qt::Key_Tab))
	{
		return;
	}
	else
	{
		QTextBrowser::keyPressEvent(e);
	}
}

QRect ChatInputEdit::getTxtRect()
{
	int margin = this->document()->documentMargin();
	int cursorOffset = cursorRect().height() - fontMetrics().height();
	QPoint pLeftTop(0, 0);
	pLeftTop.setX(pLeftTop.x() + margin /*+ 1*/);
	pLeftTop.setY(pLeftTop.y() + margin + cursorOffset/* + 1*/);
	QSize textSize(this->width()-2*margin, this->height() - 2*margin);

	QRect retRect(pLeftTop, textSize);
	return retRect;
}

void ChatInputEdit::clear()
{
	QTextBrowser::clear();

	QMap<QString, QMovie*>::iterator it = m_urlMovieMap.begin();
	while (it != m_urlMovieMap.end())
	{
		QMovie *movie = it.value();
		movie->deleteLater();
		it = m_urlMovieMap.erase(it);
	}
}
