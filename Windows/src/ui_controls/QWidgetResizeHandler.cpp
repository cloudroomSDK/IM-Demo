#include "stdafx.h"
#include "QWidgetResizeHandler.h"

#define BORDER_WIDTH 8

static bool resizeHorizontalDirectionFixed = false;
static bool resizeVerticalDirectionFixed = false;



QWidgetResizeHandlerI::QWidgetResizeHandlerI(QWidget *parent, int enableActions)
	: QObject(parent), _widget(parent), _buttonDown(false), _moveResizeMode(false), _noNcDlg(false)
{
	_actions = enableActions;
    _mode = Nowhere;
    _widget->setMouseTracking(true);

    QFrame *frame = qobject_cast<QFrame*>(_widget);
    _bordWidth = frame ? frame->frameWidth() : BORDER_WIDTH;
    _bordWidth = qMax(BORDER_WIDTH, _bordWidth);

#ifdef QNoNCDialog_H
	QNoNCDialog *dlg = dynamic_cast<QNoNCDialog*>(_widget);
	_noNcDlg = dlg != nullptr;
#endif

    _widget->installEventFilter(this);
}

bool QWidgetResizeHandlerI::isMaximized()
{
#ifdef QNoNCDialog_H
	if (_noNcDlg)
	{
		QNoNCDialog *dlg = static_cast<QNoNCDialog*>(_widget);
		return dlg->isMaximized();
	}
#endif
	return _widget->isMaximized();
}

bool QWidgetResizeHandlerI::isMinimized()
{
#ifdef QNoNCDialog_H
	if (_noNcDlg)
	{
		QNoNCDialog *dlg = static_cast<QNoNCDialog*>(_widget);
		return dlg->isMinimized();
	}
#endif
	return _widget->isMinimized();
}
void QWidgetResizeHandlerI::showMaximized()
{
#ifdef QNoNCDialog_H
	if (_noNcDlg)
	{
		QNoNCDialog *dlg = static_cast<QNoNCDialog*>(_widget);
		return dlg->showMaximized();
	}
#endif
	return _widget->showMaximized();
}
void QWidgetResizeHandlerI::showNormal()
{
#ifdef QNoNCDialog_H
	if (_noNcDlg)
	{
		QNoNCDialog *dlg = static_cast<QNoNCDialog*>(_widget);
		return dlg->showNormal();
	}
#endif
	return _widget->showNormal();
}

void QWidgetResizeHandlerI::setEnableActions(int actions)
{
	_actions = actions;
	if (_actions == 0)
	{
		setMouseCursor(Nowhere);
	}
}


bool QWidgetResizeHandlerI::eventFilter(QObject *o, QEvent *evt)
{
	if (_actions == 0)
	{
		return false;
	}

	if (evt->type() != QEvent::MouseButtonPress
		&& evt->type() != QEvent::MouseButtonRelease
		&& evt->type() != QEvent::MouseButtonDblClick
		&& evt->type() != QEvent::MouseMove
		&& evt->type() != QEvent::KeyPress
		&& evt->type() != QEvent::ShortcutOverride)
	{
		return false;
	}

    Q_ASSERT(o == _widget);
    if (QApplication::activePopupWidget()) 
	{
        if (_buttonDown && evt->type() == QEvent::MouseButtonRelease)
            _buttonDown = false;
        return false;
    }

	QMouseEvent *mouseEvt = (QMouseEvent*)evt;
	switch (mouseEvt->type())
	{
	case QEvent::MouseButtonPress:
		{
			if (mouseEvt->button() != Qt::LeftButton)
				return false;
			if (isMaximized() && !isActionEnable(DbClkMaximize))
				return false;
			if (!_widget->rect().contains(_widget->mapFromGlobal(mouseEvt->globalPos())))
				return false;
#if defined(Q_WS_X11)
			/*
			Implicit grabs do not stop the X server from changing
			the cursor in children, which looks *really* bad when
			doing resizingk, so we grab the cursor. Note that we do
			not do this on Windows since double clicks are lost due
			to the grab (see change 198463).
			*/
			if (mouseEvt->spontaneous())
#  if !defined(QT_NO_CURSOR)
				_widget->grabMouse(_widget->cursor());
#  else
				_widget->grabMouse();
#  endif // QT_NO_CURSOR
#endif // Q_WS_X11
			_buttonDown = false;
			mouseMoveEvent(mouseEvt);
			_buttonDown = true;
			_moveOffset = _widget->mapFromGlobal(mouseEvt->globalPos());
			_invertedMoveOffset = _widget->rect().bottomRight() - _moveOffset;
			return true;
		}break;

	case QEvent::MouseButtonRelease:
		{
			if (mouseEvt->button() != Qt::LeftButton)
				return false;
			if (isMaximized() && !isActionEnable(DbClkMaximize))
				return false;
			_moveResizeMode = false;
			_buttonDown = false;
			_widget->releaseMouse();
			_widget->releaseKeyboard();
			return true;
		}break;

	case QEvent::MouseMove:
		{
			_buttonDown = _buttonDown && (mouseEvt->buttons() & Qt::LeftButton); // safety, state machine broken!
			mouseMoveEvent(mouseEvt);
			return true;
		} break;

	case QEvent::MouseButtonDblClick:
		if (mouseEvt->button() == Qt::LeftButton && isActionEnable(DbClkMaximize))
		{
			if (isMaximized())
			{
				showNormal();
			}
			else
			{
				showMaximized();
			}
			return true;
		}break;

	case QEvent::KeyPress:
		if (keyPressEvent((QKeyEvent*)evt))
		{
			return true;
		}
		break;

	case QEvent::ShortcutOverride:
		if (_buttonDown) 
		{
			((QKeyEvent*)evt)->accept();
			return true;
		}break;

	default:
		break;
	}

    return false;
}

