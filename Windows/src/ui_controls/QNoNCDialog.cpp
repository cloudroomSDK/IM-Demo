#include "stdafx.h"
#include "QNoNCDialog.h"
#include "ui_standardTitleBar.h"
#include "UICommon.h"
#include "QWidgetResizeHandler.h"
#ifdef WIN32
#include <dwmapi.h>
#endif

#ifndef min
#define min(a,b)   (((a) < (b)) ? (a) : (b))
#endif

#define SHADOW_SIZE 8

TitleBar::TitleBar(QWidget *parent, unsigned int sysBtnStyle) : QWidget(parent)
{
	this->setObjectName("dlgTitleBar");

	_sysBtnStyle = sysBtnStyle;
	_closeBtn = NULL;
	_maxBtn = NULL;
	_restoreBtn = NULL;
	_minBtn = NULL;
	_pDlg = NULL;

	QNoNCDialog *pDlg = dynamic_cast<QNoNCDialog*>(parent->window());
	if (pDlg != nullptr)
	{
		bindDialog(pDlg);
	}
}


int TitleBar::dlgCornerType()
{
	if (_pDlg == nullptr)
		return 0;
	return _pDlg->dlgCornerType();
}

void TitleBar::setSysBtnStyle(unsigned int sysBtnStyle)
{
	if (_pDlg == nullptr)
		return; 

	_sysBtnStyle = sysBtnStyle;
	resetBtns();
	_pDlg->resetDialogFlags();
}

void TitleBar::bindDialog(QNoNCDialog *pDlg)
{
	_pDlg = pDlg;
	connect(_pDlg, &QNoNCDialog::s_WindowStateChanged, this, &TitleBar::slot_dlgStateChanged, Qt::UniqueConnection);
}

void TitleBar::bindTitleBarBtns(QToolButton *CloseBtn, QToolButton *MinBtn, QToolButton *RestoreBtn, QToolButton *MaxBtn)
{
	if (_pDlg == nullptr)
		return;

	_closeBtn = CloseBtn;
	_minBtn = MinBtn;
	_maxBtn = MaxBtn;
	_restoreBtn = RestoreBtn;

	Qt::ConnectionType connType = Qt::UniqueConnection;
	if (_closeBtn)
		connect(_closeBtn, SIGNAL(clicked()), _pDlg, SLOT(close()), connType);
	if (_minBtn)
		connect(_minBtn, SIGNAL(clicked()), _pDlg, SLOT(showMinimized()), connType);
	if (_maxBtn)
		connect(_maxBtn, SIGNAL(clicked()), _pDlg, SLOT(showMaximized()), connType);
	if (_restoreBtn)
		connect(_restoreBtn, SIGNAL(clicked()), _pDlg, SLOT(showNormal()), connType);

	this->resetBtns();
	_pDlg->resetDialogFlags();
}

QToolButton *TitleBar::getSysBtn(SYS_BTN_DEF type)
{
	switch (type)
	{
	case TBTN_CLOSE: return _closeBtn;
	case TBTN_MAX: return _maxBtn;
	case TBTN_MIN: return _minBtn;
	default: return NULL;
	}
}

void TitleBar::resetBtns()
{
	if (_pDlg == nullptr)
		return;

	bool bMaxed = _pDlg->isMaximized();
	bool bMined = _pDlg->isMinimized();
	if (_minBtn)
	{
		_minBtn->setVisible(!bMined && (_sysBtnStyle&TBTN_MIN));
	}
	if (_restoreBtn)
	{
		_restoreBtn->setVisible((bMaxed && (_sysBtnStyle&TBTN_MAX)) | (bMined && (_sysBtnStyle&TBTN_MIN)));
	}
	if (_maxBtn)
	{
		_maxBtn->setVisible(!bMaxed && (_sysBtnStyle&TBTN_MAX));
	}
	if (_closeBtn)
	{
		_closeBtn->setVisible(_sysBtnStyle&TBTN_CLOSE);
	}
}


