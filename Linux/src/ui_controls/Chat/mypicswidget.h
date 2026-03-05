#ifndef MYPICSWIDGET_H
#define MYPICSWIDGET_H

class MypicsWidget : public QNoNCDialog
{
	Q_OBJECT

	MypicsWidget(const QString& image, QWidget *parent = 0);

	static MypicsWidget *s_Instance;
public:	
	~MypicsWidget();
	
	static void showInstance(const QString& image, QWidget *parent);
	static void destroyInstance();

protected slots:
	void customContextMenuRequestedSlot(const QPoint& pos);
	void CopyImage();
	void saveImageAs();
	void OriginalSize();

private: 
	void resizeEvent(QResizeEvent *) override;
	void showEvent(QShowEvent *) override;
	bool eventFilter(QObject *, QEvent *) override;

	void initializeImg();
	void resizeImg(int rate, bool bShowRate);
	QImage getImage();
	void showRate();

private:
	QScrollArea* m_scrollArea;
	QLabel* m_imgLabel;
	QLabel* m_rateLabel;
	int		m_showRate;		//显示比例
	QString	m_imgName;
	QSize	m_imgSz;
	QTimer  m_hideRateTimer;

};

#endif // MYPICSWIDGET_H
