#include "stdafx.h"
#include "NewFriendMsgPage.h"
#include "ui_NewFriendMsgPage.h"
#include "ui_NewFriendMsgItem.h"


class FriendApItem : public ListWidgetItemWithIMIcon, public CRIMFriendReqInfo
{
public:
	FriendApItem(QListWidget *parent) : ListWidgetItemWithIMIcon(parent){}
	~FriendApItem() {}

	virtual bool operator<(const QListWidgetItem &other) const
	{
		const FriendApItem* item2 = static_cast<const FriendApItem*>(&other);
		return getItemTime() > item2->getItemTime();
	}

	qint64 getItemTime() const
	{
		return qMax(createTime, handleTime);
	}

};


class NewFriendMsgItemWidget : public QWidget
{
public:
	NewFriendMsgItemWidget(QWidget *parent, FriendApItem *item) : QWidget(parent), _item(item)
	{
		ui.setupUi(this);
	}

	Ui::NewFriendMsgItem ui;
	FriendApItem* _item;
};



NewFriendMsgPage::NewFriendMsgPage(QWidget *parent)
	: QWidget(parent)
{
	ui = new Ui::NewFriendMsgPage();
	ui->setupUi(this);
	QHBoxLayout *pLyout = new QHBoxLayout(ui->listWidget);
	_noneDescLb = new QLabel(this);
	_noneDescLb->setObjectName("noneDescLb");
	_noneDescLb->setAlignment(Qt::AlignCenter);
	_noneDescLb->hide();
	pLyout->addWidget(_noneDescLb);

	ui->listWidget->setIconSize(DEFUALT_ICONSZ);

	connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_getFriendReqListAsRecipientSuccess, this, &NewFriendMsgPage::slot_getFriendReqListSuccess);
	connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_getFriendReqListAsApplicantSuccess, this, &NewFriendMsgPage::slot_getFriendReqListSuccess);
	connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_friendReqAdded, this, &NewFriendMsgPage::slot_friendReqAdded);
	connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_friendReqDeleted, this, &NewFriendMsgPage::slot_friendReqDeleted);
	connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_friendReqAccepted, this, &NewFriendMsgPage::slot_friendReqChanged);
	connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_friendReqRejected, this, &NewFriendMsgPage::slot_friendReqChanged);
	connect(ui->listWidget, &QListWidget::itemClicked, this, &NewFriendMsgPage::slot_itemClicked);

	slot_loginSuccess();
}

NewFriendMsgPage::~NewFriendMsgPage()
{
	delete ui;
}

void NewFriendMsgPage::updateNoneDescLb()
{
	if (ui->listWidget->count() <= 0)
	{
		_noneDescLb->setText(tr("没有通知消息"));
		_noneDescLb->show();
	}
	else
	{
		_noneDescLb->hide();
	}
}


void NewFriendMsgPage::slot_loginSuccess()
{
	ui->listWidget->clear();
	_items.clear();

	CRIMManager_Qt::instance()->getFriendReqListAsRecipient(0, 100);
	CRIMManager_Qt::instance()->getFriendReqListAsApplicant(0, 100);
}

void NewFriendMsgPage::AddToList(const CRIMFriendReqInfo &info)
{
	QString key = info.fromUserID + "|" + info.toUserID;
	if (_items.find(key) == _items.end())
	{
		FriendApItem *pItem = new FriendApItem(ui->listWidget);
		ui->listWidget->addItem(pItem);
		_items[key] = pItem;

		QWidget *pW = new NewFriendMsgItemWidget(ui->listWidget, pItem);
		ui->listWidget->setItemWidget(pItem, pW);
	}

	UpdateToList(info);
}

