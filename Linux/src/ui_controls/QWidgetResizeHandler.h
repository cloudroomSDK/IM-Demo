#ifndef QWidgetResizeHandlerI_H
#define QWidgetResizeHandlerI_H

/*
1. 目标窗口支持顶层窗口，也支持子控件（在窗口内resize、move)；
2. resize时，不能小于minimumSizeHint及minimumSize；
*/

class QWidgetResizeHandlerI : public QObject
{
    Q_OBJECT

public:
    enum Action {
        Move			= 0x01,	//拖动
        Resize			= 0x02,	//调整大小, bord为QFrame.frameWidth, 非QFrame为8像素
		DbClkMaximize	= 0x04,	//双击最大化
    };

	enum { ALL_ACTIONS = Move | Resize | DbClkMaximize };

	explicit QWidgetResizeHandlerI(QWidget *parent, int enableActions = Move);
	void setEnableActions(int actions);
	int  getEnableActions() const { return _actions; }
	bool isActionEnable(Action ac) const { return (_actions & ac) == ac; }
	bool isButtonDown() const { return _buttonDown; }

    void doResize();
    void doMove();

protected:
    bool eventFilter(QObject *o, QEvent *e);
	bool mouseMoveEvent(QMouseEvent *e);
	bool keyPressEvent(QKeyEvent *e);

private:
    Q_DISABLE_COPY(QWidgetResizeHandlerI)

    enum MousePosition {
        Nowhere,
        TopLeft, BottomRight, BottomLeft, TopRight,
        Top, Bottom, Left, Right,
        Center
    };

	QWidget*	_widget{ nullptr };
    QPoint		_moveOffset;
    QPoint		_invertedMoveOffset;
	MousePosition _mode{ Nowhere };
	int	_actions{ 0 };
	int _bordWidth{ 0 };

	uint _buttonDown		: 1;
	uint _moveResizeMode	: 1;
	uint _noNcDlg			: 1;

    void setMouseCursor(MousePosition m);
    bool isMove() const { return _moveResizeMode && _mode == Center; }
    bool isResize() const { return _moveResizeMode && !isMove(); }

	bool isMaximized();
	bool isMinimized();
	void showMaximized();
	void showNormal();

};


#endif // QWidgetResizeHandlerI_H
