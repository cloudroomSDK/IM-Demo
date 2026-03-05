#ifndef QACTIONSTYLEBTN_H
#define QACTIONSTYLEBTN_H

class QActionStyleBtn : public QToolButton
{
	Q_OBJECT
	Q_PROPERTY(bool isHover READ isHover)
	Q_PROPERTY(bool isPressed READ isPressed)
	Q_PROPERTY(bool isChecked READ isChecked)
	Q_PROPERTY(bool waitting READ waitting WRITE setWaitting)
	Q_PROPERTY(QString customIcon READ customIcon WRITE setCustomIcon)
	Q_PROPERTY(bool isEnabled READ isEnabled)
signals:
	void doubleClicked();
public:
	enum BIND_TYPE
	{
		//绑定显示
		BIND_NAME		= 0x01,
		BIND_ICON		= 0x02,
		BIND_TOOLTIP	= 0x04,
		BIND_CHECK_STATE= 0x08,
		BIND_ENABLE		= 0x10,
		BIND_VISIABLE	= 0x20,
		BIND_VIEW_ALL	= 0xFF,

		//绑定动作
		BIND_CLICK		= 0x100,
		BIND_DCLICK		= 0x200,
		BIND_OPR_ALL	= 0xF00,
		
		//所有
		BIND_ALL		= 0xFFFF
	};

	QActionStyleBtn(QWidget *parent=0);
	QActionStyleBtn(QWidget *parent, QAction *pAction, int bindType=BIND_ALL);
	~QActionStyleBtn();
	bool isHover(){return m_isHover;}
	void setHover(bool b);
	bool isPressed(){return m_isPressed;}
	void setPressed(bool b);

	void setCustomIcon(const QString &icon);
	QString customIcon() const;

	void setWaitting(bool bWaitting);
	bool waitting() const{ return m_bWaitting; }

	void SetAction(QAction *pAction, int bindType=BIND_ALL);
	QAction *GetBindAction() { return m_pAction; }
protected:
	void enterEvent(QEvent *e);
	void leaveEvent(QEvent *e);
	void mousePressEvent(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);
	void mouseDoubleClickEvent(QMouseEvent *event);

protected slots:
	void OnActionChanged();
	void OnToggled(bool checked);
	void slot_gifAnimation(int);

public slots:
	void setEnabled(bool);
	void setDisabled(bool);

private:
	int		m_type;
	QAction *m_pAction;
	bool	m_isHover;
	bool	m_isPressed;
	bool	m_bWaitting;
	QString	m_customIcon;
	QMovie	*m_movie;
};

#endif // QACTIONSTYLEBTN_H