void TitleBar::mouseDoubleClickEvent(QMouseEvent *event)
{
	if (event->button() != Qt::LeftButton)
		return;
	if (_pDlg == nullptr)
		return;

	if (_pDlg->isMaximized())
	{
		if (_restoreBtn != NULL && _restoreBtn->isVisible())
		{
			_restoreBtn->click();
		}
	}
	else
	{
		if (_maxBtn != NULL && _maxBtn->isVisible())
		{
			_maxBtn->click();
		}
	}
}

void TitleBar::paintEvent(QPaintEvent *event)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}


//////////////////////////////////////////////////////////////////////////
StandardTitleBar::StandardTitleBar(QNoNCDialog *parent, unsigned int sysBtnStyle) : TitleBar(parent, sysBtnStyle)
{
	ui = new Ui::standardTitleBar;
	ui->setupUi(this);

	ui->titleIcon->hide();
	ui->titleMinBtn->setFocusPolicy(Qt::NoFocus);
	ui->titleRestoreBtn->setFocusPolicy(Qt::NoFocus);
	ui->titleMaxBtn->setFocusPolicy(Qt::NoFocus);
	ui->titleCloseBtn->setFocusPolicy(Qt::NoFocus);
	this->bindTitleBarBtns(ui->titleCloseBtn, ui->titleMinBtn, ui->titleRestoreBtn, ui->titleMaxBtn);

	_alignFlag = Qt::AlignTop;
}

StandardTitleBar::~StandardTitleBar()
{
	delete ui;
}

void StandardTitleBar::addWidgetBeforeIcon(QWidget *page)
{
	ui->lexBtnsLayout->addWidget(page);
}

void StandardTitleBar::addWidgetAfterTitle(QWidget *page)
{
	ui->rexBtnsLayout->addWidget(page, 0, _alignFlag);
}

void StandardTitleBar::setRightBtnsAlign(Qt::AlignmentFlag alignFlag)
{
	if (_alignFlag == alignFlag)
		return;

	_alignFlag = alignFlag;
	QList<QAbstractButton*> rightBtns;
	rightBtns << ui->titleMinBtn << ui->titleRestoreBtn << ui->titleMaxBtn << ui->titleCloseBtn;
	for (QAbstractButton *pBtn : rightBtns)
	{
		ui->horizontalLayout->removeWidget(pBtn);
		ui->horizontalLayout->addWidget(pBtn, 0, _alignFlag);
	}
}

void StandardTitleBar::setTitleText(const QString &txt)
{
	if (txt != _pDlg->windowTitle())
	{
		_pDlg->setWindowTitle(txt);
	}
#ifndef MAC
	ui->titleText->setText(txt);
#endif
}

void StandardTitleBar::setIcon(const QPixmap &icon)
{
	TitleBar::setIcon(icon);
	ui->titleIcon->setVisible(!icon.isNull());
#ifndef MAC
	int iw = icon.width();
	ui->titleIcon->setMaximumWidth(iw);
	ui->titleIcon->setPixmap(icon);
#endif
}

//////////////////////////////////////////////////////////////////////////

QNoNCDialog::QNoNCDialog(QWidget *parent, Qt::WindowFlags flags, DLG_TYPE type, BORDER_TYPE borderType) : QDialog(parent, flags)
{
	this->setAttribute(Qt::WA_TranslucentBackground, true);//窗口透明
	m_mainLay = new QVBoxLayout(this);
	m_mainLay->setContentsMargins(0, 0, 0, 0);
	m_mainLay->setSpacing(0);
	m_pFrame = new QFrame(this);
	m_pFrame->setObjectName("dlgFrame");
	m_mainLay->addWidget(m_pFrame);
	QVBoxLayout *dlgFrameLy = new QVBoxLayout(m_pFrame);
	dlgFrameLy->setContentsMargins(0, 0, 0, 0);
	dlgFrameLy->setSpacing(0);

	m_borderType = borderType;
	if( !true )
	{
		//不透明时，不能有阴影效果
		if (m_borderType == BORDER_SHADOW)
			m_borderType = BORDER_LINE;
	}
#ifdef MAC
	//mac有系统边框，不再有边框
	m_borderType = BORDER_NONE;
#elif defined(LINUX)
	//uos系统可能原生的阴影效果，界面内部不能再加阴影
    QString osName = GetOSName();
    bool bUos = osName.contains("uos");
	if (bUos && m_borderType == BORDER_SHADOW)
	{
		m_borderType = BORDER_LINE;
	}
#endif

	setWindowType(type);

	m_reszHandler = new QWidgetResizeHandlerI(this);
}


