#include <QTimer>
#include <QVector>

#ifndef H__SUSPEND_MANAGER
#define H__SUSPEND_MANAGER

class SuspendManagerCallback;

class SuspendManager: public QTimer
{
public:
    enum State
    {
        ssActive,
        ssSuspending,
        ssResuming,
        ssLast
    };

    enum Event
    {
        evPowerBtn,
        evSleepCoverPressed,
        evSleepCoverReleased,
        evFifo,
        evTimeout,
        evResume,
        evLast
    };

    struct Transition
    {
        State oldState;
        State newState;
        Event event;
        Transition( State os, State ns, Event e )
            : oldState(os)
            , newState(ns)
            , event(e)
        {}
        Transition()
            : oldState(ssActive)
            , newState(ssActive)
            , event(evResume)
        {}
    };

private:
    SuspendManager();

    virtual void timerEvent( QTimerEvent* e );

public:
    static SuspendManager* instanse();
    static void cleanup();

public:
    void event( Event event );
    void setSuspendTimeout( int val );
    int getSuspendTimeout() const
    { return suspendTimeout_; }
    void activity();
    void setCallBack( SuspendManagerCallback* pCallback );

    static const char* get_state_name( State state )
    {
        static const char* states[ssLast] = {"Active","Suspending","Resuming"};
        return states[state];
    }

    static const char* get_event_name( Event event )
    {
        static const char* events[evLast] = {"PowerBtn","SleepCoverPressed","SleepCoverReleased","Fifo","TimerEvent","Resume"};
        return events[event];
    }

private:
    static SuspendManager* pManager;
    SuspendManagerCallback* pCallback_;
    QVector<Transition> transitions;
    State state;
    int suspendTimeout_;
};

class SuspendManagerCallback
{
public:
    virtual ~SuspendManagerCallback() {}
    virtual void onStateChanged( const SuspendManager::Transition& t ) = 0;
};

#endif
