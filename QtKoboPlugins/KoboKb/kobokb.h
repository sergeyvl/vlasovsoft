#ifndef KOBOKB_H
#define KOBOKB_H

#include <QWSKeyboardHandler>
#include <QDebug>
#include <QSocketNotifier>
#include <QWSServer>

#include <iostream>
#include <fcntl.h>
#include <linux/input.h>

class KoboKb : public QObject, public QWSKeyboardHandler
{
    Q_OBJECT

public:
    KoboKb(const QString & device = QString("/dev/input/event0"));
    virtual ~KoboKb();

private slots:
    void activity(int);

private:
    int _fd;
    QSocketNotifier* _sn;
    
    bool debug;
    bool isInputCaptured;

private:
    void captureInput();
    void releaseInput();
};

#define KEY_LIGHT 90
#define KEY_HOME  102
#define KEY_POWER 116
#define KEY_SLEEP_COVER 59

#define EVENT_PRESS 1
#define EVENT_RELEASE 0

#endif // KOBOKB_H
