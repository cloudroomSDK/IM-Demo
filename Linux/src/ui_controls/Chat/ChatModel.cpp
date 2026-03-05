#include "stdafx.h"
#include "ChatModel.h"
#include "ChatWidget.h"

ChatModel::ChatModel(ChatWidget *view, QObject *parent)
	: QAbstractItemModel(parent)
{
	qRegisterMetaType<UIMsgInfo>("UIMsgInfo");
 	m_listView = view;
	clear();
}

ChatModel::~ChatModel()
{
	clear();
}

void ChatModel::updateStyle()
{
	for (auto pItem : m_items)
	{
		pItem->updateStyle();
	}
}

void ChatModel::setItemsShow(int start, int end)
{
	if (m_itemStartShow != -1 && m_itemEndShow != -1)
	{
		for (int row = m_itemStartShow; row <= m_itemEndShow && row < m_items.size(); row++)
		{
			ChatItemObject *itemObj = m_items.at(row);
			itemObj->setVisible(false);
		}
	}
	m_itemStartShow = start;
	m_itemEndShow = end;
	if (m_itemStartShow != -1 && m_itemEndShow != -1)
	{
		for (int row = m_itemStartShow; row <= m_itemEndShow; row++)
		{
			ChatItemObject *itemObj = m_items.at(row);
			itemObj->setVisible(true);
		}
	}
}

void ChatModel::getItemsShow(int &start, int &end)
{
	start = m_itemStartShow;
	end = m_itemEndShow;
}

int ChatModel::getFisrtShow()
{
	for (int i = 0; i < m_items.count(); i++)
	{
		if (!m_listView->isRowHidden(i))
		{
			return i;
		}
	}
	return -1;
}

int ChatModel::getLastShow()
{
	for (int i = m_items.count() - 1; i >= 0; i--)
	{
		if (!m_listView->isRowHidden(i))
		{
			return i;
		}
	}
	return -1;
}

//初始化第一个空白消息
void ChatModel::initFirstMsg()
{
	UIMsgInfo msg;
	msg.contentType = CRIM_MsgType_NULL;
	msg.clientMsgID = QUuid::createUuid().toString();
	msg.defHeightForNull = CONTENT_TOP_MARGIN;
	insertRowData(0, QList<UIMsgInfo>() << msg);
	m_firstMsgIndex = 1;
}

void ChatModel::setTopMarginVisible(bool bVisible)
{
	m_listView->setRowHidden(0, !bVisible);
}

void ChatModel::insertMsg(const UIMsgInfo &msg, bool insertFirst)
{
	int row = insertFirst ? m_firstMsgIndex : rowCount();
	//创建itemObject
	insertRowData(row, QList<UIMsgInfo>() << msg);
}

void ChatModel::insertMsg(const QList<UIMsgInfo> &msgs, bool insertFirst)
{
	int row = insertFirst ? m_firstMsgIndex : rowCount();
	//创建itemObject
	insertRowData(row, msgs);
}

void ChatModel::removeMsg(const QString &msgId)
{
	ChatItemObject *itemObj = m_itemForMsgId.value(msgId);
	if (itemObj == NULL)
	{
		return;
	}
	int row = m_items.indexOf(itemObj);
	if (row < 0)
	{
		return;
	}

	QList<int> delIndexs;
	delIndexs.append(row);
	delItem(delIndexs);
}

void ChatModel::updateMsg(const UIMsgInfo &msg)
{
	ChatItemObject *itemObj = m_itemForMsgId.value(msg.getMsgID());
	if (itemObj == NULL)
	{
		return;
	}
	int row = m_items.indexOf(itemObj);
	if (row < 0)
	{
		return;
	}

	//类型改变需要重新创建
	if (ChatItemObject::getUIType(msg) != itemObj->getUIType())
	{
		removeRowData(row);
		insertRowData(row, QList<UIMsgInfo>() << msg);
	}
	else
	{
		QModelIndex index = this->index(row, 0);
		//设置内容，通知更新
		itemObj->setMsgInfo(msg);
		setData(index, QVariant());
		//设置高度，通知更新
		if (!getViewSize().isEmpty())
		{
			updateSizeHintData(row);
		}
	}
}

void ChatModel::updateResource(const QString &msgId, const QUrl &resUrl)
{
	ChatItemObject *itemObj = m_itemForMsgId.value(msgId);
	if (itemObj == NULL)
	{
		return;
	}
	itemObj->updateResource(resUrl);
}

