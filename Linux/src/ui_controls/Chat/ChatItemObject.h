#ifndef CHATITEMOBJECT_H
#define CHATITEMOBJECT_H

#include "ChatStyle.h"
class ChatItemObject : public QObject
{
	Q_OBJECT
signals:
	void s_faceClicked(bool leftButton);
	void s_contentClicked(bool leftButton, const QVariantMap &exParams = QVariantMap());
	void s_areaClicked(bool leftButton);
	void s_resendClicked();
	void s_needUpdate();

public:
	enum UIType
	{
		UT_NULL,
		UT_HTML,
		UT_FILE,
		UT_VIDEO,
		UT_AUDIO,

		UT_QUOTE,
	};
	static ChatItemObject *CreateItemObj(const UIMsgInfo &info, const ChatStyle &style);
	static UIType getUIType(const UIMsgInfo &info);
	virtual ~ChatItemObject();

public:
	//获取当前对象类型（文件、富文本等）
	virtual UIType getUIType() { return UT_NULL; }
	//更新样式
	virtual void updateStyle();

	//设置消息
	virtual void setMsgInfo(const UIMsgInfo &info);
	const UIMsgInfo &getMsgInfo() const { return m_msgInfo; }
	//更新资源
	virtual void updateResource(const QUrl &imgUrl){ ; }
	//更新大小
	void updateSize(int viewWidth);
	QSize getSize(){ return m_size; }
	//设置显示状态
	virtual void setVisible(bool bVisible);
	bool isVisible(){ return m_bVisible; }
	//绘制
	void draw(QPainter *p, const QRect &rect);

	//是否显示消息时间
	void showTime(bool bShowTime);

public:
	struct SelectectObj
	{
		QString text;
		QString html;
		QUrl	url;
		QString	image;
		bool isNull()
		{
			return text.isEmpty() && html.isEmpty() && url.isEmpty() && image.isEmpty();
		}
	};
	//选择文本相关接口
	virtual void selectionStart(const QPoint &pos);
	virtual void selectionMove(const QPoint &pos, bool bDownDirection);
	virtual void selectionEnd(bool bDownDirection);
	virtual void selectionAll();
	virtual void selectionClear();
	virtual bool hasSelection() const;
	virtual SelectectObj getSelectedObject(bool bGetAllText){ return SelectectObj(); }

	//事件接口
	virtual void onMouseMoveEvent(QMouseEvent *mouseEvent, QCursor &cursor);
	virtual void onMouseButtonEvent(QMouseEvent *mouseEvent);

protected:
	//更新内容区域大小
	virtual void updateContentSize(int maxWidth){ ; }
	//获取内容区域大小
	virtual QSize getContentSize(){ return QSize(0, 0); }
	//绘制内容区域
	virtual void drawContent(QPainter *p, const QRect &rect);

protected:
	ChatItemObject(const ChatStyle &style, QObject *parent = Q_NULLPTR);
	//选择文本相关接口
	void checkSelectStart(bool bDownDirection);
	bool updateSelection();
	//绘制相关
	void update();
	void updateDrawRect(const QRect &rect);
	void drawRing(QPainter *painter, const QRect &rt, float progress);
	void makeImageRoundBound(QImage &img, const QSize &contentSz);
	QMargins getPadding();
	Qt::AlignmentFlag getHAlignment();
	QString getTimeStr();
	bool isShowFace();
	bool isShowName();
	bool needDrawBubble();

protected slots:
	void slot_waittingAnimation(int);

protected:
	const ChatStyle &m_style;
	UIMsgInfo m_msgInfo;
	bool m_bVisible;

	//当前绘制区域
	QSize m_size;
	int m_fontHeight_medium;
	int m_fontHeight_big;
	int m_fontHeight_small;
	bool m_bShowTime;
	QRect m_faceRt;
	QRect m_nameRt;
	QRect m_contentRt;
	QRect m_timeRt;
	QRect m_stateRt;

	//内容区域
	bool m_bDragingOnClick;
	QPoint m_lastClickPos;
	QPoint m_selectionStart;
	QPoint m_selectionEnd;
	bool m_bSelected;
};


#endif // ChatItemObject_H
