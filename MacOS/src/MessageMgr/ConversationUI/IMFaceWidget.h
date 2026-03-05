#ifndef IMFACEWIDGET_H
#define IMFACEWIDGET_H

class IMFaceListItemWidget : public QWidget
{
	Q_OBJECT
signals:
	void s_clicked();
public:
    IMFaceListItemWidget(QWidget *parent = 0);
    ~IMFaceListItemWidget(){;}
	void setInfo(const QString &fileName, const QSize &icoSize);
	QString getFileName(){return m_movie.fileName();}
protected:
	void enterEvent(QEvent *);
	void leaveEvent(QEvent *);
	void mouseReleaseEvent(QMouseEvent *);
private:
	QMovie m_movie;
	QLabel *m_lbPic;
};
class IMFaceWidget : public QWidget
{
	Q_OBJECT

public:
	struct ImgDef
	{
		QSize	iconSize;
		std::list<QString> imgs;
	};


    IMFaceWidget(QWidget *parent, QToolButton *pBinCheckBtn, ImgDef &imgs);
    ~IMFaceWidget();

signals:
	void s_faceSelSig(const QString& fileName);

protected slots:
	void slot_itemClicked();
protected:
	virtual void closeEvent(QCloseEvent *e);
	void setImageList(ImgDef &imgs);

private:
	QToolButton	 *m_pBinCheckBtn;
	QListWidget *m_icoListWidget;
	ImgDef m_imgsDef;
};

#endif // IMFaceWidget_H
