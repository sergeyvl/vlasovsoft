#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <signal.h>

#include <QApplication>
#include <QMainWindow>
#include <QProcess>
#include <QDebug>
#include <QDecorationDefault>
#include <QTranslator>
#include <QTextCodec>
#include <QClipboard>
#include <QWSServer>
#include <QTimer>
#include <QShortcut>

#include "cr3widget.h"
#include "crqtutil.h"
#include "crgui.h"

#include "crengine.h"
#include "lvstring.h"
#include "lvtinydom.h"

#include "settings.h"
#include "searchdlg.h"
#include "openfiledlg.h"
#include "tocdlg.h"
#include "recentdlg.h"
#include "bookmarklistdlg.h"
#include "filepropsdlg.h"
#include "frontlightdlg.h"
#include "actionlist.h"
#include "mainmenu.h"
#include "gesturescontroller.h"
#include "keymapper.h"

#define CMD_REFRESH                 1001
#define CMD_ZOOM_FONT               1002
#define CMD_ZOOM_HEADER_FONT        1003
#define CMD_TOGGLE_HEADER           1004
#define CMD_JUMP_FROM_PAGE          1005
#define CMD_CHANGE_FONT             1006
#define CMD_CHANGE_HEADER_FONT      1007
#define CMD_CHANGE_FONT_GAMMA       1008
#define CMD_INTERLINE_SPACE         1009
#define CMD_EXIT                    1010
#define CMD_MINIMIZE                1011

void InitCREngineLog(CRPropRef props);
bool InitCREngine(const char* exename, lString16Collection& fontDirs);
void ShutdownCREngine();
bool getDirectoryFonts( lString16Collection& pathList, lString16Collection& ext, lString16Collection& fonts, bool absPath );
void sigCatcher(int sig);
void wakeUp();
void gotoSleep();

namespace Ui
{
class MainWindowClass;
}

class MainWindow : public QWidget, public PropsChangeCallback
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    virtual void onPropsChange(PropsRef) {}
    void doStartupActions();

    void connectSystemBus();
    void disconnectSystemBus();
    bool isFocusInReader();

protected:
    virtual void keyReleaseEvent(QKeyEvent * event);

private:
    Ui::MainWindowClass* ui;
    MainMenu* menu;

    QString _filenameToOpen;
    CRGUIAcceleratorTableRef wndkeys;

    bool loadKeymaps(CRGUIWindowManager & winman, const char * locations[]);
    void doCommand(int cmd, int param = 0);

    bool menuActive;
    bool handleFl;

    int lastPage;
    int updateCounter;
    int updateInterval;

    ActionList aList;
    KeyMapper kmap;
    GesturesController gc;

private:
    QString getOpenFilePath();
    void writeFifoCommand( const char* cmd );
    void handlePageTurnScreenUpdate();
    void disableFullUpdate();
    void processTapZoneAction( int type, const QPoint& pos );
    void processSwipesAction( int type );
    bool findText( const QString& pattern, int origin, bool reverse, bool caseInsensitive );
    void setDefaultTapZoneAction( PropsRef props, int type, int x, int y, int action );

public slots:
    void contextMenu( QPoint pos );
    void on_actionFindText_triggered();

private slots:
    void on_actionFileProperties_triggered();
    void on_actionShowBookmarksList_triggered();
    void on_actionAddBookmark_triggered();
    void on_actionSettings_triggered();
    void on_actionRecentBooks_triggered();
    void on_actionTOC_triggered();
    void on_actionPreviousPage_triggered();
    void on_actionNextPage_triggered();
    void on_actionClose_triggered();
    void on_actionOpen_triggered();
    void on_actionShowMenu_triggered();
    void on_actionEmpty_triggered();

    void battLevelChanged(int fparam);
    void goingToScreenSaver();
    void outOfScreenSaver();
    void usbDriveConnected();
    void usbDriveDisconnected();
    void on_actionScreenRotation_triggered();

    void on_actionFrontlight_triggered();
    void on_actionFrontlightToggle_triggered();
    void on_actionFrontlightPlus_triggered();
    void on_actionFrontlightMinus_triggered();
    void on_actionFirstPage_triggered();
    void on_actionLastPage_triggered();
    void on_actionNextChapter_triggered();
    void on_actionPreviousChapter_triggered();
    void on_actionBack_triggered();
    void on_actionForward_triggered();
    void on_actionPrevious10Pages_triggered();
    void on_actionNext10Pages_triggered();
    void on_actionZoomIn_triggered();
    void on_actionZoomOut_triggered();
    void on_actionPosition_triggered();
    void on_actionToggleInversion_triggered();
    void on_actionToggleHeader_triggered();
    void on_actionSuspend_triggered();

    // gestures
    void onGesture(QPoint pt, GesturesController::GestureType t);

    // rotation
    void on_actionScreenRotation0_triggered();
    void on_actionScreenRotation90_triggered();
    void on_actionScreenRotation180_triggered();
    void on_actionScreenRotation270_triggered();
    void setRotation( int val );
    void on_actionScreenRotationPlus90_triggered();
    void on_actionScreenRotationMinus90_triggered();
    void on_actionScreenRotationPlus180_triggered();
    void on_actionDeleteCurrentDocument_triggered();
    void on_actionDictionary_triggered();
};

class MyApplication : public QApplication 
{
    Q_OBJECT
public:
    MyApplication(int& argc, char** argv)
        : QApplication(argc, argv)
    {
        QPalette palette;
        palette.setColor(QPalette::Window, Qt::white);
        palette.setColor(QPalette::Highlight, Qt::black);
        setPalette(palette);
        setNavigationMode(Qt::NavigationModeKeypadDirectional);
    }

    void setMainWindow(MainWindow* w) 
    {
        mainWindow = w;
    }

    void connectSystemBus() 
    {}

    void disconnectSystemBus() 
    {}

    MainWindow* getMainWindow()
    { return mainWindow; }

private:
    MainWindow* mainWindow;
};

#endif // MAINWINDOW_H
