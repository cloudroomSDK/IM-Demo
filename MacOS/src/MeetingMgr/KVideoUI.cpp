#include "stdafx.h"
#include "KVideoUI.h"
#include "ui_KVideoUI.h"

KVideoUI::KVideoUI(QWidget* pParent) : CustomRenderBase(pParent)
{
	ui = new Ui::KVideoUI;
	ui->setupUi(this);
	this->layout()->setMargin(0);

	setDefaultBackground(QColor(0, 0, 0));
}

void KVideoUI::clean()
{
 	this->setVideoInfo(CRUserVideoID());
}

KVideoUI::~KVideoUI()
{
	g_sdkMain->getSDKMeeting().rmCustomRender(this);

	delete ui;
	ui = NULL;
}

void KVideoUI::setVideoInfo(const char* userID)
{
	CRVSDK::CRUserVideoID usrVideoID;
	usrVideoID._userID = CRBase::CRString(userID);
	usrVideoID._videoID = -1;
	setVideoInfo(usrVideoID);
}

void KVideoUI::setVideoInfo(const CRVSDK::CRUserVideoID &cam)
{
	if ( cam==m_vId )
	{
		return;
	}

	//应用新设置
	setLocMirror(false);
	m_vId = cam;
	//setDefaultBackground(m_vId._userID.isEmpty() ? QColor(0, 0, 0) : Qt::black);
	this->setVideoID(m_vId);
}

void KVideoUI::paintEvent(QPaintEvent *event)
{
	CustomRenderBase::paintEvent(event);

	QPainter p(this);

	//无图像时，绘制名称
	QSize pixSz = CustomRenderWidget::getFrameSize();
	if (pixSz.isEmpty() && g_sdkMain!=nullptr)
	{
		CRMeetingMember mInfo;
		g_sdkMain->getSDKMeeting().getMemberInfo(m_vId._userID.constData(), mInfo);
		QString name = crStrToQStr(mInfo._nickName.constData());

		int minWH = qMin(this->width(), this->height());
		int fontSz = (minWH > 200) ? 42 : 16;
		QFont f;
		f.setPixelSize(fontSz);
		p.setFont(f);

		QPen pen;
		pen.setColor(QColor(Qt::white));
		p.setPen(pen);

		int txtMargin = 6;
		QRect textRt = rect().adjusted(txtMargin, txtMargin, -txtMargin, -txtMargin);
		p.drawText(textRt, Qt::AlignHCenter | Qt::AlignVCenter | Qt::TextWordWrap, name);
	}

	//draw border
	{
		QPen pe(QColor(61, 61, 61));
		pe.setWidth(2);
		p.setPen(pe);
		p.setBrush(Qt::NoBrush);
		p.drawRect(rect());
	}
}
