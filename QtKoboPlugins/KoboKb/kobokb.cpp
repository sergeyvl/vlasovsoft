#include <unistd.h>

#include "kobokb.h"

KoboKb::KoboKb(const QString & device)
    : debug(false)
    , isInputCaptured(false)
{
    qDebug("KoboKb(%s)", (const char*)device.toAscii());

    _fd = open("/dev/input/event0", O_RDONLY);

    _sn = new QSocketNotifier(_fd, QSocketNotifier::Read);

    connect(_sn, SIGNAL(activated(int)), this, SLOT(activity(int)));

    if (device.contains("debug", Qt::CaseInsensitive))
        debug = true;

    _sn->setEnabled(true);

    captureInput();
}

KoboKb::~KoboKb()
{
    releaseInput();
    delete _sn;
    close(_fd);
}

void KoboKb::activity(int)
{
    _sn->setEnabled(false);

    input_event in;
    read(_fd, &in, sizeof(input_event));

    int code = Qt::Key_unknown;

    switch ( in.code )
    {
    case KEY_POWER:
        code = Qt::Key_PowerDown;
        break;
    case KEY_LIGHT:
        code = Qt::Key_BrightnessAdjust;
        break;
    case KEY_HOME:
        code = Qt::Key_Home;
        break;
    case KEY_SLEEP_COVER:
        code = Qt::Key_PowerOff;
        break;
    default:
        break;
    } 

    if ( code != Qt::Key_unknown )
    {
        processKeyEvent(0, code, Qt::NoModifier, (in.value == 1), false);
    }

    if ( debug )
        qDebug() << "in.type:" << in.type << " | in.code: " << in.code << " | code:" << code << " | " << (in.value? "pressed":"released");

   _sn->setEnabled(true);
}

void KoboKb::captureInput(void)
{
    int on = 1 ;
    if ( !isInputCaptured )
    {
        if (debug) qDebug("KoboKb: attempting to capture input...");
        if ( _fd != -1 )
        {
            if ( ioctl(_fd, EVIOCGRAB, on) )
            {
                if (debug) qDebug("KoboKb: capture keyboard input: error");
            }
        }
        isInputCaptured = true;
    }
}

void KoboKb::releaseInput()
{
    int off = 0;
    if ( isInputCaptured )
    {
        if (debug) qDebug("KoboKb: attempting to release input...");
        if (_fd != -1)
        {
            if (ioctl(_fd, EVIOCGRAB, off))
            {
                if (debug) qDebug("KoboKb: release keyboard input: error");
            }
        }
        isInputCaptured = false;
    }
}
