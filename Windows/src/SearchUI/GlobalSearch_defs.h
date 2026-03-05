#ifndef GlobalSearch_defs_h
#define GlobalSearch_defs_h


#define ITEMHI 56
#define ITEMTitleHI 30
#define DefaultItemCount	2

//////////////////////////////////////////////////////////////////////////
class GlobalSearchRsltItem : public QListWidgetItem
{
public:
	enum ItemType
	{
		FriendsTitle = 0,
		FriendsItem,
		FriendsExpand,

		GroupsTitle,
		GroupsItem,
		GroupsExpand,

		MsgTitle,
		MsgItem,
		MsgExpand,
		MsgLoading,

		ItemType_BUTT
	};

	GlobalSearchRsltItem(QListWidget *parent, ItemType type) : QListWidgetItem(parent) { _itemType = type; _matchIndex = -1; _matchStrLen = 0xFFFF; }
	~GlobalSearchRsltItem() {}

	bool isFixedItem() { return ((_itemType != FriendsItem) &&  (_itemType != GroupsItem) && (_itemType != MsgItem)); }

	bool isTitleItem() { return (_itemType == FriendsTitle || _itemType == GroupsTitle || _itemType == MsgTitle); }
	bool isExpandItem() { return (_itemType == FriendsExpand  || _itemType == GroupsExpand || _itemType == MsgExpand); }
	bool isDatItem() { return (_itemType == FriendsItem  || _itemType == GroupsItem || _itemType == MsgItem); }

	ItemType	_itemType;
	int			_matchIndex;
	int			_matchStrLen;
	QString		_ID;
	QString		_row1;
	QString		_row2;

	virtual bool operator<(const QListWidgetItem &other) const;
};

class GlobalSearchExpandItem : public GlobalSearchRsltItem
{
public:
	GlobalSearchExpandItem(QListWidget *parent, ItemType type) : GlobalSearchRsltItem(parent, type) { _totalCount = 0; _bExpand = false; }
	~GlobalSearchExpandItem() {}

	int _totalCount;
	bool _bExpand;
};

class GlobalSearchConvsationItem : public GlobalSearchRsltItem
{
public:
	GlobalSearchConvsationItem(QListWidget *parent) : GlobalSearchRsltItem(parent, MsgItem) { _rslt.messageCount = -1; }
	~GlobalSearchConvsationItem() {}

	CRIMSearchResultItem _rslt;
};



//////////////////////////////////////////////////////////////////////////
class GlobalSearchRsltItemDelegate : public QStyledItemDelegate
{
public:
	GlobalSearchRsltItemDelegate(QListWidget *pList);
	~GlobalSearchRsltItemDelegate();

	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;

	QLabel* _tmpLabel;
	QListWidget *_pList;
};


#endif // GlobalSearch_defs_h
