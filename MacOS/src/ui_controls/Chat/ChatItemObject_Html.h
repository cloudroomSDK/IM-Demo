#ifndef CHATITEMOBJECT_HTML_H
#define CHATITEMOBJECT_HTML_H

#include "ChatItemObject.h"
#include "DocumentHighLighter.h"
class ChatItemObject_Html : public ChatItemObject
{
    Q_OBJECT
public:
	explicit ChatItemObject_Html(const ChatStyle &style, QObject *parent = Q_NULLPTR);
	virtual ~ChatItemObject_Html();

	ChatItemObject::UIType getUIType() override { return UT_HTML; }
	void updateStyle() override;

	void setMsgInfo(const UIMsgInfo &info) override;
	void updateResource(const QUrl &imgUrl) override;
	void setVisible(bool bVisible) override;
	ChatItemObject::SelectectObj getSelectedObject(bool bGetAllText) override;

public:
	void selectionStart(const QPoint &pos) override;
	void selectionMove(const QPoint &pos, bool bDownDirection) override;
	void selectionEnd(bool bDownDirection) override;
	void selectionAll() override;
	void selectionClear() override;
	bool hasSelection() const override;
	void onMouseMoveEvent(QMouseEvent *mouseEvent, QCursor &cursor) override;
	void onMouseButtonEvent(QMouseEvent *mouseEvent) override;

protected:
	void updateContentSize(int contentMaxWidth) override;
	QSize getContentSize() override { return m_contentSize; }
	void drawContent(QPainter *p, const QRect &rect) override;

protected:
	int cursorFromPos(const QPoint &pos, bool fuzzy = true) const;
	int getSelectionStart() const;
	int getSelectionEnd() const;
	void getSelectedText(bool bSelectAll, QString &rsltHtml, QString &rsltText);

	QString getAnchorText(const QPoint &pos);
	QString getImageName(const QPoint &pos);
	CursorPosType getCursorType(const QPoint &pos, QString &rslt);
	void updateFileState(const UIMsgInfo &info);
	QImage getDefaultImage(const QSize &imgSize);

protected slots:
	void slot_gifAnimation(int);

protected:
	QTextDocument *m_doc;
	DocumentHighLighter *m_docHighLighter;
	QPoint m_curOffsetPt;
	int m_docPosition;

	QSize m_contentSize;
	int m_selectionStart;
	int m_selectionEnd;
	UIFileState m_imgState;
	float m_imgProgress;
	QMap<QUrl, QMovie*> m_gifMovies;
};

#endif // ChatItemObject_Html_H