const UIMsgInfo &ChatModel::getMsgInfo(int row) const
{
	if (row <0 || row >= m_items.count())
	{
		static UIMsgInfo info;
		return info;
	}
	ChatItemObject *itemObj = m_items.at(row);
	return itemObj->getMsgInfo();
}

const UIMsgInfo &ChatModel::getMsgInfo(const QString &msgId) const
{
	ChatItemObject *itemObj = m_itemForMsgId.value(msgId);
	if (itemObj == NULL)
	{
		static UIMsgInfo info;
		return info;

	}
	return itemObj->getMsgInfo();
}

bool ChatModel::containsMsg(const QString &msgId) const
{
	return m_itemForMsgId.contains(msgId);
}

QSize ChatModel::getViewSize()
{
	return m_listView->viewport()->size();
}

void ChatModel::updateAllSizeHint()
{
	for (int i = 0; i < m_items.count(); i++)
	{
		updateSizeHintData(i);
	}
}

void ChatModel::updateSizeHintData(int row)
{
	int viewWidth = getViewSize().width();
	ChatItemObject *itemObj = m_items.at(row);
	itemObj->updateSize(viewWidth);

	QModelIndex index = this->index(row, 0);
	setData(index, itemObj->getSize().height(), DOC_HEIGHT);
}

void ChatModel::copyItem(QList<int> item)
{
	if (item.size() <= 0)
	{
		return;
	}
	qSort(item.begin(), item.end());

	bool bMutliRow = item.size() > 1;
	bool bGetAllText = bMutliRow;//多行情况下，需要获取整行的内容
	bool bContainsFile = false;
	QString txt;
	QString html;
	QList<QUrl> urls;
	QString imagePath;
	for (auto row : item)
	{
		ChatItemObject *itemObj = m_items.at(row);
		const UIMsgInfo &info = itemObj->getMsgInfo();
		if (info.contentType == CRIM_MsgType_NULL)
		{
			continue;
		}
		//空对象可能是没有选中或者不支持的消息类型，需要过滤
		ChatItemObject::SelectectObj selectedObj = itemObj->getSelectedObject(bGetAllText);
		if (selectedObj.isNull())
		{
			continue;
		}
		if (info.contentType == CRIM_MsgType_File)
		{
			bContainsFile = true;
		}
		//文本
		if (!selectedObj.text.isEmpty())
		{
			if (bMutliRow)//超过两行需要添加名字
			{
				txt += info.senderNickname + " " + info.getCreateTime().toString("yyyy/MM/dd hh:mm:ss");
				txt += "\n";
			}
			txt += selectedObj.text;
			txt += "\n\n";
		}

		//富文本
		if (!selectedObj.html.isEmpty())
		{
			if (bMutliRow)//超过两行需要添加名字
			{
				html += info.senderNickname + " " + info.getCreateTime().toString("yyyy/MM/dd hh:mm:ss");
				html += "<br/>";
			}
			html += selectedObj.html;
			html += "<br/><br/>";
		}
		//文件
		if (!selectedObj.url.isEmpty())
		{
			urls.append(selectedObj.url);
		}

		//图片
		if (!selectedObj.image.isNull())
		{
			imagePath = selectedObj.image;
		}
	}

	//单行、多行逻辑处理
	if (bMutliRow)
	{
		//多行下，不设置图片内容
		imagePath.clear();
		//多行下，不设置文件
		urls.clear();
		//多行下，如果包含文件，则不设置富文本
		if (bContainsFile)
		{
			html.clear();
		}
	}

	QApplication::clipboard()->clear();
	QMimeData *data = new QMimeData();
	//文件，text/uri-list
	if (urls.size() > 0)
	{
		data->setUrls(urls);
	}
	//图片
	if (!imagePath.isEmpty())
	{
		data->setImageData(QImage(imagePath));
	}
	//富文本
	if (!html.isEmpty())
	{
		html.remove(html.count() - 10, 10);//去除最后两个换行符号
		//text/html
		data->setHtml(html);
	}
	//文本
	if (!txt.isEmpty())
	{
		txt.remove(txt.count() - 2, 2);//去除最后两个换行符号
		//text/plain
		data->setText(txt);
	}
	QApplication::clipboard()->setMimeData(data);
}

void ChatModel::delItem(const QList<int> &item)
{
	QTime t;
	t.start();
	QList<int> delItems = item;
	qSort(delItems.begin(), delItems.end());
	for (auto it = delItems.rbegin(); it != delItems.rend(); it++)
	{
		removeRowData(*it);
	}

	for (int i = 0; i < m_items.count(); i++)
	{
		ChatItemObject *itemObj = m_items.at(i);
		QModelIndex index = this->index(i, 0);
		setData(index, itemObj->getSize().height(), DOC_HEIGHT);
	}
}

