#ifndef CHATITEMOBJECT_FILE_H
#define CHATITEMOBJECT_FILE_H

#include "ChatItemObject.h"
class ChatItemObject_File : public ChatItemObject
{
    Q_OBJECT
public:
	explicit ChatItemObject_File(const ChatStyle &style, QObject *parent = Q_NULLPTR);
	virtual ~ChatItemObject_File();

	ChatItemObject::UIType getUIType() override{ return UT_FILE; }
	void setMsgInfo(const UIMsgInfo &info) override;
	ChatItemObject::SelectectObj getSelectedObject(bool bGetAllText) override;

public:
	void onMouseButtonEvent(QMouseEvent *mouseEvent) override;

protected:
	void updateContentSize(int contentMaxWidth) override { ; }
	QSize getContentSize() override{ return QSize(ScreenScale(220), ScreenScale(100)); }
	void drawContent(QPainter *p, const QRect &rect) override;

protected:
	void updateFileState(UIFileState state, float progress = 0.0);
	QString getFilePathName();
	QString getFileName();
	QString getFileSize();
	QPixmap getFileTypePic(const QSize &sz);
	QString getFileStateDesc();
	QString getFileProgressDesc();
	void drawFileName(QPainter *p, const QRect &rt);
	void drawSubText(QPainter *p, const QRect &rt, const QString &text, int index, int len, const QColor &color);

private:
	UIFileState m_fileState;
	float m_progress;
};

#endif // CHATITEMOBJECT_FILE_H
