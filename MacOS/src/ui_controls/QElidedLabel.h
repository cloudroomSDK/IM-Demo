#ifndef QELIDEDLABEL_H
#define QELIDEDLABEL_H

class QElidedLabel : public QLabel
{
	Q_OBJECT

public:
	QElidedLabel(QWidget *parent = 0);
	~QElidedLabel();

	void setElideMode(Qt::TextElideMode elideMode);
	Qt::TextElideMode elideMode() const { return m_elideMode; }

	void setText(const QString &str);
	QString text() const { return m_orgText; }

protected:
	void resizeEvent(QResizeEvent *e) override;
	QSize sizeHint() const Q_DECL_OVERRIDE;
	void updateElidedText();

private:
	Qt::TextElideMode m_elideMode;
	QString m_orgText;
};

#endif // QACTIONBTN_H
