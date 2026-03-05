#ifndef CRTxtEditor_H
#define CRTxtEditor_H

class CRTxtEditor : public QLineEdit
{
	Q_OBJECT
public:
	explicit CRTxtEditor(QWidget *parent = 0);

	void setFocusInAutoStartEdit(bool bAutoStart = true);
	void setAutoWidth(bool bAutoWidth = false);

public slots:
	void startEditWithText(const QString &text);
	void startEdit();
	void cancelEdit();

signals:
	void s_canceled();
	void s_commit(const QString &oldStr, const QString &newStr);

protected:
	QSize sizeHint() const override;
	QSize minimumSizeHint() const override;
	void keyPressEvent(QKeyEvent *event) override;
	void mousePressEvent(QMouseEvent *event) override;
	void focusInEvent(QFocusEvent *event) override;
	void focusOutEvent(QFocusEvent *event) override;
	void onCommit();

protected slots:
	void slot_textChanged();

protected:
	QString m_orgText;
	bool m_bAutoWidth{ false };
	bool m_bFocusInAutoStart{ false };
};


#endif // CRTxtEditor_H