void ChatModel::delFisrtItems(int n)
{
	QList<int> delIndexs;
	for (int i = m_firstMsgIndex; i < n; i++)
	{
		delIndexs.append(i);
	}
	delItem(delIndexs);
}

void ChatModel::clear()
{
	m_firstMsgIndex = 0;
	m_itemStartShow = -1;
	m_itemEndShow = -1;

	beginResetModel();
	while (m_items.count() > 0)
	{
		int removeIndex = 0;
		delete m_items.takeAt(removeIndex);
		m_heightForRow.removeAt(removeIndex);
	}
	m_itemForMsgId.clear();
	initFirstMsg();
	endResetModel();
}

void ChatModel::selectionStart(int row, const QPoint &pos)
{
	if (row <0 || row >= m_items.count())
	{
		return;
	}
	ChatItemObject *itemObj = m_items.at(row);
	if (!itemObj->getMsgInfo().isNormalMsg())
	{
		return;
	}
	
	itemObj->selectionStart(pos);
	QModelIndex index = this->index(row, 0);
	emit(dataChanged(index, index));
}

void ChatModel::selectionMove(int row, const QPoint &pos, bool bDownDirection)
{
	if (row <0 || row >= m_items.count())
	{
		return;
	}
	ChatItemObject *itemObj = m_items.at(row);
	if (!itemObj->getMsgInfo().isNormalMsg())
	{
		return;
	}
	itemObj->selectionMove(pos, bDownDirection);
	QModelIndex index = this->index(row, 0);
	emit(dataChanged(index, index));
}

void ChatModel::selectionEnd(int row, bool bDownDirection)
{
	if (row <0 || row >= m_items.count())
	{
		return;
	}
	ChatItemObject *itemObj = m_items.at(row);
	if (!itemObj->getMsgInfo().isNormalMsg())
	{
		return;
	}
	itemObj->selectionEnd(bDownDirection);
	QModelIndex index = this->index(row, 0);
	emit(dataChanged(index, index));
}

void ChatModel::selectionAll(int row)
{
	if (row <0 || row >= m_items.count())
	{
		return;
	}
	ChatItemObject *itemObj = m_items.at(row);
	if (!itemObj->getMsgInfo().isNormalMsg())
	{
		return;
	}
	itemObj->selectionAll();
	QModelIndex index = this->index(row, 0);
	emit(dataChanged(index, index));
}

void ChatModel::selectionClear(int row)
{
	if (row <0 || row >= m_items.count())
	{
		return;
	}
	ChatItemObject *itemObj = m_items.at(row);
	if (!itemObj->getMsgInfo().isNormalMsg())
	{
		return;
	}
	itemObj->selectionClear();
	QModelIndex index = this->index(row, 0);
	emit(dataChanged(index, index));
}

QModelIndex ChatModel::getIndex(int row) const
{
	return index(row, 0);
}

QModelIndex ChatModel::getIndex(const QString &msgId) const
{
	ChatItemObject *itemObj = m_itemForMsgId.value(msgId);
	if (itemObj == NULL)
	{
		return QModelIndex();
	}
	int row = m_items.indexOf(itemObj);
	if (row < 0)
	{
		return QModelIndex();
	}
	return index(row, 0);
}
//! [1]
int ChatModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
	return m_items.size();
}

int ChatModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}
//! [1]

//! [6]
bool ChatModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if (index.isValid())
	{
		if (role == Qt::DisplayRole)
		{
			emit(dataChanged(index, index));
			return true;
		}
		else if (role == DOC_HEIGHT)
		{
			m_heightForRow[index.row()] = value.toInt();
			emit(dataChanged(index, index));
		}
	}
	return false;
}
//! [6]

//! [2]
QVariant ChatModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

	if (index.row() >= m_items.size() || index.row() < 0)
        return QVariant();

    if (role == Qt::DisplayRole)
	{
		const auto &itemObj = m_items.at(index.row());
		return QVariant::fromValue(itemObj);
    }
	else if (role == DOC_HEIGHT)
	{
		return m_heightForRow[index.row()];
	}
    return QVariant();
}
//! [2]