bool QWidgetResizeHandlerI::mouseMoveEvent(QMouseEvent *e)
{
	QPoint gPos = e->globalPos();
    QPoint pos = _widget->mapFromGlobal(gPos);
    if (!_moveResizeMode && !_buttonDown) 
	{
        if (pos.y() <= _bordWidth && pos.x() <= _bordWidth)
            _mode = TopLeft;
        else if (pos.y() >= _widget->height()-_bordWidth && pos.x() >= _widget->width()-_bordWidth)
            _mode = BottomRight;
        else if (pos.y() >= _widget->height()-_bordWidth && pos.x() <= _bordWidth)
            _mode = BottomLeft;
        else if (pos.y() <= _bordWidth && pos.x() >= _widget->width()-_bordWidth)
            _mode = TopRight;
        else if (pos.y() <= _bordWidth)
            _mode = Top;
        else if (pos.y() >= _widget->height()-_bordWidth)
            _mode = Bottom;
        else if (pos.x() <= _bordWidth)
            _mode = Left;
        else if ( pos.x() >= _widget->width()-_bordWidth)
            _mode = Right;
        else if (_widget->rect().contains(pos))
            _mode = Center;
        else
            _mode = Nowhere;

		if (isMinimized() || isMaximized() || !isActionEnable(Resize))
            _mode = Center;
#ifndef QT_NO_CURSOR
        setMouseCursor(_mode);
#endif
        return true;
    }

	if (_mode == Nowhere)
		return true;
	if (_mode == Center && !isActionEnable(Move))
		return true;
	if (_mode == Center && !_buttonDown && !_moveResizeMode)
		return true;

    if (_widget->testAttribute(Qt::WA_WState_ConfigPending))
		return true;

	if (isMaximized() && isActionEnable(DbClkMaximize))
	{
		QSize orgSz = _widget->size();
		showNormal();
		QSize newSz = _widget->size();

		int dlgXPos = 0;
		if (pos.x() < newSz.width() / 2) //拉动左侧
		{
			dlgXPos = gPos.x() - pos.x();
		}
		else if ((orgSz.width() - pos.x()) < newSz.width() / 2) //拉动右侧
		{
			dlgXPos = gPos.x() - (newSz.width() - (orgSz.width() - pos.x()));
		}
		else //中间
		{
			dlgXPos = gPos.x() - newSz.width() / 2;
		}

		int dlgYPos = 0;
		if (pos.y() < newSz.height() / 2) //拉动顶侧
		{
			dlgYPos = gPos.y() - pos.y();
		}
		else if ((orgSz.height() - pos.y()) < newSz.height() / 2) //拉动底侧
		{
			dlgYPos = gPos.y() - (newSz.height() - (orgSz.height() - pos.y()));
		}
		else //中间
		{
			dlgYPos = gPos.y() - newSz.height() / 2;
		}

		_widget->move(dlgXPos, dlgYPos);
		_moveOffset = _widget->mapFromGlobal(gPos);
		_invertedMoveOffset = _widget->rect().bottomRight() - _moveOffset;
		return true;
	}

    QPoint globalPos = (!_widget->isWindow() && _widget->parentWidget()) ?
                       _widget->parentWidget()->mapFromGlobal(e->globalPos()) : e->globalPos();
    if (!_widget->isWindow() && !_widget->parentWidget()->rect().contains(globalPos)) {
        if (globalPos.x() < 0)
            globalPos.rx() = 0;
        if (globalPos.y() < 0)
            globalPos.ry() = 0;
        if (globalPos.x() > _widget->parentWidget()->width())
            globalPos.rx() = _widget->parentWidget()->width();
        if (globalPos.y() > _widget->parentWidget()->height())
            globalPos.ry() = _widget->parentWidget()->height();
    }

    QPoint p = globalPos + _invertedMoveOffset;
    QPoint pp = globalPos - _moveOffset;

#ifdef Q_WS_X11
    // Workaround for window managers which refuse to move a tool window partially offscreen.
    QRect desktop = QApplication::desktop()->availableGeometry(_widget);
    pp.rx() = qMax(pp.x(), desktop.left());
    pp.ry() = qMax(pp.y(), desktop.top());
    p.rx() = qMin(p.x(), desktop.right());
    p.ry() = qMin(p.y(), desktop.bottom());
#endif

	QSize miniSzHint = _widget->minimumSizeHint();
	QSize minsize(qMax(_widget->minimumWidth(), miniSzHint.width()), qMax(_widget->minimumHeight(), miniSzHint.height()));
	QSize maxsize(_widget->maximumSize());

	QSize mpsize(_widget->geometry().right() - pp.x() + 1, _widget->geometry().bottom() - pp.y() + 1);
	mpsize = mpsize.expandedTo(minsize).boundedTo(maxsize);
    QPoint mp(_widget->geometry().right() - mpsize.width() + 1, _widget->geometry().bottom() - mpsize.height() + 1);
    QRect geom = _widget->geometry();

    switch (_mode) {
    case TopLeft:
        geom = QRect(mp, _widget->geometry().bottomRight()) ;
        break;
    case BottomRight:
        geom = QRect(_widget->geometry().topLeft(), p) ;
        break;
    case BottomLeft:
        geom = QRect(QPoint(mp.x(), _widget->geometry().y()), QPoint(_widget->geometry().right(), p.y())) ;
        break;
    case TopRight:
        geom = QRect(QPoint(_widget->geometry().x(), mp.y()), QPoint(p.x(), _widget->geometry().bottom())) ;
        break;
    case Top:
        geom = QRect(QPoint(_widget->geometry().left(), mp.y()), _widget->geometry().bottomRight()) ;
        break;
    case Bottom:
        geom = QRect(_widget->geometry().topLeft(), QPoint(_widget->geometry().right(), p.y())) ;
        break;
    case Left:
        geom = QRect(QPoint(mp.x(), _widget->geometry().top()), _widget->geometry().bottomRight()) ;
        break;
    case Right:
        geom = QRect(_widget->geometry().topLeft(), QPoint(p.x(), _widget->geometry().bottom())) ;
        break;
    case Center:
        geom.moveTopLeft(pp);
        break;
    default:
        break;
    }

    geom = QRect(geom.topLeft(), geom.size().expandedTo(minsize).boundedTo(maxsize));

    if (geom != _widget->geometry() && 
        (_widget->isWindow() || _widget->parentWidget()->rect().intersects(geom)))
	{
        if (_mode == Center)
            _widget->move(geom.topLeft());
        else
            _widget->setGeometry(geom);
    }

	return true;
}

