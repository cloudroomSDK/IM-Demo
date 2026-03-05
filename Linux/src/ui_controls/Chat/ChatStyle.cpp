#include "stdafx.h"
#include "ChatStyle.h"

ChatStyle::ChatStyle() : QObject()
{
	int defFontSize = ScreenScale(14);
	m_fontStyle = {
		{ Big, appFont(defFontSize + 2, QFont::Normal) },
		{ BigBold, appFont(defFontSize + 2, QFont::Bold) },
		{ Medium, appFont(defFontSize, QFont::Normal) },
		{ MediumBold, appFont(defFontSize, QFont::Bold) },
		{ Small, appFont(defFontSize - 2, QFont::Normal) },
		{ SmallBold, appFont(defFontSize - 2, QFont::Bold) }
	};
	m_colorStyle = {
		{ ContentColor, QColor(51, 51, 51) },
		{ ContentBkColor, QColor(255, 255, 255) },
		{ ContentColor2, QColor(153, 153, 153) },
		{ ContentBkColor2, QColor(214, 232, 255) },
		{ StateColor, QColor(153, 153, 153) },
		{ StateColor2, QColor(18, 183, 245) },
		{ NameColor, QColor(51, 51, 51) },
		{ TimeColor, QColor(153, 153, 153) },
		{ TimeBkColor, Qt::transparent },
		{ SysMsgColor, QColor(119, 119, 119) },
		{ SysMsgBkColor, QColor(0, 0, 0, 40) },
		{ SelectColor, Qt::white },
		{ SelectBkColor, QColor(51, 153, 255) },
		{ HighLighterColor, QColor(35, 132, 200) },
	};
	m_imgSendFailed = QPixmap(":/Res/IM/sendFailed.svg");
	m_waittingMovie = new QMovie(":/Res/IM/waitting.gif");
	m_waittingMovie->start();
	m_viewMode = CHAT_BUBBLE;

	updateRes();
}

void ChatStyle::setViewMode(ChatStyle::ChatViewMode mode)
{
	m_viewMode = mode;
}

ChatStyle::ChatViewMode ChatStyle::getViewMode() const
{
	return m_viewMode;
}

void ChatStyle::setFontStyle(ChatStyle::FontStyle style, const QFont &f)
{
	m_fontStyle[style] = f;
	if (style == ChatStyle::Medium)
	{
		updateRes(); 
	}
}

const QFont &ChatStyle::getFontStyle(ChatStyle::FontStyle style) const
{
	auto findIt = m_fontStyle.find(style);
	if (findIt == m_fontStyle.end())
	{
		static QFont f;
		return f;
	}
	return findIt.value();
}

void ChatStyle::setColorStyle(ColorStyle style, const QColor &c)
{
	m_colorStyle[style] = c;
}

const QColor &ChatStyle::getColorStyle(ColorStyle style) const
{
	auto findIt = m_colorStyle.find(style);
	if (findIt == m_colorStyle.end())
	{
		static QColor c;
		return c;
	}
	return findIt.value();
}

QFont ChatStyle::appFont(int pixelSize, int weight)
{
	QFont font;
	font.setFamily("Microsoft YaHei");
	font.setPixelSize(pixelSize);
	font.setWeight(weight);
	return font;
}

void ChatStyle::updateRes()
{
	QFontMetrics fm(getFontStyle(ChatStyle::Medium));
	int fontHeight_medium = fm.height();
	m_imgSendFailed = m_imgSendFailed.scaled(QSize(fontHeight_medium, fontHeight_medium), Qt::KeepAspectRatio, Qt::SmoothTransformation);
	m_waittingMovie->setScaledSize(QSize(fontHeight_medium, fontHeight_medium));
}


QPixmap ChatStyle::getSvgRes(const QString &fileName, QSize &sz) const
{
	if (!fileName.endsWith(".svg", Qt::CaseInsensitive) || sz.isEmpty())
		return QPixmap(fileName);

	static QSvgRenderer svgRender;
	svgRender.load(fileName);

	QSize newSize = svgRender.defaultSize();
	newSize.scale(sz, Qt::KeepAspectRatio);
	sz = newSize;

	QPixmap pixmap(sz);
	pixmap.fill(Qt::transparent);
	QPainter p(&pixmap);
	svgRender.render(&p);
	return pixmap;
}

QSize ChatStyle::getResourceMaxSize(RESOURCE_TYPE type) const
{
	if (type == RESTYPE_FACE)//表情
	{
		return QSize(ScreenScale(28), ScreenScale(28));
	}
	else if (type == RESTYPE_IMAGE)//图片
	{
		return QSize(ScreenScale(200), ScreenScale(200));
	}
	else if (type == RESTYPE_VIDEO)//视频
	{
		return QSize(ScreenScale(200), ScreenScale(200));
	}
	return QSize(0, 0);
}