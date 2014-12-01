#ifndef H_SCREEN_MANAGER
#define H_SCREEN_MANAGER

class ScreenManagerCallback;

class ScreenManager
{
    static ScreenManager* pManager;

    ScreenManagerCallback* pCallback_;

    char allowFullUpdate;

private:
    ScreenManager();

public:
    static ScreenManager* instanse();
    static void cleanup();
    bool getAllowFullUpdate() const;
    void setAllowFullUpdate( bool val );
};

#endif // H_SCREEN_MANAGER
