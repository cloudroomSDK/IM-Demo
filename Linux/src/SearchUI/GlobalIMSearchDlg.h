#ifndef GLOBALIMSEARCHDLG_H
#define GLOBALIMSEARCHDLG_H


namespace Ui {class GlobalIMSearchDlg;};
class GlobalSearchConvsationItem;
class GlobalIMSearchDlg : public QNoNCDialog
{
	Q_OBJECT

	GlobalIMSearchDlg(QWidget *parent = 0);
public:
	static GlobalIMSearchDlg* instance();
	~GlobalIMSearchDlg();

	void setContext(const QString &searchTxt, int searchTime, const CRIMSearchResultInfo &info, const QString &conversationID = QString());

protected slots:
	void slot_textChanged(const QString &txt);
	void slot_research();
	void slot_itemSelectionChanged();
	void slot_searchLocalMsgsSuccess(const CRIMSearchResultInfo &rslt, const QString &cookie);
	void slot_searchLocalMsgsFailed(int code, const QString &error, const QString &cookie);
	void slot_imViewContext();
	void slot_enterButtonClicked();
	void slot_backBtnClicked();

protected:
	void fillDatToList(const CRIMSearchResultInfo &info, const QString &curConversationID);

private:
	static GlobalIMSearchDlg* _ins;
	Ui::GlobalIMSearchDlg *ui;

	QString	_searchText;
	int		_searchTime;
	QTimer	_delayResearchTimer;

};

#endif // GLOBALIMSEARCHDLG_H