QNoNCDialog::~QNoNCDialog()
{
}

QPixmap QNoNCDialog::getDlgPixmap()
{
	Qt::WindowFlags oldFlags = this->windowFlags();
	if ((oldFlags&Qt::Dialog) != Qt::Dialog)
	{
#ifdef MAC
		this->setWindowFlags(Qt::Dialog);
#else
		this->setWindowFlags(DEF_WINFLAGS);
#endif
	}

	QPixmap pixmap(this->size());
	pixmap.fill(QColor(255, 255, 255, 0));
	this->render(&pixmap);

	if (this->windowFlags() != oldFlags)
	{
		this->setWindowFlags(oldFlags);
	}

	return pixmap;
}

//设置窗体类型
void QNoNCDialog::setWindowType(DLG_TYPE type)
{
	m_cfgDlgType = type;
	if (!true)
	{
		//不支持透明时，不能实现圆角效果
		m_cfgDlgType = DLG_RECT;
	}
	reStyleSheet();
}

//重新设置样式
void QNoNCDialog::reStyleSheet()
{
	bool bStyleChanged = false;
	//全屏、不透明时用直角
	DLG_TYPE newType = (isMaximized() || isFullScreen()) ? DLG_RECT : m_cfgDlgType;
	if (newType != m_curDlgType)
	{
		m_curDlgType = newType;
		bStyleChanged = true;
	}

	//边框Margins
	QMargins borderMargins;
	BORDER_TYPE borderType = (BORDER_TYPE)dlgBorderType();
	if (borderType == BORDER_LINE)
	{
		borderMargins = QMargins(1, 1, 1, 1);
	}
	else if (borderType == BORDER_SHADOW)
	{
		borderMargins = QMargins(SHADOW_SIZE, SHADOW_SIZE, SHADOW_SIZE, SHADOW_SIZE);
	}
	 bool bBorderChanged = false;
	 if (m_mainLay->contentsMargins() != borderMargins)
	{
		m_mainLay->setContentsMargins(borderMargins);
		bBorderChanged = true;
		bStyleChanged = true;
	}

	if (bStyleChanged)
	{
		WidgetStyleUpdate(this);
		if (m_titleBar != NULL)
		{
			WidgetStyleUpdate(m_titleBar);
		}
	}

	if ( bBorderChanged )
	{
		emit s_WindowBorderChanged();
	}
}

void QNoNCDialog::showMinimized()
{
#ifdef LINUX
	this->setWindowFlags(DEF_WINFLAGS);
#endif

	_wndState |= Qt::WindowMinimized;
	QDialog::showMinimized();
}

void QNoNCDialog::showNormal()
{
#ifdef LINUX
	this->setWindowFlags(DEF_WINFLAGS);
#endif

	bool oldUpdatesEnabled = this->updatesEnabled();
	this->setUpdatesEnabled(false);

	_wndState = Qt::WindowNoState;
	QDialog::showNormal();

#ifdef LINUX
	if (_wndNorRt.isValid())
	{
		this->setGeometry(_wndNorRt);
	}
#endif

	reLayout();
	reStyleSheet();
	this->setUpdatesEnabled(oldUpdatesEnabled);
}

