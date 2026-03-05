#ifndef IMMSGWIDGET_SEARCH_H
#define IMMSGWIDGET_SEARCH_H

#include "IMMsgWidget.h"
class QActionStyleBtn;
class GlobalSearchConvsationItem;
class IMMsgWidget_Search : public IMMsgWidget
{
	Q_OBJECT

public:
	IMMsgWidget_Search(QWidget *parent = 0);
	~IMMsgWidget_Search();

	void init(GlobalSearchConvsationItem *pItem, const QString &searchText, int searchTime);

protected slots:
	void slot_itemEntered(const QModelIndex &index);
	void slot_viewportEntered();
	void slot_loadMoreClicked();
	void slot_searchLocalMsgsSuccess(const CRIMSearchResultInfo &rslt, const QString &cookie);
	void slot_searchLocalMsgsFailed(int code, const QString &error, const QString &cookie);

protected:
	bool isSessionMsg(const CRIMMessage &msg) override;
	void leaveEvent(QEvent *event) Q_DECL_OVERRIDE;
	virtual void onMenuExec(QMenu *menu, ChatWidget::MenuType menuType, const UIMsgInfo &msg);

	void addOnePageMsg();

private:
	GlobalSearchConvsationItem *m_pItem;
	QString	m_searchText;
	int		m_searchTime;
	int		m_loadedPage;
	QByteArray	m_queryTaskID;

	QActionStyleBtn *m_btnViewContext;
};

#endif // IMMSGWIDGET_SEARCH_H
