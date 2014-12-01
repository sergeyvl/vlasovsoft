#include "screenplugin.h"
#include "KoboFb.h"

ScreenPlugin::ScreenPlugin() : QScreenDriverPlugin()
{
    qDebug("ScreenPlugin::ScreenPlugin()");
}

QStringList ScreenPlugin::keys() const
{
    qDebug("ScreenPlugin::keys()");
    QStringList list;
    list << QLatin1String("KoboFb");
    return list;
}

QScreen* ScreenPlugin::create(const QString& driver, int /*displayId*/)
{
    qDebug("ScreenPlugin::create()");
    if (driver.toLower() == QLatin1String("kobofb"))
    {
        return new KoboFb();
    }
    return NULL;
}

Q_EXPORT_PLUGIN2(KoboFb, ScreenPlugin)
