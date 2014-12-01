#ifndef KOBOTS_H
#define KOBOTS_H

#include <QWSMouseHandler>
#include <QSocketNotifier>
#include <QWSServer>

#include <fcntl.h>
#include <linux/input.h>

class KoboTS : public QObject, public QWSMouseHandler
{
    Q_OBJECT

public:
    KoboTS(const QString & driver = QString(), const QString & device = QString(), QObject* parent = 0);
    virtual ~KoboTS();

    virtual void suspend();
    virtual void resume();

private slots:
    void activity(int);

private:
    int _fd;
    QSocketNotifier* _sn;

    Qt::MouseButton buttons;
    QPoint oldPos, newPos;

    bool _debug;
    bool isInputCaptured;

private:
    void captureInput();
    void releaseInput();
};

#define EV_SYN			0x00
#define EV_KEY			0x01
#define EV_REL			0x02
#define EV_ABS			0x03

#define ABS_MT_POSITION_X	0x35	/* Center X ellipse position */
#define ABS_MT_POSITION_Y	0x36	/* Center Y ellipse position */

#define BTN_TOOL_FINGER		0x145
#define BTN_TOUCH		0x14a
#define BTN_TOOL_DOUBLETAP	0x14d

#define KT_HOME 102

#endif // KOBOTS_H
