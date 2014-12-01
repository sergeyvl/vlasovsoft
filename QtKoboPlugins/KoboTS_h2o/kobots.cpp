#include <unistd.h>

#include "SuspendManager.h"

#include "kobots.h"

KoboTS::KoboTS(const QString & driver, const QString & device, QObject* parent) 
    : QObject(parent)
    , QWSMouseHandler(driver, device)
    , buttons(Qt::NoButton)
    , isInputCaptured(false)
{
    _debug = device.contains("debug", Qt::CaseInsensitive);

    if (_debug)
        qDebug("KoboTS(%s, %s)", (const char*)driver.toAscii(), (const char*)device.toAscii());

    _fd = open("/dev/input/event1", O_RDONLY);

    _sn = new QSocketNotifier(_fd, QSocketNotifier::Read);

    connect(_sn, SIGNAL(activated(int)), this, SLOT(activity(int)));

    _sn->setEnabled(true);

    captureInput();
}

KoboTS::~KoboTS()
{
    releaseInput();
    delete _sn;
    close(_fd);
}

#define POS_TH 10

typedef struct input_event input_event_t;

void KoboTS::activity(int)
{
    _sn->setEnabled(false);

    input_event_t in; 

    unsigned size = 0;

    while ( size < sizeof(input_event_t) )
    {
        ssize_t s = read(_fd, ((char*)&in)+size, sizeof(input_event_t)-size);
        if ( -1 == s || 0 == s ) return;
        size += s;
    }

    if (_debug) qDebug("TS data: type %d, code %d, value %d", in.type, in.code, in.value);

    switch ( in.type )
    {
    case EV_ABS:
        switch ( in.code )
        {
        // multi touch
        case 53:
            newPos.setX(in.value);
            break;
        case 54:
            newPos.setY(in.value);
            break;
        case 48:
            if ( 0 == in.value )
                newPos = oldPos;
            buttons = in.value != 0 ? Qt::LeftButton : Qt::NoButton;
            break;
        // single touch
        case 0:
            newPos.setX(in.value);
            break;
        case 1:
            newPos.setY(in.value);
            break;
        case 24:
            if ( 0 == in.value )
                newPos = oldPos;
            buttons = in.value != 0 ? Qt::LeftButton : Qt::NoButton;
            break;
        default:
            break;
        }
        break;
    case EV_SYN:
        if ( 0 == in.code )
        {
            mouseChanged(newPos, buttons, 0);
            SuspendManager::instanse()->activity();
        }
        oldPos = newPos;
        if ( _debug ) 
            qDebug("Mouse changed: x=%d, y=%d, val=%d", newPos.x(), newPos.y(), buttons);
        break;
    default:
        break ;
    }

    _sn->setEnabled(true);
}

void KoboTS::captureInput(void)
{
        int on = 1 ;
        if ( !isInputCaptured )
        {
            if (_debug) qDebug("attempting to capture input...");
            if ( _fd != -1 )
            {
                if ( ioctl(_fd, EVIOCGRAB, on) )
                {
                    if ( _debug ) qDebug("Capture touch input: error");
                }
                else
                {
                    if ( _debug ) qDebug("Capture touch input: success");
                }
            }
            isInputCaptured = true;
      }
}

void KoboTS::releaseInput()
{
    int off = 0;
    if ( isInputCaptured )
    {
        if (_debug) qDebug("attempting to release input...");
        if (_fd != -1)
        {
            if (ioctl(_fd, EVIOCGRAB, off)) 
            {
                if (_debug) qDebug("Release touch input: error");
            }
        }
        isInputCaptured = false;
    }
}