void QNoNCDialog::showMaximized()
{
#ifdef LINUX
	if (_wndState == Qt::WindowNoState && this->isVisible())
	{
		_wndNorRt = this->geometry();
	}
	this->setWindowFlags(DEF_WINFLAGS);
#endif

	bool oldUpdatesEnabled = this->updatesEnabled();
	this->setUpdatesEnabled(false);

	_wndState &= ~Qt::WindowMinimized;
	_wndState &= ~Qt::WindowFullScreen;
	_wndState |= Qt::WindowMaximized;
	QDialog::showMaximized();

#ifdef LINUX
	QRect rt = QApplication::desktop()->availableGeometry(this);
	this->setGeometry(rt);
#endif
#ifdef WIN32
	QRect rt = getDlgMaximumRt(false);
	this->setGeometry(rt);
#endif

	reLayout();
	reStyleSheet();
	this->setUpdatesEnabled(oldUpdatesEnabled);
}

void QNoNCDialog::showFullScreen()
{
#ifdef LINUX
	if (_wndState == Qt::WindowNoState && this->isVisible())
	{
		_wndNorRt = this->geometry();
	}
	//linux 下Qt::Window类型，才可以全屏显示（覆盖系统任务栏）
	this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
#endif

	bool oldUpdatesEnabled = this->updatesEnabled();
	this->setUpdatesEnabled(false);

	_wndState &= ~Qt::WindowMinimized;
	_wndState |= Qt::WindowFullScreen;
	QDialog::showFullScreen();

#ifdef LINUX
	QRect rt = QApplication::desktop()->screenGeometry(this);
	this->setGeometry(rt);
#endif

	reLayout();
	reStyleSheet();
	this->setUpdatesEnabled(oldUpdatesEnabled);
}


void QNoNCDialog::SetFullScreen(bool bFullScreen)
{
	Qt::WindowStates newST(_wndState);
	if (bFullScreen)
	{
		newST &= ~Qt::WindowMinimized;
		newST |= Qt::WindowFullScreen;
	}
	else
	{
		newST &= ~Qt::WindowFullScreen;
	}
	setWindowState(newST);
}

bool QNoNCDialog::isMinimized() const
{
	return (_wndState & Qt::WindowMinimized);
}
bool QNoNCDialog::isMaximized() const
{
	return (_wndState & Qt::WindowMaximized);
}
bool QNoNCDialog::isFullScreen() const
{
	return (_wndState & Qt::WindowFullScreen);
}
Qt::WindowStates QNoNCDialog::windowState() const
{
	Qt::WindowStates qtState = QDialog::windowState();
	return (_wndState & 0x7) | (qtState & 0xFFFFFFF8);
}

void QNoNCDialog::setWindowState(Qt::WindowStates state)
{
	if ( state==_wndState )
		return;

	if( state & Qt::WindowMinimized )
	{
		this->showMinimized();
	}
	else if ( state & Qt::WindowFullScreen )
	{
		this->showFullScreen();
	}
	else if ( state & Qt::WindowMaximized )
	{
		this->showMaximized();
	}
	else
	{
		this->showNormal();
	}
}


void QNoNCDialog::reLayout()
{
	QList<QWidget*> showWidgets;
	//标题
	if (m_titleBar != NULL)
	{
		bool showTitle = !isFullScreen();
#ifdef MAC
		showTitle = showTitle && (this->windowFlags()&Qt::FramelessWindowHint == 0 || m_bShowTitle);
#endif
		if (m_titleBar->isVisibleTo(this) != showTitle)
		{
			m_titleBar->setVisible(showTitle);
		}
	}

	//客户区
	if (m_Client != NULL)
	{
		if (!m_Client->isVisibleTo(this))
		{
			m_Client->setVisible(true);
		}
	}

	//状态栏
	if (m_Status != NULL)
	{
		bool bShow = !isFullScreen();
		if (m_Status->isVisibleTo(this) != bShow)
		{
			m_Status->setVisible(bShow);
		}
	}
}


