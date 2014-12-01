#include "tsplugin.h"
#include "kobots.h"

TSPlugin::TSPlugin(QObject* parent) : QMouseDriverPlugin(parent)
{
}

QStringList TSPlugin::keys() const
{
    QStringList list;
    list << QLatin1String("KoboTS");
    return list;
}

QWSMouseHandler* TSPlugin::create(const QString & key, const QString & device)
{
    if (key.toLower() == QLatin1String("kobots"))
    {
        if (device.contains("debug", Qt::CaseInsensitive))
            qDebug("TSPlugin::create() found!");
        return new KoboTS(key, device);
    }

    return 0;
}

Q_EXPORT_PLUGIN2(KoboTS, TSPlugin)
