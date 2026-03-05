#include "stdafx.h"
#include "mypicswidget.h"
#include "QScrollAreaDragHandler.h"
#include "QImageLabel.h"

#define  SHOW_MINRATE	10
#define  SHOW_MAXRATE	300

MypicsWidget *MypicsWidget::s_Instance = NULL;

void MypicsWidget::showInstance(const QString& image, QWidget *parent)
{
	destroyInstance();
	s_Instance = new MypicsWidget(image, parent);
	s_Instance->show();
}

void MypicsWidget::destroyInstance()
{
	if (s_Instance != NULL)
	{
		s_Instance->close();
		s_Instance = NULL;
	}
}

MypicsWidget::MypicsWidget(const QString& image, QWidget *parent)
	: QNoNCDialog(parent)
{
	s_Instance = this;
	m_imgName = image;

	StandardTitleBar *pTitleBar = new StandardTitleBar(this, StandardTitleBar::TBTN_CLOSE | StandardTitleBar::TBTN_MAX);
	this->setTitleBar(pTitleBar);
	m_scrollArea = new QScrollArea(this);
	m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_scrollArea->setAlignment(Qt::AlignCenter);
	m_scrollArea->setFrameStyle(QFrame::NoFrame);
	this->setClientWidget(m_scrollArea);

	setAttribute(Qt::WA_TranslucentBackground, true);
	setAttribute(Qt::WA_DeleteOnClose, true);
	setFocusPolicy(Qt::StrongFocus);
	setMouseTracking(true);

	//自定义系统菜单
	setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, &MypicsWidget::customContextMenuRequested, this, &MypicsWidget::customContextMenuRequestedSlot);

	//QImageLabel放大时，占用内存更小
	m_imgLabel = new QImageLabel(this);
	m_scrollArea->setWidget(m_imgLabel);
	m_imgLabel->setObjectName("PicViewLabel");
	m_imgLabel->setScaledContents(true);
	m_imgLabel->setAlignment(Qt::AlignCenter);
	m_imgLabel->setCursor(QCursor(Qt::SizeAllCursor));

	m_rateLabel = new QLabel(this);
	m_rateLabel->setObjectName("PicViewRateLabel");
	m_rateLabel->setAlignment(Qt::AlignCenter);
	m_rateLabel->hide();

	m_hideRateTimer.setSingleShot(true);
	connect(&m_hideRateTimer, &QTimer::timeout, m_rateLabel, &QWidget::hide);

	//居中显示
	QRect screenRt = QApplication::desktop()->screenGeometry(parent);
	QSize dlgSZ = screenRt.size() *2 / 3;
	this->resize(dlgSZ);
	QPoint pt(screenRt.left() + (screenRt.width() - dlgSZ.width()) / 2, screenRt.top() + (screenRt.height() - dlgSZ.height()) / 2);
	s_Instance->move(pt);

	m_scrollArea->viewport()->installEventFilter(this);
	QScrollAreaDragHandler *pMvHand = new QScrollAreaDragHandler(this, m_scrollArea);

	m_showRate = 0;
}


MypicsWidget::~MypicsWidget()
{
	if (s_Instance == this)
		s_Instance = NULL;
}

QImage MypicsWidget::getImage()
{
	QImage tmpImg;
	QImageReader imgReader(m_imgName);
	if (!imgReader.canRead())
	{
		imgReader.setDecideFormatFromContent(true);
		imgReader.setFileName(imgReader.fileName());
	}

	if (!imgReader.canRead())
	{
		tmpImg.load(":/Res/IM/DefaultIcon.svg");
	}
	else
	{
		tmpImg = imgReader.read();
	}
	return tmpImg;
}

void MypicsWidget::showEvent(QShowEvent *evt)
{
	QNoNCDialog::showEvent(evt);
	if (m_showRate == 0)
	{
		initializeImg();
	}
}


void MypicsWidget::resizeEvent(QResizeEvent *)
{
	int titleBarH = titleBar()->height();
	QSize leftPos = (size() - QSize(0,titleBarH) - m_rateLabel->size()) / 2 + QSize(0, titleBarH) ;
	m_rateLabel->move(leftPos.width(), leftPos.height());
}

void MypicsWidget::initializeImg()
{
	QImage tmpImg = getImage();
	if (tmpImg.isNull())
		return;

	m_imgLabel->setPixmap(QPixmap::fromImage(tmpImg));
	m_imgSz = tmpImg.size();

	QSize clientSz = this->clientWidget()->size();
	int hRate = clientSz.width() * 100 / m_imgSz.width();
	int vRate = clientSz.height() * 100 / m_imgSz.height();
	m_showRate = qMin(hRate, vRate);
	m_showRate = qMin(m_showRate, 100); //默认不放大

	resizeImg(m_showRate, true);
}