void QNoNCDialog::resetDialogFlags()
{
	if (m_titleBar == NULL)
		return;

	unsigned int sysBtnST = m_titleBar->getSysBtnStyle();
	Qt::WindowFlags flags = DEF_WINFLAGS;
	if ((m_titleBar->getSysBtn(TitleBar::TBTN_CLOSE) != NULL) && (sysBtnST&TitleBar::TBTN_CLOSE))
	{
		flags |= Qt::WindowCloseButtonHint;
	}
	else
	{
		flags &= ~Qt::WindowCloseButtonHint;
	}

#ifdef WIN32
	//linux等平台，增加以下属性，会导致出现系统边框
	if ((m_titleBar->getSysBtn(TitleBar::TBTN_MIN) != NULL) && (sysBtnST&TitleBar::TBTN_MIN))
	{
		flags |= Qt::WindowMinimizeButtonHint;
	}
	else
	{
		flags &= ~Qt::WindowMinimizeButtonHint;
	}

	if ((m_titleBar->getSysBtn(TitleBar::TBTN_MAX) != NULL) && (sysBtnST&TitleBar::TBTN_MAX))
	{
		flags |= Qt::WindowMaximizeButtonHint;
	}
	else
	{
		flags &= ~Qt::WindowMaximizeButtonHint;
	}
#endif

	this->setWindowFlags(flags);
}

void QNoNCDialog::setTitleBar(TitleBar *pBar, bool bRelayoutNow)
{
	if (pBar != m_titleBar)
	{
		QVBoxLayout *playout = static_cast<QVBoxLayout*>(m_pFrame->layout());
		playout->removeWidget(m_titleBar);
		m_titleBar = pBar;
		if (m_titleBar != NULL)
		{
			m_titleBar->hide();
			playout->insertWidget(0, m_titleBar);
		}
	}
	reStyleSheet();
	resetDialogFlags();
	if (bRelayoutNow)
	{
		reLayout();
	}
}

void QNoNCDialog::onWindowTitleChange()
{
	if (m_titleBar)
	{
		m_titleBar->setTitleText(this->windowTitle());
	}
}


void QNoNCDialog::setClientWidget(QWidget *clientWidget, bool bRelayoutNow)
{
	if (clientWidget != m_Client)
	{
		QVBoxLayout *playout = static_cast<QVBoxLayout*>(m_pFrame->layout());
		playout->removeWidget(m_Client);
		m_Client = clientWidget;
		if (m_Client != NULL)
		{
			QString dlgTitle = m_Client->windowTitle();
			if (!dlgTitle.isEmpty())
			{
				setWindowTitle(dlgTitle);
			}
			int index = (m_titleBar == NULL) ? 0 : 1;
			playout->insertWidget(index, m_Client);
		}
	}
	//if (bRelayoutNow)
	{
		reLayout();
	}
}

void QNoNCDialog::setStatusBar(QWidget *statusDlg, bool bRelayoutNow)
{
	if (statusDlg != m_Status)
	{
		QVBoxLayout *playout = static_cast<QVBoxLayout*>(m_pFrame->layout());
		playout->removeWidget(m_Status);
		m_Status = statusDlg;
		if (m_Status != NULL)
		{
			m_Status->hide();
			int index = 0;
			if (m_titleBar != NULL) index++;
			if (m_Client != NULL) index++;
			playout->insertWidget(index, m_Status);
		}
	}
	if (bRelayoutNow)
	{
		reLayout();
	}
}

void QNoNCDialog::changeEvent(QEvent * event)
{
	QDialog::changeEvent(event);

	switch (event->type())
	{
	case QEvent::WindowTitleChange:
		onWindowTitleChange();
		break;

	case QEvent::WindowStateChange:
		onWndStateChange((QWindowStateChangeEvent*)event);
		break;

	case QEvent::WinIdChange:
		reStyleSheet();
		break;
	};
}

