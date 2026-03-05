#include "stdafx.h"
#include "UICommon_process.h"
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusInterface>

int OpenAndSelectFile(const QString &file)
{
    QFileInfo fi(file);
    if(!fi.exists())
    {
        return 0;
    }
    QString absoluteFilePath = QString("file://%1").arg(fi.absoluteFilePath());
    QDBusConnection bus = QDBusConnection::sessionBus();
    QDBusInterface dbus_iface("org.freedesktop.FileManager1",
                             "/org/freedesktop/FileManager1",
                             "org.freedesktop.FileManager1",
                             bus);
    if (!dbus_iface.isValid())
    {
        return 0;
    }
    dbus_iface.call("ShowItems", QStringList{absoluteFilePath}, "");
    return 1;
}

int OpenLocFile(const QString &file)
{
    if(!QFile::exists(file))
    {
        return 0;
    }
    QDesktopServices::openUrl(QUrl::fromLocalFile(file));
    return 1;
}

QString GetOSName()
{
    QString osName = QSysInfo::productType().toLower();
    return osName;
}
