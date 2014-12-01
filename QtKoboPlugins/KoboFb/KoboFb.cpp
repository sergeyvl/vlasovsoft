#include <unistd.h>

#include <fcntl.h>
#include <asm/types.h>
typedef __u8 u8;
#include "linux/mxcfb.h"
#include <sys/ioctl.h>

#include <QDebug>
#include <QTime>

#include "ScreenManager.h"
#include "KoboFb.h"

KoboFb::KoboFb()
    : QLinuxFbScreen(0)
    , fbd(-1)
    , debug(false)
    , full(false)
{
    setSingleShot(true);
    QObject::connect(this, SIGNAL(timeout()), this, SLOT(update()));
}

KoboFb::~KoboFb()
{
}

bool KoboFb::connect(const QString& displaySpec)
{
    const QStringList params = displaySpec.split(':');
    if ( params.size() >= 3 )    
        debug = params.at(2) == "debug";

    if ( debug )
        qDebug() << "KoboFb::connect(" << displaySpec << ")";

    fbd = open("/dev/fb0", O_RDWR);

    if ( -1 == fbd )
    {
        if ( debug )
            qDebug("FAILED to open framebuffer.");
    }

    bool res = QLinuxFbScreen::connect( "" );

    physHeight = 90;   // mm
    physWidth  = 120;  // mm
 
    if ( params.size() >= 2 )
    {
        QStringList args = params.at(1).split('x');
        if ( args.size() >= 2 )
        {
            physWidth = args.at(0).toInt();
            physHeight = args.at(1).toInt();
        }
    }

    if ( params.size() >= 3 )
    {
        int angle = params.at(2).toInt();
        if ( ioctl(fbd, IOCTL_CMD_SET_ROTATION_ANGLE, angle) )
        {
            if ( debug )
                qDebug("FAILED to rotate framebuffer.");
        }
    }

    dw = w;
    dh = h;

    qDebug("KoboFb - Screen size: %d x %d (%dmm x %dmm)", w, h, physWidth, physHeight);

    return res;
}

void KoboFb::shutdownDevice()
{
    QLinuxFbScreen::shutdownDevice();
}

void KoboFb::setDirty ( const QRect& r )
{
    if ( debug ) qDebug("KoboFb::setDirty(%d, %d, %d, %d)", r.x(), r.y(), r.width(), r.height());
    if (    0 == r.x() 
         && 0 == r.y() 
         && width() == r.width() 
         && height() == r.height() )
    {
        full = ScreenManager::instanse()->getAllowFullUpdate();
    }
    updateRect = updateRect.united(r);
    start(full ? 100:50);
}

void KoboFb::update()
{
    if ( -1 != fbd )
    {
        mxcfb_update_data region;
        region.update_region.top = updateRect.top();
        region.update_region.left = updateRect.left();
        region.update_region.width = updateRect.width();
        region.update_region.height = updateRect.height() ;
        region.waveform_mode = WAVEFORM_MODE_AUTO;
        region.update_mode = full? UPDATE_MODE_FULL:UPDATE_MODE_PARTIAL ;
        region.temp = TEMP_USE_AMBIENT;
        region.flags = 0;
        ioctl(fbd, MXCFB_SEND_UPDATE, &region);
    }

    if ( debug )
        qDebug() << "KoboFb::update():" << updateRect << " full:" << full;

    ScreenManager::instanse()->setAllowFullUpdate( true );

    full = false;

    updateRect = QRect();
}

QT_END_NAMESPACE