void QNoNCDialog::onWndStateChange(QWindowStateChangeEvent *evt)
{
	//从系统任务栏上点击程序，会取消最小化
	if ((evt->oldState() & Qt::WindowMinimized) && !(QDialog::windowState() & Qt::WindowMinimized))
	{
		_wndState &= ~Qt::WindowMinimized;
		this->setWindowState(_wndState);
	}

	emit s_WindowStateChanged();

	//qDebug() << "OnWndStateChange:" << evt->oldState() << " -> " << windowState();
	reStyleSheet();
}

bool QNoNCDialog::nativeEvent(const QByteArray &eventType, void *msg, long *result)
{
#ifdef WIN32
	MSG *message = (MSG*)msg;
	switch (message->message)
	{
	case 0x031E: //vista+ WM_DWMCOMPOSITIONCHANGED
	{
		//触发一次全部重绘
		this->update();
		break;
	}
	case WM_SYSCOMMAND:
	{
		if (message->wParam == SC_MAXIMIZE)
		{
			showMaximized();
			*result = true;
			return true;
		}
		if (message->wParam == SC_RESTORE)
		{
			if(isMaximized())
			{
				showMaximized();
			}
			else
			{
				showNormal();
			}
			*result = true;
			return true;
		}
		break;
	}
	}

	return __super::nativeEvent(eventType, message, result);
#else
	return false;
#endif
}

QRect QNoNCDialog::getDlgMaximumRt(bool bLocScreenRt)
{
	QRect rt = QApplication::desktop()->availableGeometry(this);
	if (bLocScreenRt)
	{
		QRect rtScreen = QApplication::desktop()->screenGeometry(this);
		rt.translate(-rtScreen.left(), -rtScreen.top());
	}

#ifdef WIN32
	//为系统任务栏留一个像素，解决鼠标无法弹出系统任务栏问题
	ShellTrayWndInfo trayInfo;
	GetShellTrayWndInfo(trayInfo);
	if (trayInfo.bAutoHide)
	{
		switch (trayInfo.type)
		{
		case DIRECTION_LEFT: rt.adjust(1, 0, 0, 0); break;
		case DIRECTION_TOP:  rt.adjust(0, 1, 0, 0); break;
		case DIRECTION_RIGHT: rt.adjust(0, 0, -1, 0); break;
		case DIRECTION_BOTTOM: rt.adjust(0, 0, 0, -1); break;
		}
	}
#endif

	return rt;
}

#ifdef MAC
void QNoNCDialog::setVisible(bool visible)
{
	QDialog::setVisible(visible);
	if (this->isWindow() && visible)
	{
		this->raise();
	}
}
#endif

bool QNoNCDialog::event(QEvent * evt)
{
	bool bRstl = QDialog::event(evt);
	if (evt->type() == QEvent::WindowActivate)
	{
#ifdef MAC
		if (!m_initTitle)
		{
			m_initTitle = true;
			unifiedNativeTitle(m_unifiedTitle);
		}
#endif
		QTimer::singleShot(30, this, SLOT(repaint()));//由于窗口设置了透明属性，在激活时，有时不会进行绘制，导致窗口出现假死现象
		emit this->s_WindowActiveChanged(true);
	}
	else if (evt->type() == QEvent::WindowDeactivate)
	{
		emit this->s_WindowActiveChanged(false);
	}
	else if (evt->type() == QEvent::Resize)
	{
		//onResize();
	}

	return bRstl;
}

void QNoNCDialog::showEvent(QShowEvent *event)
{
	QDialog::showEvent(event);

#ifdef LINUX
	if (!_wndNorRt.isValid())
	{
		_wndNorRt = this->normalGeometry();
	}
#endif
}


int QNoNCDialog::dlgBorderType()
{
	if (isMaximized() || isFullScreen())
		return BORDER_NONE;
	return m_borderType;
}

QMargins QNoNCDialog::getBorderMargins()
{
	return m_mainLay->contentsMargins();
}

int QNoNCDialog::getBorderWidth()
{
	return m_mainLay->contentsMargins().left();
}


void QNoNCDialog::paintEvent(QPaintEvent *event)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
