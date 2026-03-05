#ifndef CHATINPUTEDIT_H
#define CHATINPUTEDIT_H

#include "DocumentTextObject.h"

class ChatInputEdit : public QTextBrowser
{
	Q_OBJECT
public:
	ChatInputEdit(QWidget *parent);
	~ChatInputEdit();

	enum CharFormatType
	{
		CFT_UNDEFINE,
		//QTextImageFormat
		CFT_IMG,
		CFT_FACE,
		CFT_FILE,
		//QTextCharFormat
		CFT_TEXT,
		CFT_CUSTOM_TEXT,
	};

	void setSkipTab(bool bSkip){ m_bKeySkip_Tab = bSkip; }
	void scaledResources();
	void replaceImg(const QString& strName, ChatInputEdit::CharFormatType type = CFT_IMG);
	void setCursorAtEnd();	//保证光标在最后
	void setClearEnable(bool b){m_clearEnable = b;}
	void setPlaceholderText(const QString& placeholderText);
	//void insertHtml(const QString &text);
	
	ChatInputEdit::CharFormatType getFormatType(const QTextCharFormat &charFormat) const;
	void insertImage(const QString &imagePathName);
	void insertImage(const QImage& image);
	void insertFile(const QString &filePathName);

public slots:
	virtual void customContextMenuRequestedSlot(const QPoint& pos);
	void saveImageAs();
	bool getTextCorsor(const QPoint& pos, QTextCursor &cursor);
	void slot_gifAnimation(int);
	void clear();

protected:
	virtual void mousePressEvent(QMouseEvent * e);
	virtual void mouseDoubleClickEvent(QMouseEvent * e);
	virtual QMimeData *createMimeDataFromSelection() const;
	virtual void insertFromMimeData(const QMimeData * source);
	virtual void mouseMoveEvent(QMouseEvent *e);
	virtual void resizeEvent (QResizeEvent * e);
	virtual void keyPressEvent(QKeyEvent *e); 
	virtual void focusInEvent(QFocusEvent *e);
	virtual void focusOutEvent(QFocusEvent *e);
	bool eventFilter(QObject *obj, QEvent *ev);

private:
	enum PROPERTY_FMT
	{
		PFMT_UUID = QTextFormat::UserProperty + 100,
		PFMT_ISFILE,//是否为文件
	};
	QRect getTxtRect();
	bool selImage(const QPoint& pos);
	bool canPaste();
	bool checkStringPicPath(const QString& txt);

	CharFormatType getFormatTypeFromPos(const QPoint& pos);
	void addGifToMap(const QString &fileName);
	QImage createFileImage(const QString& strFile);
	QMap<QString, ChatInputEdit::CharFormatType> getResources();

	QImage m_scaleImg;
	bool m_leftSelImg;
	bool m_clearEnable;
	QString m_placeholderText;
	bool m_bKeySkip_Tab;
	QTimer m_autoUpdateTimer;
	QMap<QString, QMovie*> m_urlMovieMap;
};

#endif // ChatInputEdit_H
