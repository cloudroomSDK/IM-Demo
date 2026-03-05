#ifndef NEWFRIENDMSGPAGE_H
#define NEWFRIENDMSGPAGE_H

namespace Ui {class NewFriendMsgPage;};
class FriendApItem;

class NewFriendMsgPage : public QWidget
{
	Q_OBJECT

public:
	NewFriendMsgPage(QWidget *parent = 0);
	~NewFriendMsgPage();

	bool hasFriendReqMsg();

signals:
	void s_firendReqMsgChanged(bool hasFriendReqMsg);

protected slots:
	void slot_loginSuccess();
	void slot_getFriendReqListSuccess(const QList<CRIMFriendReqInfo> &, const QString &cookie = QString());
	void slot_friendReqAdded(const CRIMFriendReqInfo &info);
	void slot_friendReqDeleted(const CRIMFriendReqInfo &info);
	void slot_friendReqChanged(const CRIMFriendReqInfo &info);

	void slot_itemClicked(QListWidgetItem *item);
	void slot_itemAcceptBtnClicked();
	void slot_itemRejectBtnClicked();

protected:
	void AddToList(const CRIMFriendReqInfo &info);
	void UpdateToList(const CRIMFriendReqInfo &info);
	void updateNoneDescLb();

private:
	Ui::NewFriendMsgPage *ui;
	//key: "from|to"
	QMap<QString, FriendApItem*> _items;
	QLabel*	_noneDescLb;
};

#endif // NEWFRIENDMSGPAGE_H
