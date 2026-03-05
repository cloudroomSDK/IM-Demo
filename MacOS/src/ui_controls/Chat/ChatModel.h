#ifndef CHATMODEL_H
#define CHATMODEL_H

#include "ChatItemObject.h"

#define CONTENT_TOP_MARGIN ScreenScale(30)
#define DOC_HEIGHT (Qt::UserRole + 2)
class ChatWidget;
class ChatModel : public QAbstractItemModel
{
	Q_OBJECT

signals:
	void s_faceLeftClicked(const UIMsgInfo &msg);
	void s_faceRightClicked(const UIMsgInfo &msg);
	//exParams
	void s_contentRightClicked(int row, const UIMsgInfo &msg, const QVariantMap &exParams);
	void s_contentLeftClicked(int row, const UIMsgInfo &msg, const QVariantMap &exParams);
	void s_areaRightClicked();

public:
	ChatModel(ChatWidget *, QObject *parent = 0);
	~ChatModel();
	//更新样式
	void updateStyle();

	//插入一条数据
	void insertMsg(const UIMsgInfo &msg, bool insertFirst = false);
	void insertMsg(const QList<UIMsgInfo> &msgs, bool insertFirst = false);
	//删除内容
	void removeMsg(const QString &msgId);
	//更新内容
	void updateMsg(const UIMsgInfo &msg);

	const UIMsgInfo &getMsgInfo(int row) const;
	const UIMsgInfo &getMsgInfo(const QString &msgId) const;
	bool containsMsg(const QString &msgId) const;
	//更新大小
	void updateAllSizeHint();
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	QModelIndex getIndex(int row) const;
	QModelIndex getIndex(const QString &msgId) const;
	//复制内容
	void copyItem(QList<int> item);
	//删除前面n条数据
	void delFisrtItems(int n);
	
	//清空
	void clear();
	//设置顶部预留空间显示
	void setTopMarginVisible(bool bVisible);
	//更新资源
	void updateResource(const QString &msgId, const QUrl &resUrl);
	//设置当前显示的项
	void setItemsShow(int start, int end);
	//获取当前显示的项
	void getItemsShow(int &start, int &end);
	//获取第一个显示行号
	int getFisrtShow();
	//获取最后一个显示行号
	int getLastShow();

public:
	void selectionStart(int row, const QPoint &pos);
	void selectionMove(int row, const QPoint &pos, bool bDownDirection);
	void selectionEnd(int row, bool bDownDirection);
	void selectionAll(int row);
	void selectionClear(int row);

protected:
	QVariant data(const QModelIndex &index, int role) const override;
	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
	bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::DisplayRole) override;

	int columnCount(const QModelIndex &parent = QModelIndex()) const override;
	QModelIndex parent(const QModelIndex &index) const override;

	void insertRowData(int rowIndex, const QList<UIMsgInfo> &msgs);
	void removeRowData(int row);
	QSize getViewSize();
	void updateSizeHintData(int row);
	void updateTopPadding();
	void initFirstMsg();
	void insertMsg_inner(const UIMsgInfo &msg, int insertRowIndex);
	//删除内容
	void delItem(const QList<int> &item);
protected slots:
	void slot_itemFaceClicked(bool leftButton);
	void slot_itemContentClicked(bool leftButton, const QVariantMap &exParams);
	void slot_itemAreaClicked(bool leftButton);
	void slot_resendClicked();
	void slot_itemNeedUpdate();

private:
	ChatWidget *m_listView;
	QList<ChatItemObject*> m_items;
	QList<int> m_heightForRow;
	QHash<QString, ChatItemObject*> m_itemForMsgId;
	QDateTime m_lastTime;
	int m_firstMsgIndex;

	int m_itemStartShow;
	int m_itemEndShow;
};
//! [0]

#endif // ChatModel_H
