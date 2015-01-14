#include <QWSDisplay>
#include <QTransformedScreen>

#include "orientation.h"

int get_orientation()
{
#if defined(KOBO)
    static int mapping[4] = {3,0,1,2};
#else
    static int mapping[4] = {0,1,2,3};
#endif
    return mapping[QScreen::instance()->transformOrientation()];
}

void set_orientation( int val )
{
#if defined(KOBO)
    static int mapping[4] = {1,2,3,0};
#else
    static int mapping[4] = {0,1,2,3};
#endif
    QWSDisplay::setTransformation(static_cast<QTransformedScreen::Transformation>(mapping[val]), 0);
}
