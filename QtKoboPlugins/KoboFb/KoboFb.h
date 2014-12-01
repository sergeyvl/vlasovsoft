#ifndef KOBO_FB_H
#define KOBO_FB_H

#include <QLinuxFbScreen>
#include <QTimer>

class QPoint;
class QRegion;
class QRect;

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

class KoboFb : public QTimer, public QLinuxFbScreen
{
    Q_OBJECT

    int fbd;
    bool debug;
    bool full; // full update is scheduled
    QRect updateRect;

public:
    KoboFb();
    virtual ~KoboFb();

    virtual bool connect(const QString& displaySpec);
    virtual void shutdownDevice();
    virtual void setDirty ( const QRect& rect );

private slots:
    void update();
    void rotate( int angle );
    void fullUpdate();
};

QT_END_NAMESPACE

#endif // KOBO_FB_H
