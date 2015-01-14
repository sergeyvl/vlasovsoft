#ifndef ACTIONLIST_H
#define ACTIONLIST_H

#include <QList>
#include <QAction>

class ActionList : public QList<QAction*>
{
public:
    ActionList();

    QString getTapZonesParamName( int t, int x, int y ) const;
    QString getSwipesParamName( int t ) const;
};

#endif // ACTIONLIST_H
