#include "stdafx.h"
#include "CRMsgBox.h"
#ifdef WIN32
    #include <WTypes.h>
    #include <WinUser.h>
#endif


CRMsgBox::CRMsgBox(QWidget *parent, const QString &title, const QString &msgContent, qint32 standardButtons) : QNoNCDialog(parent, DEF_WINFLAGS | Qt::WindowStaysOnTopHint)
{
	_defaultBtn = NULL;
	_escapeBtn = NULL;
	_clickedBtn = NULL;
	InitBox();

	setMsg(title, msgContent);
	addBtns(standardButtons);

	_countDownValue = 0;
	connect(&_countDownTimer, SIGNAL(timeout()), this, SLOT(slot_countDown()));
}

CRMsgBox::~CRMsgBox(void)
{
}

void CRMsgBox::InitBox()
{
//_____________________________
//| title                      |
//|----------------------------|
//|	          Msg              |
//|____________________________|
//|          buttons           |
//|____________________________|

#ifdef WIN32
	StandardTitleBar *pTitleBar = new StandardTitleBar(this, StandardTitleBar::TBTN_CLOSE);
#else
	StandardTitleBar *pTitleBar = new StandardTitleBar(this, StandardTitleBar::TBTN_NULL);
#endif
	this->setTitleBar(pTitleBar, false);


	//上部信息
	_msgLabel = new QLabel(this);
	_msgLabel->setObjectName(QLatin1String("CRMsg_label")); 
	_msgLabel->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
	_msgLabel->setWordWrap(false);
	_msgLabel->setOpenExternalLinks(true);
	this->setClientWidget(_msgLabel, false);

	//底部
	QWidget *bottomWidget = new QWidget(this);
	bottomWidget->setObjectName("frameBtns");
	QHBoxLayout *bottomWidgetLayout = new QHBoxLayout(bottomWidget);
	bottomWidgetLayout->setContentsMargins(20, 0, 20, 20);
	bottomWidgetLayout->addItem(new QSpacerItem(1, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

	//底部按钮区域
	_bottomBar = new QWidget(bottomWidget);
	_bottomBar->setObjectName("bottomBar");
	QHBoxLayout *bottomBarLayout = new QHBoxLayout(_bottomBar);
	bottomBarLayout->setContentsMargins(0,0,0,0);
	bottomBarLayout->setSpacing(10);

	bottomWidgetLayout->addWidget(_bottomBar);
	bottomWidgetLayout->addItem(new QSpacerItem(1, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
	this->setStatusBar(bottomWidget, false);

	this->reLayout();
	this->updateSize();
}

void CRMsgBox::setMsg(const QString &title, const QString &msg)
{
	StandardTitleBar *pTitle = dynamic_cast<StandardTitleBar*>(titleBar());
	if ( pTitle!=NULL )
	{
		pTitle->setTitleText(title);
	}

	_msgLabel->setText(msg);
	this->updateSize();
}

void CRMsgBox::addBtn(StandardButton btn)
{
	QAbstractButton *pBtn = NULL;
	switch(btn)
	{
	case Ok:
		pBtn = new QPushButton(this);
		pBtn->setObjectName("ok");
		pBtn->setText(tr("确定"));
		break;

	case Yes:
		pBtn = new QPushButton(this);
		pBtn->setObjectName("yes");
		pBtn->setText(tr("是"));
		break;

	case Cancel:
		pBtn = new QPushButton(this);
		pBtn->setObjectName("cancel");
		pBtn->setText(tr("取消"));
		break;

	case No:
		pBtn = new QPushButton(this);
		pBtn->setObjectName("no");
		pBtn->setText(tr("否"));
		break;

	case Close:
		pBtn = new QPushButton(this);
		pBtn->setObjectName("close");
		pBtn->setText(tr("关闭"));
		break;

	default:
		break;
	}

	if ( pBtn==NULL )
		return;

	QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
	sizePolicy.setHorizontalStretch(0);
	sizePolicy.setVerticalStretch(0);
	sizePolicy.setHeightForWidth(pBtn->sizePolicy().hasHeightForWidth());
	pBtn->setSizePolicy(sizePolicy);
	pBtn->setProperty("StandardButton", btn);
	this->addBtn(pBtn);
}


void CRMsgBox::addBtns(qint32 btns)
{
	for(int i=0; i<32; i++)
	{
		uint32_t btnBitValue = 1<<i;
		if ( btns & btnBitValue )
		{
			addBtn((StandardButton)btnBitValue);
		}
	}
}

CRMsgBox::StandardButton CRMsgBox::standardButton(QAbstractButton *button)
{
	//不在列表
	QList<QAbstractButton*> btnlist = buttons();
	QList<QAbstractButton*>::iterator pos = std::find(btnlist.begin(), btnlist.end(), button);
	if ( pos==btnlist.end() )
		return NoButton;

	int btnValue = button->property("StandardButton").toInt();
	return (StandardButton)btnValue;
}

QAbstractButton* CRMsgBox::standardButton(StandardButton btn)
{
	if ( btn==NoButton )
		return NULL;

	QList<QAbstractButton*> btnlist = buttons();
	QList<QAbstractButton*>::iterator pos = btnlist.begin();
	for(; pos!=btnlist.end(); pos++)
	{
		QAbstractButton *pBtn = *pos;
		QVariant qvar = pBtn->property("StandardButton");
		if ( qvar.isNull() )
			continue;

		int btnValue = qvar.toInt();
		if ( btnValue==btn )
			return pBtn;
	}

	return NULL;
}


void CRMsgBox::setDefaultBtn(StandardButton defaultBtn, int nSecond)
{
	QAbstractButton *btn = standardButton(defaultBtn);
	setDefaultBtn(btn,nSecond);
}

void CRMsgBox::setEscapeBtn(StandardButton escapeButton)
{
	QAbstractButton *btn = standardButton(escapeButton);
	setEscapeBtn(btn);
}

void CRMsgBox::addBtn(QAbstractButton* button)
{
	if ( button!=NULL )
	{
		_bottomBar->layout()->addWidget(button);
		connect(button, SIGNAL(clicked(bool)), this, SLOT(slot_btnClicked()), Qt::UniqueConnection);
		this->updateSize();
	}
}

void CRMsgBox::updateCountDownTxt(QAbstractButton* pBtn, int nSecond)
{
	if ( pBtn==NULL )
		return;

	//上次追加的
	QString appendStr;
	int lastSecond = pBtn->property("LastSecond").toInt();
	if ( lastSecond>0 )
	{
		appendStr = QString(" (%1)").arg(lastSecond);
	}
	//得到原始字符串
	QString btnTxt = pBtn->text();
	if ( appendStr.size()>0 && btnTxt.endsWith(appendStr) )
	{
		btnTxt = btnTxt.left(btnTxt.size()-appendStr.size());
	}

	//得到新的字符串
	pBtn->setProperty("LastSecond", nSecond);
	if ( nSecond )
	{
		appendStr = QString(" (%1)").arg(nSecond);
		btnTxt += appendStr;
	}

	pBtn->setText(btnTxt);
}


void CRMsgBox::setDefaultBtn(QAbstractButton* defaultBtn, int nSecond)
{
	//恢复上一个缺省按钮倒计时文字
	updateCountDownTxt(_defaultBtn, -1);

	//
	_defaultBtn = defaultBtn;
	if ( _defaultBtn!=NULL )
	{
		_defaultBtn->setFocus();
	}
	_countDownTimer.stop();
	_countDownValue = nSecond;

	//重新启动计时
	if ( _defaultBtn!=NULL && _countDownValue>=0 )
	{
		_countDownTimer.start(1000);
		updateCountDownTxt(_defaultBtn, _countDownValue);
	}

	this->updateSize();
}

void CRMsgBox::slot_countDown()
{
	//倒计时
	if ( _countDownValue>0 )
	{
		_countDownValue--;
	}
	if ( _countDownValue<=0 )
	{
		_countDownTimer.stop();
	}
	if ( _defaultBtn==NULL )
	{
		return;
	}

	//更新文字
	updateCountDownTxt(_defaultBtn, _countDownValue);

	//时间到执行操作
	if ( _countDownValue<=0 )
	{
		_defaultBtn->click();
	}
}



void CRMsgBox::setEscapeBtn(QAbstractButton* escapeButton)
{
	_escapeBtn = escapeButton;
	TitleBar *p = titleBar();
	if ( p==NULL )
		return;

	QAbstractButton *closeBtn = p->getSysBtn(TitleBar::TBTN_CLOSE);
	if ( closeBtn==NULL )
		return;

	closeBtn->setEnabled(_escapeBtn!=NULL);
}


QList<QAbstractButton*> CRMsgBox::buttons()
{
	QList<QAbstractButton*> btnlist;

	QLayout *btnsLayout = _bottomBar->layout();
	for (int i = 0; i<btnsLayout->count(); i++)
	{
		QWidget *pWidget = btnsLayout->itemAt(i)->widget();
		QAbstractButton *pBtn = dynamic_cast<QAbstractButton*>(pWidget);
		if (pBtn != NULL)
		{
			btnlist.push_back(pBtn);
		}
	}
	return btnlist;
}

void CRMsgBox::slot_btnClicked()
{
	_clickedBtn = dynamic_cast<QAbstractButton*>(sender());
	if ( _clickedBtn==NULL )
		return;
	emit buttonClicked(this, _clickedBtn);

	StandardButton standBtnValue = standardButton(_clickedBtn);
	done(standBtnValue);
}


void CRMsgBox::accept()
{
	QAbstractButton *pBtn = dynamic_cast<QAbstractButton*>(this->focusWidget());
	if ( pBtn==NULL )
	{
		pBtn = _defaultBtn;
	}

	if ( pBtn!=NULL )
	{
		pBtn->click();
	}
}

void CRMsgBox::reject()
{
	QAbstractButton* pBtn = _escapeBtn;
	while(1)
	{
		if ( pBtn!=NULL )
			break;

		QList<QAbstractButton*> btnsList = buttons();
		if ( btnsList.size()==1 )
		{
			pBtn = btnsList[0];
			break;
		}

		pBtn = standardButton(Cancel);
		if ( pBtn!=NULL )
			break;

		pBtn = standardButton(Close);
		if ( pBtn!=NULL )
			break;

		pBtn = standardButton(No);
		if ( pBtn!=NULL )
			break;

		break;
	}
		
	if ( pBtn!=NULL )
	{
		pBtn->click();
	}
}

CRMsgBox::StandardButton CRMsgBox::msgBox(QWidget *parent, const QString &title, const QString &text, qint32 standardButtons, StandardButton defaultBtn, int nSecondClickDefBtn)
{
	CRMsgBox* tmp = new CRMsgBox(parent, title, text, standardButtons);
	tmp->setAttribute(Qt::WA_DeleteOnClose);
	tmp->setDefaultBtn(defaultBtn, nSecondClickDefBtn);
	return (CRMsgBox::StandardButton)tmp->exec();
}

void CRMsgBox::updateSize()
{
	QSize screenSize = QApplication::desktop()->availableGeometry(QCursor::pos()).size();
	int maxWidth = screenSize.width()/2;

	bool bMutiLine = false;
	if (_msgLabel->text().contains(QChar('\n')))
	{
		//多行文本总是折行
		bMutiLine = true;
	}
	_msgLabel->setWordWrap(false);
	adjustDlgSize(this);
	int width = this->layout()->totalMinimumSize().width();
	if (width > maxWidth)
	{
		width = maxWidth;
		bMutiLine = true;
	}

	if (bMutiLine)
	{
		_msgLabel->setWordWrap(true);
		_msgLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
	}
	else
	{
		_msgLabel->setAlignment(Qt::AlignCenter);
	}
	if (width < 250)
	{
		width = 250;
	}

	layout()->activate();
	int height = (layout()->hasHeightForWidth())
		? layout()->totalHeightForWidth(width)
		: layout()->totalMinimumSize().height();

	this->resize(width, height);
	QCoreApplication::removePostedEvents(this, QEvent::LayoutRequest);
}

void CRMsgBox::showEvent(QShowEvent *event)
{
	QDialog::showEvent(event);
	updateSize();
}
