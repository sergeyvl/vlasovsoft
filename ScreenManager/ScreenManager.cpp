#include <QDebug>

#include "ScreenManagerCallback.h"
#include "ScreenManager.h"

ScreenManager* ScreenManager::pManager = 0;

ScreenManager::ScreenManager()
    : pCallback_(0)
    , allowFullUpdate(1)
{
}

ScreenManager* ScreenManager::instanse()
{
    if ( !pManager )
        pManager = new ScreenManager();
    return pManager;
}

void ScreenManager::cleanup()
{
    if ( pManager )
    {
        delete pManager;
        pManager = 0;
    }
}
    
bool ScreenManager::getAllowFullUpdate() const
{
    return static_cast<bool>(allowFullUpdate);
}

void ScreenManager::setAllowFullUpdate( bool val  )
{
    allowFullUpdate = (0 != val);
}