void ChatModel::insertRowData(int rowIndex, const QList<UIMsgInfo> &msgs)
{
	for (int i = 0; i < msgs.count(); i++)
	{
		int curRowIndex = rowIndex + i;
		const UIMsgInfo &msgInfo = msgs.at(i);
		//超过半小时，显示时间
		bool bShowTime = false;
		if (msgInfo.contentType != CRIM_MsgType_NULL)
		{
			QDateTime msgTime = msgInfo.getSendTime();
			//第一条消息显示
			if (!m_lastTime.isValid() || m_lastTime.isNull())
			{
				bShowTime = true;
			}
			//间隔30分钟显示
			if (msgTime.isValid() && m_lastTime.secsTo(msgTime) >= 30 * 60)
			{
				bShowTime = true;
			}
			if (msgTime.isValid())
			{
				m_lastTime = msgTime;
			}
		}

		ChatItemObject *itemObj = ChatItemObject::CreateItemObj(msgInfo, m_listView->getStyle());
 		connect(itemObj, &ChatItemObject::s_faceClicked, this, &ChatModel::slot_itemFaceClicked);
		connect(itemObj, &ChatItemObject::s_contentClicked, this, &ChatModel::slot_itemContentClicked);
		connect(itemObj, &ChatItemObject::s_areaClicked, this, &ChatModel::slot_itemAreaClicked);
		connect(itemObj, &ChatItemObject::s_resendClicked, this, &ChatModel::slot_resendClicked);
		connect(itemObj, &ChatItemObject::s_needUpdate, this, &ChatModel::slot_itemNeedUpdate);
		itemObj->showTime(bShowTime);
		itemObj->setMsgInfo(msgInfo);

		m_items.insert(curRowIndex, itemObj);
		m_heightForRow.insert(curRowIndex, 0);
		m_itemForMsgId[msgInfo.clientMsgID] = itemObj;

		setData(index(curRowIndex, 0), QVariant());
		//设置高度，通知更新
		if (!getViewSize().isEmpty())
		{
			updateSizeHintData(curRowIndex);
		}
	}
}

void ChatModel::removeRowData(int row)
{
	ChatItemObject *itemObj = m_items.at(row);
	m_itemForMsgId.remove(m_itemForMsgId.key(itemObj));
	itemObj->deleteLater();
	m_items.removeAt(row);
	m_heightForRow.removeAt(row);
}

QModelIndex ChatModel::index(int row, int column, const QModelIndex &parent ) const
{
	if (!hasIndex(row, column, parent))
		return QModelIndex();

	return createIndex(row, column);
}

QModelIndex ChatModel::parent(const QModelIndex& index) const
{
	return QModelIndex();
}

void ChatModel::slot_itemNeedUpdate()
{
	ChatItemObject *itemObj = static_cast<ChatItemObject *>(sender());
	if (itemObj == NULL)
	{
		return;
	}
	int row = m_items.indexOf(itemObj);
	if (row < 0)
	{
		return;
	}

	QModelIndex index = this->index(row, 0);
	setData(index, QVariant());
}

void ChatModel::slot_itemFaceClicked(bool leftButton)
{
	ChatItemObject *itemObj = static_cast<ChatItemObject *>(sender());
	if (itemObj == NULL)
	{
		return;
	}

	if (leftButton)
	{
		emit s_faceLeftClicked(itemObj->getMsgInfo());
	}
	else
	{
		emit s_faceRightClicked(itemObj->getMsgInfo());
	}
}

void ChatModel::slot_itemContentClicked(bool leftButton, const QVariantMap &exParams)
{
	ChatItemObject *itemObj = static_cast<ChatItemObject *>(sender());
	if (itemObj == NULL)
	{
		return;
	}
	int row = m_items.indexOf(itemObj);
	if (row < 0)
	{
		return;
	}

	if (leftButton)
	{
		emit s_contentLeftClicked(row, itemObj->getMsgInfo(), exParams);
	}
	else
	{
		emit s_contentRightClicked(row, itemObj->getMsgInfo(), exParams);
	}
}

void ChatModel::slot_itemAreaClicked(bool leftButton)
{
	ChatItemObject *itemObj = static_cast<ChatItemObject *>(sender());
	if (itemObj == NULL)
	{
		return;
	}

	if (!leftButton)
	{
		emit s_areaRightClicked();
	}
}

void ChatModel::slot_resendClicked()
{
	ChatItemObject *itemObj = static_cast<ChatItemObject *>(sender());
	if (itemObj == NULL)
	{
		return;
	}

	emit m_listView->s_resendClicked(itemObj->getMsgInfo());
}
