#include "SuspendManager.h"

SuspendManager* SuspendManager::pManager = NULL;

SuspendManager::SuspendManager()
    : pCallback_(NULL)
    , state(ssActive)
    , suspendTimeout_(0)
{
    setSingleShot(true);
    transitions.push_back(Transition(ssActive,ssSuspending,evPowerBtn));
    transitions.push_back(Transition(ssActive,ssSuspending,evSleepCoverPressed));
    transitions.push_back(Transition(ssActive,ssSuspending,evFifo));
    transitions.push_back(Transition(ssActive,ssSuspending,evTimeout));
    transitions.push_back(Transition(ssSuspending,ssActive,evPowerBtn));
    transitions.push_back(Transition(ssSuspending,ssActive,evSleepCoverReleased));
    transitions.push_back(Transition(ssSuspending,ssResuming,evResume));    
    transitions.push_back(Transition(ssResuming,ssActive,evPowerBtn));
    transitions.push_back(Transition(ssResuming,ssActive,evSleepCoverReleased));
}

void SuspendManager::timerEvent( QTimerEvent* e )
{
    Q_UNUSED(e)
    event( evTimeout );
}

SuspendManager* SuspendManager::instanse()
{
    if ( !pManager )
        pManager = new SuspendManager();
    return pManager;
}

void SuspendManager::cleanup()
{
    if ( pManager )
    {
        delete pManager;
        pManager = NULL;
    }
}

void SuspendManager::event( SuspendManager::Event e )
{
    for ( int i=0; i<transitions.size(); ++i )
    {
        Transition& t = transitions[i];
        if ( t.oldState == state && t.event == e )
        {
            state = t.newState;
            if ( pCallback_ )
                pCallback_->onStateChanged( t );
            break;
        }
    }
}

void SuspendManager::setSuspendTimeout( int val )
{
    suspendTimeout_ = val;
    if ( 0 == suspendTimeout_ )
        stop();
    else
        start(suspendTimeout_);
}

void SuspendManager::activity()
{
    if ( suspendTimeout_ != 0 && ssActive == state )
        start( suspendTimeout_ );
}

void SuspendManager::setCallBack( SuspendManagerCallback* pCallback )
{ 
    pCallback_ = pCallback; 
}