void NewFriendMsgPage::UpdateToList(const CRIMFriendReqInfo &info)
{
	QString key = info.fromUserID + "|" + info.toUserID;
	auto fItem = _items.find(key);
	if (fItem == _items.end())
	{
		AddToList(info);
		return;
	}
	
	bool bSendType = (info.fromUserID == CRIMManager_Qt::instance()->getUserID());
	FriendApItem *pItem = fItem.value();
	QString peerUID = bSendType ? info.toUserID : info.fromUserID;
	QString peerName = bSendType ? info.toNickname : info.fromNickname;
	QString faceUrl = bSendType ? info.toFaceURL : info.fromFaceURL;

	*((CRIMFriendReqInfo*)pItem) = info;
	ImIconMgr::instance()->bindIcon(pItem, IMIconInfo(peerUID, faceUrl, peerName, IMICON_USR, ui->listWidget->iconSize()));

	NewFriendMsgItemWidget *pW = static_cast<NewFriendMsgItemWidget*>(ui->listWidget->itemWidget(pItem));
	pW->ui.name->setText(peerName);
	QString reqMsg(info.reqMsg);
	pW->ui.text->setText(reqMsg.replace("\n", " "));
	pW->ui.text->setToolTip(reqMsg);
	connect(pW->ui.acceptBtn, &QToolButton::clicked, this, &NewFriendMsgPage::slot_itemAcceptBtnClicked);
	connect(pW->ui.rejectBtn, &QToolButton::clicked, this, &NewFriendMsgPage::slot_itemRejectBtnClicked);

	switch (info.handleResult)
	{
	case CRIM_ReqStatus_Normal:
		if (bSendType)
		{
			pW->ui.acceptBtn->setText(tr("等待验证"));
			pW->ui.acceptBtn->setEnabled(false);
			pW->ui.acceptBtn->show();
			pW->ui.rejectBtn->hide();
		}
		else
		{
			pW->ui.acceptBtn->setText(tr("接受"));
			pW->ui.acceptBtn->setEnabled(true);
			pW->ui.acceptBtn->show();
			pW->ui.rejectBtn->show();
		}
		break;

	case CRIM_ReqStatus_Accept:
		{
			pW->ui.acceptBtn->setText(tr("已接受"));
			pW->ui.acceptBtn->setEnabled(false);
			pW->ui.acceptBtn->show();
			pW->ui.rejectBtn->hide();
		}
		break;

	case CRIM_ReqStatus_Decline:
		{
			pW->ui.acceptBtn->setText(tr("已拒绝"));
			pW->ui.acceptBtn->setEnabled(false);
			pW->ui.acceptBtn->show();
			pW->ui.rejectBtn->hide();
		}
		break;

	default:
		{
			pW->ui.acceptBtn->hide();
			pW->ui.rejectBtn->hide();
		}
		break;
	}
}

void NewFriendMsgPage::slot_getFriendReqListSuccess(const QList<CRIMFriendReqInfo> &infos, const QString &cookie)
{
	for (auto &info : infos)
	{
		AddToList(info);
	}
	ui->listWidget->sortItems();
	emit s_firendReqMsgChanged(hasFriendReqMsg());

	updateNoneDescLb();
}

void NewFriendMsgPage::slot_friendReqAdded(const CRIMFriendReqInfo &info)
{
	AddToList(info);
	ui->listWidget->sortItems();
	emit s_firendReqMsgChanged(hasFriendReqMsg());

	updateNoneDescLb();
}

void NewFriendMsgPage::slot_friendReqDeleted(const CRIMFriendReqInfo &info)
{
	QString key = info.fromUserID + "|" + info.toUserID;
	auto fItem = _items.find(key);
	if (fItem == _items.end())
		return;

	FriendApItem *pItem = fItem.value();
	int row = ui->listWidget->row(pItem);
	ui->listWidget->takeItem(row);

	delete pItem;
	_items.erase(fItem);
	emit s_firendReqMsgChanged(hasFriendReqMsg());

	updateNoneDescLb();
}

void NewFriendMsgPage::slot_friendReqChanged(const CRIMFriendReqInfo &info)
{
	UpdateToList(info);
	ui->listWidget->sortItems();
	emit s_firendReqMsgChanged(hasFriendReqMsg());
}

void NewFriendMsgPage::slot_itemClicked(QListWidgetItem *item)
{
// 	FriendApItem* pItem = static_cast<FriendApItem*>(item);
// 	if(NULL == pItem)
// 		return;
// 	NewFriendMsgItemWidget *pItemWidget = static_cast<NewFriendMsgItemWidget*>(ui->listWidget->itemWidget(pItem));
// 	if(NULL == pItemWidget)
// 		return;
// 
// 	if(pItemWidget->ui.acceptBtn->isEnabled())
// 	{
// 		NewFriendOperDlg *pOperDlg = new NewFriendOperDlg(*pItem, this);
// 		this->setAttribute(Qt::WA_DeleteOnClose);
// 		pOperDlg->exec();
// 	}
}

void NewFriendMsgPage::slot_itemAcceptBtnClicked()
{
	QWidget *pBtn = dynamic_cast<QWidget*>(sender());
	if (pBtn == NULL)
		return;
	NewFriendMsgItemWidget *pItemWidget = dynamic_cast<NewFriendMsgItemWidget*>(pBtn->parentWidget());
	if (pItemWidget == NULL)
		return;

	CRIMManager_Qt::instance()->acceptFriendReq(pItemWidget->_item->fromUserID, QString());
}

void NewFriendMsgPage::slot_itemRejectBtnClicked()
{
	QWidget *pBtn = dynamic_cast<QWidget*>(sender());
	if (pBtn == NULL)
		return;
	NewFriendMsgItemWidget *pItemWidget = dynamic_cast<NewFriendMsgItemWidget*>(pBtn->parentWidget());
	if (pItemWidget == NULL)
		return;

	CRIMManager_Qt::instance()->refuseFriendReq(pItemWidget->_item->fromUserID, QString());
}

bool NewFriendMsgPage::hasFriendReqMsg()
{
	QString myUserID = CRIMManager_Qt::instance()->getUserID();
	int i = 0;
	for (auto pItem : _items)
	{
		bool bRecvType = (pItem->fromUserID != myUserID);
		if (bRecvType && pItem->handleResult == 0)
		{
			i++;
		}
	}
	return (i!=0);
}