void QWidgetResizeHandlerI::setMouseCursor(MousePosition m)
{
#ifdef QT_NO_CURSOR
    Q_UNUSED(m);
#else
    QObjectList children = _widget->children();
    for (int i = 0; i < children.size(); ++i) {
        if (QWidget *w = qobject_cast<QWidget*>(children.at(i))) {
            if (!w->testAttribute(Qt::WA_SetCursor) && !w->inherits("QWorkspaceTitleBar")) {
                w->setCursor(Qt::ArrowCursor);
            }
        }
    }

    switch (m) {
    case TopLeft:
    case BottomRight:
        _widget->setCursor(Qt::SizeFDiagCursor);
        break;
    case BottomLeft:
    case TopRight:
        _widget->setCursor(Qt::SizeBDiagCursor);
        break;
    case Top:
    case Bottom:
        _widget->setCursor(Qt::SizeVerCursor);
        break;
    case Left:
    case Right:
        _widget->setCursor(Qt::SizeHorCursor);
        break;
    default:
        _widget->setCursor(Qt::ArrowCursor);
        break;
    }
#endif // QT_NO_CURSOR
}

bool QWidgetResizeHandlerI::keyPressEvent(QKeyEvent * e)
{
    if (!isMove() && !isResize())
        return false;

    bool is_control = e->modifiers() & Qt::ControlModifier;
    int delta = is_control?1:8;
    QPoint pos = QCursor::pos();
    switch (e->key()) 
	{
    case Qt::Key_Left:
        pos.rx() -= delta;
        if (pos.x() <= QApplication::desktop()->geometry().left()) {
            if (_mode == TopLeft || _mode == BottomLeft) {
                _moveOffset.rx() += delta;
                _invertedMoveOffset.rx() += delta;
            } else {
                _moveOffset.rx() -= delta;
                _invertedMoveOffset.rx() -= delta;
            }
        }
        if (isResize() && !resizeHorizontalDirectionFixed) {
            resizeHorizontalDirectionFixed = true;
            if (_mode == BottomRight)
                _mode = BottomLeft;
            else if (_mode == TopRight)
                _mode = TopLeft;
#ifndef QT_NO_CURSOR
            setMouseCursor(_mode);
            _widget->grabMouse(_widget->cursor());
#else
            _widget->grabMouse();
#endif
        }
        break;

    case Qt::Key_Right:
        pos.rx() += delta;
        if (pos.x() >= QApplication::desktop()->geometry().right()) {
            if (_mode == TopRight || _mode == BottomRight) {
                _moveOffset.rx() += delta;
                _invertedMoveOffset.rx() += delta;
            } else {
                _moveOffset.rx() -= delta;
                _invertedMoveOffset.rx() -= delta;
            }
        }
        if (isResize() && !resizeHorizontalDirectionFixed) {
            resizeHorizontalDirectionFixed = true;
            if (_mode == BottomLeft)
                _mode = BottomRight;
            else if (_mode == TopLeft)
                _mode = TopRight;
#ifndef QT_NO_CURSOR
            setMouseCursor(_mode);
            _widget->grabMouse(_widget->cursor());
#else
            _widget->grabMouse();
#endif
        }
        break;

    case Qt::Key_Up:
        pos.ry() -= delta;
        if (pos.y() <= QApplication::desktop()->geometry().top()) {
            if (_mode == TopLeft || _mode == TopRight) {
                _moveOffset.ry() += delta;
                _invertedMoveOffset.ry() += delta;
            } else {
                _moveOffset.ry() -= delta;
                _invertedMoveOffset.ry() -= delta;
            }
        }
        if (isResize() && !resizeVerticalDirectionFixed) {
            resizeVerticalDirectionFixed = true;
            if (_mode == BottomLeft)
                _mode = TopLeft;
            else if (_mode == BottomRight)
                _mode = TopRight;
#ifndef QT_NO_CURSOR
            setMouseCursor(_mode);
            _widget->grabMouse(_widget->cursor());
#else
            _widget->grabMouse();
#endif
        }
        break;

    case Qt::Key_Down:
        pos.ry() += delta;
        if (pos.y() >= QApplication::desktop()->geometry().bottom()) {
            if (_mode == BottomLeft || _mode == BottomRight) {
                _moveOffset.ry() += delta;
                _invertedMoveOffset.ry() += delta;
            } else {
                _moveOffset.ry() -= delta;
                _invertedMoveOffset.ry() -= delta;
            }
        }
        if (isResize() && !resizeVerticalDirectionFixed) {
            resizeVerticalDirectionFixed = true;
            if (_mode == TopLeft)
                _mode = BottomLeft;
            else if (_mode == TopRight)
                _mode = BottomRight;
#ifndef QT_NO_CURSOR
            setMouseCursor(_mode);
            _widget->grabMouse(_widget->cursor());
#else
            _widget->grabMouse();
#endif
        }
        break;

    case Qt::Key_Space:
    case Qt::Key_Return:
    case Qt::Key_Enter:
    case Qt::Key_Escape:
        _widget->releaseMouse();
        _widget->releaseKeyboard();
        _moveResizeMode = false;
        _buttonDown = false;
        break;

    default:
        return false;
    }

    QCursor::setPos(pos);
	return true;
}


