#ifndef ACTIONDISPLAYER_H
#define ACTIONDISPLAYER_H

#include <QLabel>
#include <QTimer>

class CR3View;

class ActionDisplayer : public QLabel
{
    Q_OBJECT

    QTimer timer;

public:
    ActionDisplayer(QWidget* parent, CR3View* docView, const QString& title);
};

#endif // ACTIONDISPLAYER_H
