#ifndef DOCUMENTHIGHLIGHTER_H
#define DOCUMENTHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>

QT_BEGIN_NAMESPACE
class QTextDocument;
QT_END_NAMESPACE

//! [0]
class DocumentHighLighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    DocumentHighLighter(QTextDocument *parent = 0);
	void setHighlighterText(const QString &text, const QColor &color);

protected:
    void highlightBlock(const QString &text) override;

private:
    struct HighlightingRule
    {
        QRegularExpression pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;
};
//! [0]

#endif // DOCUMENTHIGHLIGHTER_H
