#ifndef QNoNCDialog_H
#define QNoNCDialog_H


class QNoNCDialog;
class QWidgetResizeHandlerI;

class TitleBar : public QWidget
{
	Q_OBJECT

	Q_PROPERTY(int dlgCornerType READ dlgCornerType)

public:
	TitleBar(QWidget *parent, unsigned int sysBtnStyle = 0);
	virtual ~TitleBar(){}

	virtual void setIcon(const QPixmap &icon){}
	virtual void setTitleText(const QString &txt){}

	enum SYS_BTN_DEF
	{
		TBTN_CLOSE = 0x1,
		TBTN_MAX = 0x2,
		TBTN_MIN = 0x4,
		TBTN_NULL = 0x8,
	};

	enum { TBTN_ALL = TBTN_CLOSE | TBTN_MAX | TBTN_MIN };

	void setSysBtnStyle(unsigned int sysBtnStyle);
	unsigned int getSysBtnStyle() { return _sysBtnStyle; }
	QToolButton *getSysBtn(SYS_BTN_DEF type);

protected slots:
	void slot_dlgStateChanged() { resetBtns(); }

protected:
	void bindDialog(QNoNCDialog *pDlg);
	void bindTitleBarBtns(QToolButton *CloseBtn, QToolButton *MinBtn = NULL, QToolButton *RestoreBtn = NULL, QToolButton *MaxBtn = NULL);

	void resetBtns();
	int dlgCornerType();

	void mouseDoubleClickEvent(QMouseEvent *event) override;
	void paintEvent(QPaintEvent *event) override;

protected:
	unsigned int _sysBtnStyle;
	unsigned int _sysBtnStates;
	QNoNCDialog *_pDlg;
	QToolButton *_minBtn;
	QToolButton *_restoreBtn;
	QToolButton *_maxBtn;
	QToolButton *_closeBtn;
};

namespace Ui { class standardTitleBar; }
class StandardTitleBar : public TitleBar
{
public:
	StandardTitleBar(QNoNCDialog *parent, unsigned int sysBtnStyle);
	virtual ~StandardTitleBar();

public:
	virtual void setIcon(const QPixmap &icon);
	virtual void setTitleText(const QString &txt);

	void addWidgetBeforeIcon(QWidget *page);
	void addWidgetAfterTitle(QWidget *page);
	void setRightBtnsAlign(Qt::AlignmentFlag alignFlag);

private:
	Ui::standardTitleBar *ui;
	Qt::AlignmentFlag _alignFlag;
};

struct tagMSG;
typedef struct tagMSG MSG;
struct tagMINMAXINFO;
typedef struct tagMINMAXINFO MINMAXINFO;

#ifdef MAC
#define DEF_WINFLAGS (Qt::Dialog|Qt::WindowSystemMenuHint)
#elif defined WIN32
#define DEF_WINFLAGS (Qt::Dialog|Qt::FramelessWindowHint|Qt::WindowSystemMenuHint)
#else
#define DEF_WINFLAGS (Qt::Dialog|Qt::FramelessWindowHint)
#endif


class QNoNCDialog : public QDialog
{
	friend class TitleBar;
	Q_OBJECT

	Q_PROPERTY(int dlgCornerType READ dlgCornerType)	//DLG_TYPE
	Q_PROPERTY(int dlgBorderType READ dlgBorderType)

public:
	enum DLG_TYPE{//窗体类型
		DLG_ROUND = 0,	//圆角
		DLG_RECT,		//直角
		DLG_TYPE_BUT,
	};
	enum BORDER_TYPE{//边框类型
		BORDER_NONE = 0,	//无边框：全屏、最大化时无边框， mac下有原生框架无边框
		BORDER_LINE,		//1px边框 
		BORDER_SHADOW,		//阴影边框
	};
public:
	QNoNCDialog(QWidget *parent = 0, Qt::WindowFlags flags = DEF_WINFLAGS, DLG_TYPE type = DLG_ROUND, BORDER_TYPE borderType = BORDER_SHADOW);
	~QNoNCDialog();

	void cfgCloseType(bool bSupportESCClose = false)
	{
		m_bSupportESCClose = bSupportESCClose;
	}

	//不包含边框阴影部份
	QWidget *getDlgFrame() { return m_pFrame; }

	void setTitleBar(TitleBar *pBar, bool bRelayoutNow = true);
	TitleBar *titleBar(){ return m_titleBar; }
	virtual void setClientWidget(QWidget *clientWidget, bool bRelayoutNow = true);
	QWidget *clientWidget(){ return m_Client; }
	void setStatusBar(QWidget *statusDlg, bool bRelayoutNow = true);
	QWidget *statusBar(){ return m_Status; }

	void setWindowType(DLG_TYPE);

#ifdef MAC
	virtual void setVisible(bool visible);
	void unifiedNativeTitle(bool bUnified);
#endif

	bool isMinimized() const;
	bool isMaximized() const;
	bool isFullScreen() const;
	void SetFullScreen(bool bFullScreen);
	void setWindowState(Qt::WindowStates state);
	Qt::WindowStates windowState() const;

	QMargins getBorderMargins();
	int getBorderWidth();

public slots:
	void showMinimized();
	void showNormal();
	void showMaximized();
	void showFullScreen();

signals:
	void s_WindowActiveChanged(bool bActived);
	void s_WindowBorderChanged();
	void s_WindowStateChanged();

protected:
	bool event(QEvent *evt) override;
	void showEvent(QShowEvent *event) override;
	void paintEvent(QPaintEvent *event) override;

	void keyPressEvent(QKeyEvent *event) override
	{
		if (!m_bSupportESCClose && event->key() == Qt::Key_Escape)
			return;
		QDialog::keyPressEvent(event);
	}
	bool nativeEvent(const QByteArray &eventType, void *message, long *result) override;
	void changeEvent(QEvent *) override;
	virtual void reStyleSheet();
	virtual QRect getDlgMaximumRt(bool bLocScreenRt = false);

protected:
	int dlgCornerType() { return m_curDlgType; }
	virtual int dlgBorderType();
	QPixmap getDlgPixmap();
	void reLayout();
	void resetDialogFlags();
	void onWndStateChange(QWindowStateChangeEvent *evt);
	void onWindowTitleChange();
	void updateUIShadow();

protected:
	bool				m_bSupportESCClose{ true };

	QWidgetResizeHandlerI* m_reszHandler{ nullptr };
	QFrame				*m_pFrame{ nullptr };
	TitleBar			*m_titleBar{ nullptr };
	QWidget				*m_Client{ nullptr };
	QWidget				*m_Status{ nullptr };
	QVBoxLayout			*m_mainLay{ nullptr };

	DLG_TYPE			m_cfgDlgType{ DLG_ROUND };
	DLG_TYPE			m_curDlgType{ DLG_ROUND };
	BORDER_TYPE			m_borderType{ BORDER_SHADOW };

#ifdef MAC
	bool                m_bShowTitle{false};
	bool                m_initTitle{false};
	bool                m_unifiedTitle{false};
#endif

	//QWidget的WindowStates某些逻辑工作不正常，在此自已实现
	Qt::WindowStates _wndState = Qt::WindowNoState;
#ifdef LINUX
	QRect            _wndNorRt;
#endif
};


#endif // QNoNCDialog_H
