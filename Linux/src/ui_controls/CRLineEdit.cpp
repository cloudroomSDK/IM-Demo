#include "stdafx.h"
#include "CRLineEdit.h"

CRLineEdit::CRLineEdit(QWidget *parent) : QLineEdit(parent)
{
	QHBoxLayout *horizontalLayout = new QHBoxLayout(this);
	horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
	horizontalLayout->setContentsMargins(0, 0, 0, 0);

	m_pLeftIcon = new QWidget(this);
	m_pLeftIcon->hide();
	m_pLeftIcon->setObjectName("leftIcon");
	m_pLeftIcon->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	horizontalLayout->addWidget(m_pLeftIcon);

	QSpacerItem *horizontalSpacer = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
	horizontalLayout->addItem(horizontalSpacer);

	m_pClearBtn = new QToolButton(this);
	m_pClearBtn->hide();
	m_pClearBtn->setFocusPolicy(Qt::NoFocus);
	m_pClearBtn->setObjectName("clsBtn");
	m_pClearBtn->setCursor(QCursor(Qt::ArrowCursor));
	m_pClearBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	horizontalLayout->addWidget(m_pClearBtn);

	connect(m_pClearBtn, SIGNAL(clicked(bool)), this, SLOT(slot_clearClicked()));
	connect(this, SIGNAL(textChanged(const QString &)), this, SLOT(slot_textChanged(const QString &)));

	m_pClearBtn->setProperty("_cr_visible", int(CLRV_AUTO));
}

void CRLineEdit::setLeftIconVisible(bool bShow)
{
	m_pLeftIcon->setVisible(bShow);
	resized();
}

void CRLineEdit::setClearBtnVisibleType(ClearVisibleType vType)
{
	m_pClearBtn->setProperty("_cr_visible", int(vType));
	slot_textChanged(this->text());
	resized();
}

void CRLineEdit::showEvent(QShowEvent *event)
{
	QLineEdit::showEvent(event);
}


void CRLineEdit::resizeEvent(QResizeEvent *event)
{
	QLineEdit::resizeEvent(event);
	int iconH = this->height();
	m_pLeftIcon->setFixedSize(iconH, iconH);
	m_pClearBtn->setFixedSize(iconH, iconH);
	resized();
}

void CRLineEdit::resized()
{
	int leftIconW = m_pLeftIcon->isVisibleTo(this) ? m_pLeftIcon->width() : 0;
	int clsBtnW = m_pClearBtn->isVisibleTo(this) ? m_pClearBtn->width() : 0;

	QMargins contentMgs(0,0,0,0);
	contentMgs.setLeft(contentMgs.left() + leftIconW);
	contentMgs.setRight(contentMgs.right() + clsBtnW);
	this->setTextMargins(contentMgs);
	this->layout()->activate();
}

void CRLineEdit::slot_textChanged( const QString &text )
{
	ClearVisibleType vType = (ClearVisibleType)(m_pClearBtn->property("_cr_visible").toInt());
	bool bShow = false;
	if ( (!isReadOnly()) && (vType == CLRV_SHOWALWAYS || (vType == CLRV_AUTO && !text.isEmpty())))
		bShow = true;

	if (m_pClearBtn->isVisibleTo(this) != bShow)
	{
		m_pClearBtn->setVisible(bShow);
		resized();
		WidgetStyleUpdate(this);
	}
}


void CRLineEdit::slot_clearClicked()
{
	this->clear();
	emit clearBtnClicked();
}

void CRLineEdit::addExtRightWidget(QWidget *pWidget)
{
	this->layout()->addWidget(pWidget);
}
