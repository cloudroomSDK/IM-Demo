#include "stdafx.h"
#include "IMFaceWidget.h"

IMFaceListItemWidget::IMFaceListItemWidget(QWidget *parent)
: QWidget(parent)
{
	QHBoxLayout *lay = new QHBoxLayout(this);
	lay->setContentsMargins(0,0,0,0);
	lay->setSpacing(0);
	m_lbPic = new QLabel(this);
	lay->addWidget(m_lbPic,0,Qt::AlignCenter);
}

void IMFaceListItemWidget::setInfo(const QString &fileName, const QSize &icoSize)
{
	m_movie.setFileName(fileName);
	m_lbPic->setMovie(&m_movie);
	m_movie.setScaledSize(icoSize);
	m_movie.jumpToFrame(0);//显示第一帧图片
}

void IMFaceListItemWidget::enterEvent(QEvent *)
{
	m_movie.start();
}

void IMFaceListItemWidget::leaveEvent(QEvent *)
{
	m_movie.stop();
	m_movie.jumpToFrame(0);//显示第一帧图片
}

void IMFaceListItemWidget::mouseReleaseEvent(QMouseEvent *)
{
	emit s_clicked();
}

IMFaceWidget::IMFaceWidget(QWidget *parent, QToolButton *pBinCheckBtn, ImgDef &imgs)
: QWidget(parent)
{
	this->setObjectName("imImageSelpage");

	this->setWindowFlags(Qt::Popup); 
	this->setAttribute(Qt::WA_DeleteOnClose);

	m_imgsDef = imgs;
	m_pBinCheckBtn = pBinCheckBtn;

	QHBoxLayout *pLayout = new QHBoxLayout(this);
	pLayout->setSpacing(0);
	pLayout->setContentsMargins(0, 0, 0, 0);
 	m_icoListWidget = new QListWidget(this);
	m_icoListWidget->setObjectName("faceImgList");
 	m_icoListWidget->setViewMode(QListView::IconMode);
	pLayout->addWidget(m_icoListWidget);

	setImageList(imgs);
	this->setFixedSize(310, 190);

	QPoint orgPt = m_pBinCheckBtn->mapToGlobal(m_pBinCheckBtn->rect().topLeft());
	QPoint rsltPt(orgPt.x(), orgPt.y() - this->height());
	if ((rsltPt.x() + this->width()) >= QApplication::desktop()->availableGeometry(orgPt).right())
	{
		rsltPt.setX(rsltPt.x() - this->width() + m_pBinCheckBtn->width());
	}
	if (rsltPt.y() < 0)
	{
		rsltPt.setY(rsltPt.y() + this->height() + m_pBinCheckBtn->height());
	}
	this->move(rsltPt);
	this->show();
}

IMFaceWidget::~IMFaceWidget()
{

}

void IMFaceWidget::setImageList(ImgDef &imgs)
{
	for(auto &fileName : imgs.imgs)
	{
		QListWidgetItem *item = new QListWidgetItem(m_icoListWidget);
		item->setData(Qt::UserRole, fileName);
		item->setSizeHint(m_imgsDef.iconSize + QSize(8, 8));
		m_icoListWidget->addItem(item);
		
    	IMFaceListItemWidget *pItemWidget = new IMFaceListItemWidget();
		pItemWidget->setInfo(fileName, m_imgsDef.iconSize);
    	connect(pItemWidget, &IMFaceListItemWidget::s_clicked, this, &IMFaceWidget::slot_itemClicked);
		m_icoListWidget->setItemWidget(item, pItemWidget);
	}
}

void IMFaceWidget::slot_itemClicked()
{
    IMFaceListItemWidget *pWidget = dynamic_cast<IMFaceListItemWidget*>(sender());
	//LogAssert(pWidget!=NULL, __FILE__, __LINE__);

	emit s_faceSelSig(pWidget->getFileName());
	
	this->close();
}

void IMFaceWidget::closeEvent(QCloseEvent * e)
{
	QPoint pt = QCursor::pos();
	QRect  btnRt = m_pBinCheckBtn->geometry();
	if ( btnRt.contains(pt) )
		return;

	m_pBinCheckBtn->setChecked(false);
}
