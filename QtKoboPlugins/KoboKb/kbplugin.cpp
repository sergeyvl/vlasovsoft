#include "kbplugin.h"
#include "kobokb.h"

KbPlugin::KbPlugin(QObject* parent) : QKbdDriverPlugin(parent)
{
}

QStringList KbPlugin::keys() const
{
    QStringList list;
    list << QLatin1String("KoboKb");
    return list;
}

QWSKeyboardHandler* KbPlugin::create(const QString & driver, const QString & device)
{
    Q_UNUSED(driver)
    if (device.contains("debug", Qt::CaseInsensitive))
        qDebug("KbPlugin::create() found!");
    return new KoboKb(device);
}

Q_EXPORT_PLUGIN2(KoboKb, KbPlugin)