void MypicsWidget::resizeImg(int rate, bool bShowRate)
{
	//记录中心位置
	int  oldRate = m_showRate;
	QWidget *pViewPort = m_scrollArea->viewport();
	QPoint aaa = pViewPort->mapToGlobal(QPoint(pViewPort->width()/2, pViewPort->height()/2));
	QPoint centPosAtImg = m_imgLabel->mapFromGlobal(aaa);

	//缩放范围控制
	m_showRate = rate;
	m_showRate = qMax(m_showRate, SHOW_MINRATE);
	m_showRate = qMin(m_showRate, SHOW_MAXRATE);

	//缩放
	QSize picNewSZ = m_imgSz * m_showRate / 100;
	m_imgLabel->resize(picNewSZ);
	if (bShowRate)
	{
		showRate();
	}

	//保持中心位置
	QPoint newCentPosAtImg = centPosAtImg * float(m_showRate) / oldRate;
	QPoint newOffset = newCentPosAtImg - QPoint(pViewPort->width() / 2, pViewPort->height() / 2);
	m_scrollArea->horizontalScrollBar()->setValue(newOffset.x());
	m_scrollArea->verticalScrollBar()->setValue(newOffset.y());

}

void MypicsWidget::showRate()
{
	QString showRate = QString("%1%").arg(m_showRate);
	m_rateLabel->setText(showRate);
	m_rateLabel->show();
	m_hideRateTimer.start(800);
}


bool MypicsWidget::eventFilter(QObject *obj, QEvent *evt)
{
	switch(evt->type())
	{
	case QEvent::MouseButtonRelease:
		{
			QMouseEvent* event = static_cast<QMouseEvent*>(evt);
			if (Qt::LeftButton == event->button())
			{
				this->close();
				return true;
			}
		}
		break;

	case QEvent::Wheel:
		{
			QWheelEvent* event = static_cast<QWheelEvent*>(evt);
			int numDegrees = event->delta() / 8;
			int numSteps = numDegrees / 15;

			int targetValue = m_showRate;
			for (int i = abs(numSteps); i > 0; i--)
			{
				targetValue *= (numSteps > 0) ? 1.1 : 0.9;
			}
			if ((m_showRate < 100 && targetValue>100) || (m_showRate > 100 && targetValue < 100))
			{
				targetValue = 100;
			}

			//垂直滚动
			resizeImg(targetValue, true);
			return true;
		}
		break;

	case QEvent::KeyPress:
		{
			QKeyEvent* event = static_cast<QKeyEvent*>(evt);
			if (event->key() == Qt::Key_Escape)
			{
				close();
				return true;
			}
		}
		break;

	default:break;
	}

	return QNoNCDialog::eventFilter(obj, evt);
}

void MypicsWidget::customContextMenuRequestedSlot(const QPoint& pos)
{
	QMenu *pMenu = new QMenu(this);
	pMenu->setAttribute(Qt::WA_TranslucentBackground);
	pMenu->setWindowFlags(Qt::FramelessWindowHint | pMenu->windowFlags());
	QObjectCleanupHandler handler;
	handler.add(pMenu);

	QAction *pCopyAction = new QAction(tr("拷贝"), pMenu);
	connect(pCopyAction, &QAction::triggered, this, &MypicsWidget::CopyImage);
	pMenu->addAction(pCopyAction);

	QAction *pResizeAction = new QAction(tr("原始大小"), pMenu);
	connect(pResizeAction, &QAction::triggered, this, &MypicsWidget::OriginalSize);
	pMenu->addAction(pResizeAction);

	QAction *pSaveAction = new QAction(tr("图片另存为..."), pMenu);
	connect(pSaveAction, &QAction::triggered, this, &MypicsWidget::saveImageAs);
	pMenu->addAction(pSaveAction);

	pMenu->addSeparator();

	QAction *pCloseAction = new QAction(tr("关闭"), pMenu);
	connect(pCloseAction, &QAction::triggered, this, &MypicsWidget::close);
	pMenu->addAction(pCloseAction);

	pMenu->exec(mapToGlobal(pos));
}

void MypicsWidget::OriginalSize()
{
	resizeImg(100, true);
}

void MypicsWidget::CopyImage()
{
	QClipboard *pClipboard = QApplication::clipboard();
	pClipboard->setImage(getImage());
}

void MypicsWidget::saveImageAs()
{
	if (m_imgName.isEmpty()) 
	{
		return;
	}

	QString filters = tr("JPG (*.jpg);;PNG (*.png);;BMP (*.bmp)");
	QString filePath = QDir::currentPath() +tr("未命名.jpg");
	QString fileName = QFileDialog::getSaveFileName(this, tr("保存图片"), filePath, filters);
	if (fileName.isEmpty()) 
		return;

	//重名,则删除
	QDir dir;
	if ( dir.exists(fileName) )
		dir.remove(fileName);

	QImage img(m_imgName);
	if (!(img.save(fileName, NULL, 95)))
	{
		CRMsgBox::warning(this, tr("错误"), tr("保存图片失败！"));
	}
}
