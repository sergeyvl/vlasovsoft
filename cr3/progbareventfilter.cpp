#include <QMouseEvent>
#include <QProgressBar>

#include "progbareventfilter.h"

static int round( float val )
{
    return (val > 0.0) ? (val + 0.5) : (val - 0.5);
}

ProgBarEventFilter::ProgBarEventFilter(QObject *parent)
    : QObject(parent)
    , pixelsOrigin(0)
    , progressOrigin(0)
{
}

bool ProgBarEventFilter::eventFilter(QObject* obj, QEvent* e)
{
    if (    QEvent::MouseButtonPress   == e->type()
         || QEvent::MouseButtonRelease == e->type()
         || QEvent::MouseMove          == e->type() )
    {
        QProgressBar* pb = qobject_cast<QProgressBar*>(obj);
        if ( !pb ) return false;
        QMouseEvent* me = static_cast<QMouseEvent*>(e);
        switch ( e->type() )
        {
        case QEvent::MouseButtonPress:
            pixelsOrigin   = me->pos().x();
            progressOrigin = pb->value();
            break;
        case QEvent::MouseButtonRelease:
            break;
        case QEvent::MouseMove:
            {
                int deltaPixels   = me->pos().x() - pixelsOrigin;
                int deltaProgress = round( 1.0 * (pb->maximum() - pb->minimum() + 1) * deltaPixels / pb->width() );
                int newValue      = progressOrigin + deltaProgress;
                if ( newValue > pb->maximum() )
                    newValue = pb->maximum();
                if ( newValue < pb->minimum() )
                    newValue = pb->minimum();
                pb->setValue(newValue);
            }
            break;
        default:
            break;
        }

        return true;
    }

    return false;
}
