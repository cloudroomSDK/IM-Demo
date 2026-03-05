#include "stdafx.h"
#include "GlobalSearch_defs.h"
#include "MessageMgr/MessageMgrPage.h"

bool GlobalSearchRsltItem::operator<(const QListWidgetItem &other) const
{
	const GlobalSearchRsltItem* item2 = static_cast<const GlobalSearchRsltItem*>(&other);
	if (_itemType < item2->_itemType)
		return true;
	if (_itemType > item2->_itemType)
		return false;

	if (quint32(_matchIndex) < quint32(item2->_matchIndex))
		return true;
	if (quint32(_matchIndex) > quint32(item2->_matchIndex))
		return false;
	return _matchStrLen < item2->_matchStrLen;
}

//////////////////////////////////////////////////////////////////////////
GlobalSearchRsltItemDelegate::GlobalSearchRsltItemDelegate(QListWidget *pList) : QStyledItemDelegate(pList), _pList(pList)
{
	_tmpLabel = new QLabel(NULL, Qt::FramelessWindowHint);
	_tmpLabel->setTextFormat(Qt::RichText);
}
GlobalSearchRsltItemDelegate::~GlobalSearchRsltItemDelegate()
{
	delete _tmpLabel;
}

void GlobalSearchRsltItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QStyledItemDelegate::paint(painter, option, index);

	GlobalSearchRsltItem *pItem = static_cast<GlobalSearchRsltItem*>(_pList->item(index.row()));
	if (pItem == NULL)
		return;

	static QSize iconSize = DEFUALT_ICONSZ;
	int space = (ITEMHI - iconSize.width()) / 2;

	if (!pItem->isFixedItem())
	{
		//绘制Icon
		QPixmap pix;
		if (pItem->_itemType == GlobalSearchRsltItem::FriendsItem)
		{
			pix = ImIconMgr::instance()->getUserIcon(pItem->_ID, iconSize);
		}
		if (pItem->_itemType == GlobalSearchRsltItem::GroupsItem)
		{
			pix = ImIconMgr::instance()->getGrpIcon(pItem->_ID, iconSize);
		}
		else
		{
			const CRIMConversationInfo* pConv = MessageMgrPage::instance()->getConversationInfo(pItem->_ID);
			if (pConv != NULL)
			{
				if (pConv->conversationType == CRIM_Conversation_1v1)
				{
					pix = ImIconMgr::instance()->getUserIcon(pConv->userID, pConv->faceURL, pConv->showName(), iconSize);
				}
				else
				{
					pix = ImIconMgr::instance()->getGrpIcon(pConv->groupID, pConv->faceURL, iconSize);
				}
			}
		}
		painter->drawPixmap(option.rect.left() + space, option.rect.top() + space, pix);

		painter->translate(QPoint());
		//绘制文本
		QRect txtRt(option.rect);
		txtRt.setLeft(txtRt.left() + space + iconSize.width() + space);
		if (pItem->_row2.length() > 0)
		{
			txtRt.setHeight(txtRt.height() / 2);
			_tmpLabel->setAlignment(Qt::AlignLeft | Qt::AlignBottom);
		}
		else
		{
			_tmpLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		}
		_tmpLabel->setFixedSize(txtRt.size());

		_tmpLabel->setText(pItem->_row1);
		_tmpLabel->render(painter, txtRt.topLeft(), QRegion(), QWidget::DrawChildren);

		if (pItem->_row2.length() > 0)
		{
			txtRt.moveTop(txtRt.bottom());
			_tmpLabel->setText(pItem->_row2);
			_tmpLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);
			_tmpLabel->render(painter, txtRt.topLeft(), QRegion(), QWidget::DrawChildren);
		}
	}
	else if (pItem->isTitleItem())
	{
		bool haveVisibleRow = false;
		for (int i = 0; i < index.row(); i++)
		{
			QListWidgetItem *pTmpItem = _pList->item(i);
			if (!pTmpItem->isHidden())
			{
				haveVisibleRow = true;
				break;
			}
		}
		if ( haveVisibleRow )
		{
			painter->setPen(QColor(237, 237, 237));
			painter->drawLine(option.rect.topLeft() + QPoint(2,0), option.rect.topRight());
		}
	}
	else if (pItem->isExpandItem())
	{
		painter->setPen(QColor(35, 132, 200));
		QRect txtRt(option.rect);
		txtRt.setLeft(txtRt.left() + space + 2);
		painter->drawText(txtRt, Qt::AlignLeft | Qt::AlignVCenter, pItem->_row1);
	}

}
