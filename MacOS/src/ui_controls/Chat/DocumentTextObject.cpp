#include "stdafx.h"
#include "DocumentTextObject.h"

QSizeF DocumentTextObject::intrinsicSize(QTextDocument * doc, int /*posInDocument*/, const QTextFormat &format)
{
	QString txt = format.property(Text).toString();
	QFontMetrics fm(doc->defaultFont());
	int w = fm.width(txt);
	int h = fm.height();
	return QSizeF(w, h);
}

void DocumentTextObject::drawObject(QPainter *painter, const QRectF &rect,
                               QTextDocument *doc, int posInDocument,
                               const QTextFormat &format)
{
	QFont font = doc->defaultFont();
	painter->setFont(font);
	QFontMetrics fm(font);
	QRectF rt = rect;
	rt.moveTop(rt.top() + fm.descent());//需要偏移像素才能对齐
	QString txt = qvariant_cast<QString>(format.property(Text));
	if (format.property(Color).isValid())
	{
		QColor c = qvariant_cast<QColor>(format.property(Color));
		painter->setPen(c);
	}
    QTextCursor cursor = m_textEdit->textCursor();
    bool bSel = cursor.hasSelection() && posInDocument >= cursor.selectionStart() && posInDocument < cursor.selectionEnd();
	if (bSel)
	{
		//painter->setPen(Qt::white);
	}
    QTextOption option(Qt::AlignLeft| Qt::AlignCenter);
    painter->drawText(rt, txt, option);
}