void QWidgetResizeHandlerI::doResize()
{
    if (!isActionEnable(Resize))
        return;

    _moveResizeMode = true;
    _moveOffset = _widget->mapFromGlobal(QCursor::pos());
    if (_moveOffset.x() < _widget->width()/2) {
        if (_moveOffset.y() < _widget->height()/2)
            _mode = TopLeft;
        else
            _mode = BottomLeft;
    } else {
        if (_moveOffset.y() < _widget->height()/2)
            _mode = TopRight;
        else
            _mode = BottomRight;
    }
    _invertedMoveOffset = _widget->rect().bottomRight() - _moveOffset;
#ifndef QT_NO_CURSOR
    setMouseCursor(_mode);
    _widget->grabMouse(_widget->cursor() );
#else
    _widget->grabMouse();
#endif
    _widget->grabKeyboard();
    resizeHorizontalDirectionFixed = false;
    resizeVerticalDirectionFixed = false;
}

void QWidgetResizeHandlerI::doMove()
{
    if (!isActionEnable(Move))
        return;

    _mode = Center;
    _moveResizeMode = true;
    _moveOffset = _widget->mapFromGlobal(QCursor::pos());
    _invertedMoveOffset = _widget->rect().bottomRight() - _moveOffset;
#ifndef QT_NO_CURSOR
    _widget->grabMouse(Qt::SizeAllCursor);
#else
    _widget->grabMouse();
#endif
    _widget->grabKeyboard();
}


