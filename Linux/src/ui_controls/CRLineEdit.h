#ifndef CRLINEEDIT_H
#define CRLINEEDIT_H

//[图标]lineEdit[clearBtn]
class CRLineEdit : public QLineEdit
{
	Q_OBJECT

	Q_PROPERTY(bool leftIconVisible READ leftIconVisible WRITE setLeftIconVisible)
	Q_PROPERTY(bool clearBtnVisible READ clearBtnVisible)

public:
    explicit CRLineEdit(QWidget *parent = 0);
	
	bool leftIconVisible() { return m_pLeftIcon->isVisibleTo(this); }
	void setLeftIconVisible(bool bVisible);

	enum ClearVisibleType { CLRV_NONE=0, CLRV_AUTO, CLRV_SHOWALWAYS };
	void setClearBtnVisibleType(ClearVisibleType vType);
	ClearVisibleType clearBtnVisibleType();
	bool clearBtnVisible() { return m_pClearBtn->isVisibleTo(this); }

	void addExtRightWidget(QWidget *pWidget);

signals:
	void clearBtnClicked();

protected:
	void resizeEvent(QResizeEvent *event) override;
	void showEvent(QShowEvent *event) override;
	void resized();


private slots:
	virtual void slot_textChanged(const QString &text);
	void slot_clearClicked();

private:
	QWidget		*m_pLeftIcon;
	QToolButton *m_pClearBtn;
};

#endif // SEARCHEDIT_H
