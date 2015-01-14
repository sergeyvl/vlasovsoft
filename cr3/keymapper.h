#ifndef KEYMAPPER_H
#define KEYMAPPER_H

#include <QString>
#include <QMap>

class KeyMapper
{
    QMap<int, QString> keyMap;

public:
    KeyMapper();

    QString map( int code ) const;
};

#endif // KEYMAPPER_H
