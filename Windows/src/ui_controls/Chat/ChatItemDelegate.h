#ifndef CHATITEMDELEGATE_H
#define CHATITEMDELEGATE_H

#include <QStyledItemDelegate>
#include "ChatItemObject.h"

#define RIGHT_ALIGN (Qt::UserRole + 1)
#define DOC_HEIGHT (Qt::UserRole + 2)
class ChatItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
	ChatItemDelegate(QObject *parent = Q_NULLPTR);
	~ChatItemDelegate(){}

	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;
	QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;

protected:
	bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) Q_DECL_OVERRIDE;

private:
	// Disable copying
	ChatItemDelegate(const ChatItemDelegate&) = delete;
	ChatItemDelegate& operator=(const ChatItemDelegate&) = delete;
};

#endif // ChatItemDelegate_H
