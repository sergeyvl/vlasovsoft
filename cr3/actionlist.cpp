#include "actionlist.h"

ActionList::ActionList()
{
}

QString ActionList::getTapZonesParamName(int t, int x, int y) const
{
    return QString("window.tapzone.%1.%2.%3").arg(t).arg(x).arg(y);
}

QString ActionList::getSwipesParamName(int t) const
{
    return QString("window.swipe.%1").arg(t);
}
