#ifndef QCLICKEDLABEL_H
#define QCLICKEDLABEL_H

class QClickedLabel : public QLabel
{
	Q_OBJECT

public:
	QClickedLabel(QWidget *parent=0);
	~QClickedLabel();

signals:
	void clicked(QClickedLabel *pLable);
	void doubleClick(QClickedLabel *pLable);

protected:
	virtual void mouseDoubleClickEvent(QMouseEvent *ev);
	virtual void mouseReleaseEvent(QMouseEvent *ev);
};

#endif // QACTIONBTN_H
