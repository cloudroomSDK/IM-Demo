#ifndef DOCUMENTTEXTOBJECT_H
#define DOCUMENTTEXTOBJECT_H

#include <QTextObjectInterface>

QT_BEGIN_NAMESPACE
class QTextDocument;
class QTextFormat;
class QPainter;
class QRectF;
class QSizeF;
class QTextEdit;
QT_END_NAMESPACE

class DocumentTextObject : public QObject, public QTextObjectInterface
{
    Q_OBJECT
    Q_INTERFACES(QTextObjectInterface)

public:
    DocumentTextObject(QTextEdit *textEdit){m_textEdit = textEdit;}
	static int getObjectType(){ return QTextFormat::UserObject + 1; }
	enum Properties 
	{
		TextUserProperty = QTextFormat::UserProperty,
		UUID,	//唯一ID，必须传
		Text, 
		Color, 

		UserPropertyBegin,
	};

public:        
    QSizeF intrinsicSize(QTextDocument *doc, int posInDocument,
                         const QTextFormat &format) override;
    void drawObject(QPainter *painter, const QRectF &rect, QTextDocument *doc,
                    int posInDocument, const QTextFormat &format) override;
private:
    QTextEdit *m_textEdit;
};

#endif
