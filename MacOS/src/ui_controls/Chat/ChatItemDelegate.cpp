#include "stdafx.h"
#include "ChatItemDelegate.h"
#include "ChatWidget.h"

ChatItemDelegate::ChatItemDelegate(QObject *parent)
{
}

void ChatItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	if (option.rect.isNull() || option.rect.isEmpty() || !option.rect.isValid())
	{
		return;
	}
	if (index.data().canConvert<ChatItemObject*>())
	{
 		ChatItemObject* itemObj = index.data().value<ChatItemObject*>();
		if (itemObj == NULL)
		{
			return;
		}
  		painter->save();
  		//painter->translate(option.rect.topLeft());
		itemObj->draw(painter, option.rect);
		painter->restore();
	}
}

//----------------------------------------------------------------------------------------------------------------------
QSize ChatItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	if (index.data(DOC_HEIGHT).isValid())
	{
		QSize sz = QSize(option.rect.width(), index.data(DOC_HEIGHT).toInt());
		return sz;
	}
	return QStyledItemDelegate::sizeHint(option, index);
}

//----------------------------------------------------------------------------------------------------------------------
bool ChatItemDelegate::editorEvent(QEvent* event, QAbstractItemModel*, const QStyleOptionViewItem& option, const QModelIndex& index)
{
	if (index.data().canConvert<ChatItemObject*>())
	{
		ChatItemObject* itemObj = index.data().value<ChatItemObject*>();
		if (itemObj == NULL)
		{
			return false;
		}
	
		//鼠标点击
		if (QEvent::MouseButtonPress == event->type()|| QEvent::MouseButtonRelease == event->type())
		{
			QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
			itemObj->onMouseButtonEvent(mouseEvent);
		}
		else if (QEvent::MouseMove == event->type())//鼠标移动
		{
			QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
			QCursor cursor;
			itemObj->onMouseMoveEvent(mouseEvent, cursor);

			const ChatWidget* chatWidget = static_cast<const ChatWidget*>(option.widget);
			if (chatWidget != NULL)
			{
				chatWidget->viewport()->setCursor(cursor);
			}
		}
	}
 	return false;
}
