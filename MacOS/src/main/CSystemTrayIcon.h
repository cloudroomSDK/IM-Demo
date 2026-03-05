#ifndef CSYSTEMTRAYICON_H
#define CSYSTEMTRAYICON_H

class CSystemTrayIcon : public QSystemTrayIcon
{
	Q_OBJECT

public:
	CSystemTrayIcon(QObject *parent = 0);
	~CSystemTrayIcon();

	void startFlashTray();
	void stopFlashTray();

private slots:
	void slot_iconActivated(QSystemTrayIcon::ActivationReason reason);
	void slot_animationValueChanged(const QVariant &value);

signals:
	void s_show();
	void s_exitApp();
	void s_showNextMsgSession();

private:
	QMenu*				m_menu;
	QVariantAnimation	m_flashAnimation;

};

#endif // CSYSTEMTRAYICON_H
