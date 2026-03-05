#ifndef CHATSTYLE_H
#define CHATSTYLE_H

enum CursorPosType
{
	CURSOR_NULL,	//无
	CURSOR_ANCHOR,	//超链接类型
	CURSOR_IMAGE,	//图片类型
	CURSOR_FILE,	//文件类型
	CURSOR_VIDEO,	//视频类型
};
class ChatStyle : public QObject
{
	Q_OBJECT

public:
	enum FontStyle
	{
		Big,
		BigBold,
		Medium,
		MediumBold,
		Small,
		SmallBold
	};
	enum ColorStyle
	{
		//内容区域
		ContentColor,	//内容默认的文本颜色		
		ContentBkColor,	//内容默认的背景颜色（主要应用在靠左摆放的消息）		
		ContentColor2,	//内容的文本颜色（主要应用在文件类型的描述区域）		
		ContentBkColor2,	//内容的背景颜色（主要应用在靠右摆放的消息）

		//状态区域
		StateColor,		//状态区域文本颜色
		StateColor2,	//状态区域文本颜色（主要应用在倒计时）

		//名字
		NameColor,	

		//时间
		TimeColor,
		TimeBkColor,

		//通知消息
		SysMsgColor,
		SysMsgBkColor,

		//选中效果
		SelectColor,
		SelectBkColor,

		//高亮颜色
		HighLighterColor,
	};
	//视图模式
	enum ChatViewMode
	{
		CHAT_BUBBLE = 0,	//气泡模式
		CHAT_LIST,		//列表模式
	};
	//资源类型
	enum RESOURCE_TYPE
	{
		RESTYPE_FACE = 0,	//表情
		RESTYPE_IMAGE,		//图片
		RESTYPE_VIDEO,		//视频
	};
public:
	ChatStyle();
	~ChatStyle(){ ; }
	ChatStyle(ChatStyle& settings) = delete;
	ChatStyle& operator=(const ChatStyle&) = delete;

	void setViewMode(ChatStyle::ChatViewMode mode);
	ChatStyle::ChatViewMode getViewMode() const;

	void setFontStyle(ChatStyle::FontStyle style, const QFont &f);
	const QFont &getFontStyle(ChatStyle::FontStyle style) const;

	void setColorStyle(ChatStyle::ColorStyle style, const QColor &c);
	const QColor &getColorStyle(ChatStyle::ColorStyle style) const;

	const QPixmap &getSendFailedPic() const { return m_imgSendFailed; }
	const QMovie *getWaittingObj() const { return m_waittingMovie; }
	QPixmap getSvgRes(const QString &fileName, QSize &sz)const;

	//获取资源最大显示大小
	QSize getResourceMaxSize(RESOURCE_TYPE type)  const;
protected:
	QFont appFont(int pixelSize, int weight);
	void updateRes();
public:
	QMap<FontStyle, QFont> m_fontStyle;
	QMap<ColorStyle, QColor> m_colorStyle;
	QPixmap m_imgSendFailed;
	QMovie* m_waittingMovie;
	ChatStyle::ChatViewMode m_viewMode;
};

#endif // MYLISTWIDGET_H
