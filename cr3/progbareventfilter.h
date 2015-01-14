#ifndef PROGBAREVENTFILTER_H
#define PROGBAREVENTFILTER_H

#include <QObject>

class ProgBarEventFilter : public QObject
{
    Q_OBJECT

    int pixelsOrigin;
    int progressOrigin;

public:
    ProgBarEventFilter(QObject *parent = 0);

    bool eventFilter(QObject* obj, QEvent* e);
};

#endif // PROGBAREVENTFILTER_H
