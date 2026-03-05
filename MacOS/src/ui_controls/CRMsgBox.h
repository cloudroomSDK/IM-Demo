#ifndef __CRMSGBOX_H__
#define __CRMSGBOX_H__

class CRMsgBox : public QNoNCDialog
{
	Q_OBJECT
public:
	enum StandardButton {
		NoButton           = 0x00,
		Ok                 = 0x01,
		Yes                = 0x02,
		No                 = 0x04,
		Close              = 0x08,
		Cancel             = 0x10,
	};

	CRMsgBox(QWidget *parent, const QString &title="", const QString &msgContent="", qint32 standardButtons=Ok);
	~CRMsgBox();

	void setMsg(const QString &title, const QString &msg);

	void addBtn(StandardButton btn);
	void addBtns(qint32 standardButtons);
	void setDefaultBtn(StandardButton defaultBtn=NoButton, int nSecond=-1);
	void setEscapeBtn(StandardButton escapeButton=NoButton);

	void addBtn(QAbstractButton* button);
	void setDefaultBtn(QAbstractButton* defaultBtn=NULL, int nSecond=-1);
	void setEscapeBtn(QAbstractButton* escapeButton=NULL);

	StandardButton standardButton(QAbstractButton *button);
	QAbstractButton* standardButton(StandardButton btn);
	QAbstractButton* clickBtn() { return _clickedBtn; }

	QList<QAbstractButton*> buttons();
signals:
	void buttonClicked(QWidget *pMsgBox, QAbstractButton *button);

protected slots:
	void slot_btnClicked();
	void slot_countDown();

public:
	static StandardButton msgBox(QWidget *parent, const QString &title, const QString &text, qint32 standardButtons = Ok, StandardButton defaultBtn = NoButton, int nSecondClickDefBtn = -1);
	inline static StandardButton critical(QWidget *parent, const QString &title, const QString &text, qint32 standardButtons = Ok, StandardButton defaultBtn = NoButton, int nSecondClickDefBtn = -1)
	{
		return msgBox(parent, title, text, standardButtons, defaultBtn, nSecondClickDefBtn);
	}
	inline static StandardButton warning(QWidget *parent, const QString &title, const QString &text, qint32 standardButtons = Ok, StandardButton defaultBtn = NoButton, int nSecondClickDefBtn = -1)
	{
		return msgBox(parent, title, text, standardButtons, defaultBtn, nSecondClickDefBtn);
	}
	inline static StandardButton question(QWidget *parent, const QString &title, const QString &text, qint32 standardButtons = Ok, StandardButton defaultBtn = NoButton, int nSecondClickDefBtn = -1)
	{
		return msgBox(parent, title, text, standardButtons, defaultBtn, nSecondClickDefBtn);
	}
	inline static StandardButton information(QWidget *parent, const QString &title, const QString &text, qint32 standardButtons = Ok, StandardButton defaultBtn = NoButton, int nSecondClickDefBtn = -1)
	{
		return msgBox(parent, title, text, standardButtons, defaultBtn, nSecondClickDefBtn);
	}

protected:
	virtual void closeEvent(QCloseEvent *e) { reject(); }
	virtual void accept();
	virtual void reject();
	virtual void showEvent(QShowEvent *event);

protected:
	void InitBox();
	void updateSize();
	void updateCountDownTxt(QAbstractButton* pBtn, int nSecond);

protected:
	QAbstractButton*		_defaultBtn;
	QAbstractButton*		_escapeBtn;
	QAbstractButton*		_clickedBtn;
	QLabel*					_msgLabel;
	QWidget*				_bottomBar;

	QTimer					_countDownTimer;
	int						_countDownValue;
};

#endif //__CRMSGBOX_H__
