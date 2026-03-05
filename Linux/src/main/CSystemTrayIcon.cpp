#include "stdafx.h"
#include "CSystemTrayIcon.h"

CSystemTrayIcon::CSystemTrayIcon(QObject *parent) : QSystemTrayIcon(parent)
{
	m_menu = new QMenu;
	QAction *showUI = new QAction(tr("显示"), m_menu);
	QAction *quitAction = new QAction(tr("退出"), m_menu);
	m_menu->addAction(showUI);
	m_menu->addAction(quitAction);
	connect(showUI, SIGNAL(triggered()), this, SIGNAL(s_show()));
	connect(quitAction, SIGNAL(triggered()), this, SIGNAL(s_exitApp()));

	this->setIcon(App::instance()->getAppIcon());
	this->setToolTip(App::instance()->getAppName());
	this->setContextMenu(m_menu);
	connect(this, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(slot_iconActivated(QSystemTrayIcon::ActivationReason)));

	m_flashAnimation.setStartValue(0);
	m_flashAnimation.setEndValue(2);
	m_flashAnimation.setLoopCount(-1);
	m_flashAnimation.setDuration(900);
	connect(&m_flashAnimation, SIGNAL(valueChanged(const QVariant &)), this, SLOT(slot_animationValueChanged(const QVariant &)));
}

CSystemTrayIcon::~CSystemTrayIcon()
{
	this->stopFlashTray();
	this->hide();
}

//响应托盘操作
void CSystemTrayIcon::slot_iconActivated(QSystemTrayIcon::ActivationReason reason)
{
	switch (reason) {
	case QSystemTrayIcon::Trigger:
	case QSystemTrayIcon::DoubleClick:
		if (m_flashAnimation.state() == QAbstractAnimation::Stopped)
		{
			emit s_show();
		}
		else
		{
			emit s_showNextMsgSession();
		}
		break;
	default:
		;
	}
}

void CSystemTrayIcon::startFlashTray()
{
	m_flashAnimation.start();
}

void CSystemTrayIcon::stopFlashTray()
{
	m_flashAnimation.stop();
	if (App::instance() != nullptr)
	{
		this->setIcon(App::instance()->getAppIcon());
	}
}

void CSystemTrayIcon::slot_animationValueChanged(const QVariant &value)
{
	int intValue = value.toInt();
	QIcon newIcon;
	if (0 == intValue)
	{
		newIcon = QIcon();
	}
	else
	{
		newIcon = App::instance()->getAppIcon();
	}
	this->setIcon(newIcon);
}
