#include "stdafx.h"
#include "CRTxtEditor.h"

CRTxtEditor::CRTxtEditor(QWidget *parent) : QLineEdit(parent)
{
	connect(this, &QLineEdit::textChanged, this, &CRTxtEditor::slot_textChanged);
	this->setReadOnly(true);
}

void CRTxtEditor::setFocusInAutoStartEdit(bool bAutoStart)
{
	m_bFocusInAutoStart = bAutoStart;
}

void CRTxtEditor::setAutoWidth(bool bAutoWidth)
{
	m_bAutoWidth = bAutoWidth;
	updateGeometry();
}

QSize CRTxtEditor::sizeHint() const
{
	if (m_bAutoWidth)
	{
		const QFontMetrics fm(fontMetrics());
		QMargins txtMgs = this->textMargins();
		QSize sz = QSize(fm.width(this->text()) + txtMgs.left() + txtMgs.right() + 6, fm.height());
		return sz;
	}
	return QLineEdit::sizeHint();
}

QSize CRTxtEditor::minimumSizeHint() const
{
	if (m_bAutoWidth)
	{
		return QSize(6, 1);
	}
	return QLineEdit::minimumSizeHint();
}

void CRTxtEditor::slot_textChanged()
{
	this->updateGeometry();
}

void CRTxtEditor::startEditWithText(const QString &text)
{
	this->setText(text);
	startEdit();
}

void CRTxtEditor::startEdit()
{
	m_orgText = this->text();
	this->setReadOnly(false);
	this->setFocus();
	//this->selectAll();
	this->grabMouse();

	//因为捕获了鼠标事件， 之前的窗口将获取不到离开事件，
	//很可能导致显示异常，在此主动向其发送离开消息；
	QObject *pObj = sender();
	if (pObj != nullptr && pObj->isWidgetType())
	{
		QWidget *pWidget = static_cast<QWidget*>(pObj);
		if (pWidget->testAttribute(Qt::WA_UnderMouse))
		{
			QEvent evt(QEvent::Leave);
			qApp->sendEvent(pObj, &evt);
		}
	}
}


void CRTxtEditor::keyPressEvent(QKeyEvent *event)
{
	QLineEdit::keyPressEvent(event);
	if (event->key() == Qt::Key_Cancel || event->key() == Qt::Key_Escape)
	{
		cancelEdit();
		return;
	}
	if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
	{
		onCommit();
		return;
	}
}

void CRTxtEditor::mousePressEvent(QMouseEvent *event)
{
	QLineEdit::mousePressEvent(event);
	if (this->isReadOnly())
	{
		if (this->hasFocus() && m_bFocusInAutoStart)
		{
			startEdit();
		}
	}
	else
	{
		QPoint pt = event->pos();
		if (this->rect().contains(pt))
			return;

		onCommit();
	}
}

void CRTxtEditor::focusInEvent(QFocusEvent *event)
{
	if (m_bFocusInAutoStart)
	{
		startEdit();
	}
	QLineEdit::focusInEvent(event);
}


void CRTxtEditor::focusOutEvent(QFocusEvent *event)
{
	QLineEdit::focusOutEvent(event);
	onCommit();
}

void CRTxtEditor::cancelEdit()
{
	this->releaseMouse();
	if (!this->isReadOnly())
	{
		this->setReadOnly(true);
		this->setText(m_orgText);
		emit s_canceled();
	}
}

void CRTxtEditor::onCommit()
{
	this->releaseMouse();
	if (!this->isReadOnly())
	{
		this->setReadOnly(true);
		emit s_commit(m_orgText, this->text());
	}
}
