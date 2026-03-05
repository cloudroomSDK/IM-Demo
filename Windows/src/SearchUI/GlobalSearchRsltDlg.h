#ifndef GLOBALSEARCHRSLTDLG_H
#define GLOBALSEARCHRSLTDLG_H

namespace Ui { class GlobalSearchRsltDlg; };


class GlobalSearchRsltItem;
class GlobalSearchExpandItem;
class GlobalSearchRsltDlg : public QWidget
{
	Q_OBJECT

public:
	GlobalSearchRsltDlg(QLineEdit *edit, QWidget* parent = 0);
	~GlobalSearchRsltDlg();

	QLineEdit *curEdit() { return _searchEdit; }
	void setSearchText(const QString &str);

protected slots:
	void slot_research();
	void slot_itemClicked(QListWidgetItem *pItem);
	void slot_searchLocalMsgsSuccess(const CRIMSearchResultInfo &rslt, const QString &cookie);
	void slot_searchLocalMsgsFailed(int code, const QString &error, const QString &cookie);

protected:
	void paintEvent(QPaintEvent *event) override;
	bool eventFilter(QObject *watched, QEvent *event) override;

	void initListWidget();
	void clearSearchRslt();
	QString HighlighterText(const QString &str);
	void searchIMHistory();

	void selectNextItem(bool bDown);
	void itemExpandStateChanged(GlobalSearchExpandItem *pItem);
	void showImSearchDlg(const QString &conversationID);

private:
	Ui::GlobalSearchRsltDlg *ui;
	QTimer	_delayResearchTimer;

	QLineEdit*	_searchEdit{ nullptr };
	QString		_searchText;
	int			_imSearchTime;
	CRIMSearchResultInfo _imSearchRslt;

	GlobalSearchRsltItem*	_friendsTitle{ nullptr };
	GlobalSearchRsltItem*	_grpsTitle{ nullptr };
	GlobalSearchRsltItem*	_msgTitle{ nullptr };
	GlobalSearchRsltItem*	_msgLoading{ nullptr };

	GlobalSearchExpandItem*	_friendsExpand{ nullptr };
	GlobalSearchExpandItem*	_grpsExpand{ nullptr };
	GlobalSearchExpandItem*	_msgExpand{ nullptr };
};

#endif // GLOBALSEARCHRSLTDLG_H
