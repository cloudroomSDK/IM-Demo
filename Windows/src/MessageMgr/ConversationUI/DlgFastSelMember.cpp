#include "stdafx.h"
#include "DlgFastSelMember.h"

#ifdef MAC
#define SEL_DLG_FLAGS Qt::Popup | Qt::FramelessWindowHint | Qt::WindowSystemMenuHint
#else
#define SEL_DLG_FLAGS Qt::Popup | Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::NoDropShadowWindowHint
#endif
DlgFastSelMember::DlgFastSelMember(QTextEdit *edit, QWidget *parent) : QNoNCDialog(parent, SEL_DLG_FLAGS)
{
	m_textEdit = edit;
	m_stringListmodel = new QStringListModel(this);
	m_pListWnd = new QListView(this);
	m_pListWnd->setObjectName("FindListWnd");
	m_pListWnd->setEditTriggers(QAbstractItemView::NoEditTriggers);
	m_pListWnd->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_pListWnd->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_pListWnd->setSelectionMode(QAbstractItemView::SingleSelection);
	this->setClientWidget(m_pListWnd);
	QObject::connect(m_pListWnd, SIGNAL(clicked(QModelIndex)), this, SLOT(clickedSlot(QModelIndex)));
	connect(this, SIGNAL(s_WindowActiveChanged(bool)), this, SLOT(slot_WindowActiveChanged(bool)));

	this->installEventFilter(this);
	m_pListWnd->installEventFilter(this);
	this->setFixedWidth(ScreenScale(160));
}

DlgFastSelMember::~DlgFastSelMember()
{
}

void DlgFastSelMember::insertName(const QString &name)
{
	m_stringList << name;
}

void DlgFastSelMember::removeName(const QString &name)
{
	m_stringList.removeAll(name);
}

void DlgFastSelMember::clear()
{
	m_stringList.clear();
}

void DlgFastSelMember::showFindWnd(const QString& text, const QPoint &pos)
{
	//效率较低,需要优化
	QStringList sl;
	foreach(const QString &word, m_stringList)
	{
		if (word.contains(text, Qt::CaseInsensitive))
		{
			sl << word;
		}
	}
	m_stringListmodel->setStringList(sl);
	m_pListWnd->setModel(m_stringListmodel);

	if (sl.size() > 0)
	{
		int totalH = sl.size()*m_pListWnd->sizeHintForRow(0);
		int h = qMin(totalH, ScreenScale(160));
		h = h + this->layout()->contentsMargins().top() + this->layout()->contentsMargins().bottom();
		this->setFixedHeight(h + 5);
	}
	
	this->setVisible(sl.size() > 0);
	this->move(pos.x(), pos.y() - this->height());
	if (this->isVisible())
	{
		QModelIndex index = m_pListWnd->model()->index(0, 0);
		m_pListWnd->setCurrentIndex(index);
		m_pListWnd->setFocus();
	}
}

void DlgFastSelMember::clickedSlot(QModelIndex modelIndex)
{
	QString selText = m_pListWnd->model()->data(modelIndex).toString();
	emit s_selFinished(selText);
	this->close();
}

void DlgFastSelMember::slot_WindowActiveChanged(bool b)
{
	if (!b)
	{
		this->close();
	}
}

bool DlgFastSelMember::eventFilter(QObject *o, QEvent *e)
{
	if (o == this)
	{
		QEvent::Type evt = e->type();
		if (evt == QEvent::KeyPress)
		{
			QKeyEvent *ke = static_cast<QKeyEvent *>(e);
			const int key = ke->key();

			switch (key)
			{
			case Qt::Key_Return:
			case Qt::Key_Enter:
			{
				QModelIndex curIndex = m_pListWnd->currentIndex();
				clickedSlot(curIndex);
				return true;
			}
			case Qt::Key_Escape:
			{
				this->close();
				return true;
			}
				break;
			}
		}//QEvent::KeyPress:
	}
	else if (o == m_pListWnd)
	{
		QEvent::Type evt = e->type();
		if (evt == QEvent::KeyPress || evt == QEvent::KeyRelease)
		{
			QKeyEvent *ke = static_cast<QKeyEvent *>(e);
			const int key = ke->key();
			if (key != Qt::Key_Down && key != Qt::Key_Up && key != Qt::Key_PageDown && key != Qt::Key_PageUp)
			{
				QApplication::sendEvent(m_textEdit, e);
				return true;
			}
		}
	}
	return false;
}
