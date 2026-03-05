#ifndef QFINDEDIT_H
#define QFINDEDIT_H

#include <CRLineEdit.h>

class QFindEdit : public CRLineEdit
{
	Q_OBJECT

public:
	QFindEdit(QWidget *parent);
	~QFindEdit();

	QStringList& stringList();//返回引用，可以往返回结果里面添加成员，修改m_stringList成员！

signals:
	void s_findFinished(const QString &str);
	void s_addMenber(const QString &str);

	protected slots:
		void textEditedSlot(const QString& text);
		void clickedSlot(QModelIndex modelIndex);
		void findFinished();

		void delayFocusSet();

protected:
	virtual bool  eventFilter(QObject *obj, QEvent *event);
private:
	QListView    *m_pListWnd;
	QFrame		*m_viewFrame;
	QStringListModel *m_stringListmodel;
	bool         m_bEditFocus;
	bool		 m_bHaveResult;
	QStringList  m_stringList;

	void        showFindWnd(const QString& text);
	void        hideFindWnd();
	void        showPopup(const QRect& rect);

};

#endif // QFINDEDIT_H
