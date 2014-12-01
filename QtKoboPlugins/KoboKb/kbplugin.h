#ifndef KBPLUGIN_H
#define KBPLUGIN_H

#include <QKbdDriverPlugin>
#include <QWSKeyboardHandler>

class KbPlugin : public QKbdDriverPlugin {
    Q_OBJECT
public:
    KbPlugin(QObject* parent = 0);

    QStringList keys() const;
    QWSKeyboardHandler* create(const QString & driver, const QString & device);
};

#endif // KBPLUGIN_H
