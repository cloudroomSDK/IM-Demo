#ifndef DLGFASTSELMEMBER_H
#define DLGFASTSELMEMBER_H

class DlgFastSelMember : public QNoNCDialog
{
	Q_OBJECT

signals:
	void s_selFinished(const QString &name);
public:
	DlgFastSelMember(QTextEdit *edit, QWidget *parent = 0);
	~DlgFastSelMember();

	void insertName(const QString &name);
	void removeName(const QString &name);
	void clear();

	void showFindWnd(const QString& text, const QPoint &pos);

protected:
	bool eventFilter(QObject *o, QEvent *e);

protected slots:
	void slot_WindowActiveChanged(bool b);
	void clickedSlot(QModelIndex modelIndex);

private:
	QStringList  m_stringList;
	QStringListModel *m_stringListmodel;
	QListView *m_pListWnd;
	QTextEdit *m_textEdit;
};

#endif // DlgFastSelMember_h
