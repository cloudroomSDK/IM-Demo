#include "stdafx.h"
#include "DocumentHighLighter.h"

DocumentHighLighter::DocumentHighLighter(QTextDocument *parent): QSyntaxHighlighter(parent)
{
	
}
void DocumentHighLighter::setHighlighterText(const QString &text, const QColor &color)
{
	highlightingRules.clear();
	if (!text.isEmpty())
	{
		HighlightingRule rule;
		rule.pattern = QRegularExpression(QString("%1").arg(text), QRegularExpression::CaseInsensitiveOption);
		rule.format.setForeground(color);
		highlightingRules.append(rule);
	}
}

void DocumentHighLighter::highlightBlock(const QString &text)
{
    for (auto &rule : highlightingRules) 
	{
		QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) 
		{
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
}
