#include "Platform.h"

#include "props.h"
#include "frontlightdlg.h"
#include "rotationdlg.h"
#include "gotodialog.h"
#include "actiondisplayer.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"

#define DOC_CACHE_SIZE 128 * 0x100000
#define ENABLE_BOOKMARKS_DIR 1

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWindowClass)
    , menu(0)
    , menuActive(false)
    , lastPage(0)
    , updateCounter(1)
    , updateInterval(1)
    , gc(this)
{      
    ui->setupUi(this);

    QString dataDir = qApp->applicationDirPath() + QDir::toNativeSeparators(QString("/data/"));
    QString cacheDir = dataDir + "cache";
    QString bookmarksDir = dataDir + "bookmarks";
    QString histFile = dataDir + "cr3hist.bmk";
    QString iniFile = dataDir + "cr3.ini";
    QString cssFile = dataDir + "fb2.css";
    QString hyphDir = dataDir + "hyph" + QDir::separator();

    ui->view->setHyphDir(hyphDir);

    ldomDocCache::init(qt2cr(cacheDir), DOC_CACHE_SIZE);

    ui->view->setPropsChangeCallback(this);
    ui->view->loadSettings(iniFile);
    ui->view->loadHistory(histFile);
    ui->view->loadCSS(cssFile);
#if ENABLE_BOOKMARKS_DIR==1
    ui->view->setBookmarksDir(bookmarksDir);
#endif

    ui->view->getDocView()->setBatteryState(Platform::get()->getBatteryLevel());

    updateCounter = updateInterval = ui->view->getOptions()->getIntDef(PROP_DISPLAY_FULL_UPDATE_INTERVAL, 1);

    // Fill action list
    aList.push_back(ui->actionEmpty);        // 0
    aList.push_back(ui->actionShowMenu);     // 1
    aList.push_back(ui->actionBack);         // 2
    aList.push_back(ui->actionForward);      // 3
    aList.push_back(ui->actionNextPage);     // 4
    aList.push_back(ui->actionPreviousPage); // 5
    aList.push_back(ui->actionNext10Pages);
    aList.push_back(ui->actionPrevious10Pages);
    aList.push_back(ui->actionNextChapter);
    aList.push_back(ui->actionPreviousChapter);
    aList.push_back(ui->actionFirstPage);
    aList.push_back(ui->actionLastPage);
    aList.push_back(ui->actionPosition);
    aList.push_back(ui->actionOpen);
    aList.push_back(ui->actionClose);
    aList.push_back(ui->actionTOC);
    aList.push_back(ui->actionRecentBooks);
    aList.push_back(ui->actionSettings);
    aList.push_back(ui->actionAddBookmark);
    aList.push_back(ui->actionShowBookmarksList);
    aList.push_back(ui->actionFileProperties);
    aList.push_back(ui->actionScreenRotation);
    aList.push_back(ui->actionFrontlight);
    aList.push_back(ui->actionFrontlightToggle);
    aList.push_back(ui->actionFrontlightPlus);
    aList.push_back(ui->actionFrontlightMinus);   
    aList.push_back(ui->actionZoomIn);
    aList.push_back(ui->actionZoomOut);
    aList.push_back(ui->actionToggleInversion);
    aList.push_back(ui->actionToggleHeader);
    aList.push_back(ui->actionSuspend);
    aList.push_back(ui->actionScreenRotation0);
    aList.push_back(ui->actionScreenRotation90);
    aList.push_back(ui->actionScreenRotation180);
    aList.push_back(ui->actionScreenRotation270);
    aList.push_back(ui->actionScreenRotationPlus90);
    aList.push_back(ui->actionScreenRotationPlus180);
    aList.push_back(ui->actionScreenRotationMinus90);
    aList.push_back(ui->actionDeleteCurrentDocument);
    aList.push_back(ui->actionDictionary);

    PropsRef props( ui->view->getOptions() );
    // use right column for next page by default
    setDefaultTapZoneAction( props,0,2,0,4 );
    setDefaultTapZoneAction( props,0,2,1,4 );
    setDefaultTapZoneAction( props,0,2,2,4 );
    // use left column for previous page by default
    setDefaultTapZoneAction( props,0,0,0,5 );
    setDefaultTapZoneAction( props,0,0,1,5 );
    setDefaultTapZoneAction( props,0,0,2,5 );
    // use center column for menu by default
    setDefaultTapZoneAction( props,0,1,0,1 );
    setDefaultTapZoneAction( props,0,1,1,1 );
    setDefaultTapZoneAction( props,0,1,2,1 );

    ui->view->setOptions(props);

    gc.setRotation( props->getIntDef(PROP_ROTATE_ANGLE, 0) );
    gc.setWeights( QSize(props->getIntDef(PROP_APP_UI_SWIPES_X_WEIGHT, 1), props->getIntDef(PROP_APP_UI_SWIPES_Y_WEIGHT, 1)) );

    QObject::connect( &gc, SIGNAL(sigGesture(QPoint,GesturesController::GestureType)), this, SLOT(onGesture(QPoint,GesturesController::GestureType)) );
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::connectSystemBus() 
{}

void MainWindow::disconnectSystemBus() 
{}

void MainWindow::battLevelChanged(int)
{}

void MainWindow::goingToScreenSaver()
{}

void MainWindow::outOfScreenSaver()
{}

void MainWindow::usbDriveConnected()
{}

void MainWindow::usbDriveDisconnected()
{}

void MainWindow::on_actionOpen_triggered()
{
    QString path(getOpenFilePath());
    QString fn;
    if ( ui->view->GetLastPathName( &fn ) )
    {
        QFileInfo fi(fn);
        if ( fi.absolutePath() == path )
        {
            fn = fi.fileName();
        }
        else
        {
            fn.clear();
        }
    }

    OpenFileDlg dlg(this, ui->view, path, fn);
    if ( dlg.exec() && !dlg.fileName.isEmpty() )
    {
        ui->view->loadDocument( dlg.fileName );
    }
}

void MainWindow::on_actionClose_triggered()
{
    // Necessary to hide the Dictionary top-level window
    // before close the main top-level window in order the application
    // can exit correctly
    ui->view->onTranslateFinish();
    close();
}

void MainWindow::on_actionNextPage_triggered()
{
    handlePageTurnScreenUpdate();
    ui->view->doCommand(DCMD_PAGEDOWN, 1);
    printf("PAGE++(%d) ; battery: %d", updateCounter, Platform::get()->getBatteryLevel()); fflush(stdout);
}

void MainWindow::on_actionPreviousPage_triggered()
{
    handlePageTurnScreenUpdate();
    ui->view->doCommand(DCMD_PAGEUP, 1);
    printf("PAGE--(%d) ; battery: %d", updateCounter, Platform::get()->getBatteryLevel()); fflush(stdout);
}

void MainWindow::on_actionTOC_triggered()
{
    TocDlg(this, ui->view).exec();
}

void MainWindow::on_actionRecentBooks_triggered()
{
    RecentBooksDlg dlg(this, ui->view);
    if ( dlg.exec() && !dlg.fileName.isEmpty() )
        ui->view->loadDocument(dlg.fileName);
}

void MainWindow::on_actionSettings_triggered()
{
    SettingsDlg(this, ui->view, aList).exec();

    updateCounter = updateInterval = ui->view->getOptions()->getIntDef(PROP_DISPLAY_FULL_UPDATE_INTERVAL, 1);
}

void MainWindow::contextMenu( QPoint pos )
{
    if (!menu) 
    {
        menu = new MainMenu(this);
        menu->addAction(ui->actionOpen);
        menu->addAction(ui->actionRecentBooks);
        menu->addAction(ui->actionTOC);
        QMenu* pMenuNavigation = menu->addMenu(tr("Navigation"));
        pMenuNavigation->addAction(ui->actionNext10Pages);
        pMenuNavigation->addAction(ui->actionPrevious10Pages);
        pMenuNavigation->addSeparator();
        pMenuNavigation->addAction(ui->actionNextChapter);
        pMenuNavigation->addAction(ui->actionPreviousChapter);
        pMenuNavigation->addSeparator();
        pMenuNavigation->addAction(ui->actionFirstPage);
        pMenuNavigation->addAction(ui->actionLastPage);
        pMenuNavigation->addSeparator();
        pMenuNavigation->addAction(ui->actionBack);
        pMenuNavigation->addAction(ui->actionForward);
        pMenuNavigation->addSeparator();
        pMenuNavigation->addAction(ui->actionPosition);
        menu->addAction(ui->actionFindText);
        menu->addAction(ui->actionShowBookmarksList);
        menu->addAction(ui->actionAddBookmark);
        menu->addAction(ui->actionFileProperties);
        menu->addSeparator();
        menu->addAction(ui->actionDictionary);
        menu->addSeparator();
        menu->addAction(ui->actionFrontlight);
        menu->addAction(ui->actionScreenRotation);
        menu->addAction(ui->actionSettings);
        menu->addSeparator();
        menu->addAction(ui->actionClose);
    }

    menuActive = true;
    menu->exec(ui->view->mapToGlobal(pos));
    menuActive = false;
}

void MainWindow::doStartupActions()
{
    switch(ui->view->getOptions()->getIntDef(PROP_APP_START_ACTION, 0))
    {
    case 0:
        // open recent book
        ui->view->loadLastDocument();
        break;
    case 1:
        on_actionRecentBooks_triggered();
        break;
    case 2:
        // show file open dialog
        on_actionOpen_triggered();
        break;
    }
}

void MainWindow::on_actionAddBookmark_triggered()
{
    ui->view->createBookmark();
    // will delete itself on close
    new ActionDisplayer(this, ui->view, tr("Bookmark created"));
}

void MainWindow::on_actionShowBookmarksList_triggered()
{
    BookmarkListDialog(this, ui->view).exec();
}

void MainWindow::on_actionFileProperties_triggered()
{
    FilePropsDialog(this, ui->view).exec();
}

void MainWindow::on_actionFindText_triggered()
{
    SearchDialog dlg(this);
    if ( dlg.exec() )
    {
        if ( !dlg.text.isEmpty() )
        {
            bool found = false;
            found = findText(dlg.text, 1, SearchDialog::reverse, !SearchDialog::caseSensitive);
            if ( !found )
                found = findText(dlg.text, -1, SearchDialog::reverse, !SearchDialog::caseSensitive);
            if ( !found )
            {
                QMessageBox * mb = new QMessageBox( QMessageBox::Information, tr("Not found"), tr("Search pattern is not found in document"), QMessageBox::Close, this );
                mb->exec();
                if ( !ui->view->getDocView()->getMarkedRanges()->empty() )
                {
                    ui->view->getDocView()->clearSelection();
                    ui->view->update();
                }
            }
            else
            {
                ui->view->update();
            }
        }
        else
        if ( !ui->view->getDocView()->getMarkedRanges()->empty() )
        {
            ui->view->getDocView()->clearSelection();
            ui->view->update();
        }
    }
}

void MainWindow::on_actionShowMenu_triggered()
{
    contextMenu(QPoint(10,10));
}

void MainWindow::on_actionEmpty_triggered()
{
    QAction *a = qobject_cast<QAction *>(sender());
    if(a) {
        QString cmdstr = a->text();
        int cmd = 0;
        int param = 0;
        int pos = cmdstr.indexOf("|");
        if(pos!=-1) {
            param = cmdstr.mid(pos+1).toInt();
            cmd = cmdstr.mid(0, pos).toInt();
        } else cmd = cmdstr.toInt();
        doCommand(cmd, param);
    }
}

bool MainWindow::isFocusInReader()
{
    return hasFocus() && !menuActive;
}

void MainWindow::keyReleaseEvent(QKeyEvent* event)
{
    if ( !event->isAutoRepeat() )
    {
        QString key(kmap.map( event->key()));
        if ( !key.isEmpty() )
        {
            QString propName( QString( PROP_APP_UI_KEY_MAPPING ).arg(key));
            int action = ui->view->getOptions()->getIntDef(propName.toAscii().constData(), 0);
            if ( action != 0 && action < aList.size() )
            {
                aList.at(action)->trigger();
            }
            else
            switch ( event->key() )
            {
            case Qt::Key_Menu:
                on_actionShowMenu_triggered();
                break;
            case Qt::Key_PageUp:
                on_actionPreviousPage_triggered();
                break;
            case Qt::Key_PageDown:
                on_actionNextPage_triggered();
                break;
            default:
                break;
            }
        }
    }
}

void MainWindow::handlePageTurnScreenUpdate()
{
    if ( 0 == updateInterval || --updateCounter > 0 )
        disableFullUpdate();
    else
        updateCounter = updateInterval;
}

void MainWindow::disableFullUpdate()
{
    writeFifoCommand("dfu");
}

void MainWindow::processTapZoneAction( int type, const QPoint& pos )
{
    static const int SIZE = 3;

    int w = width()  / SIZE;
    int h = height() / SIZE;

    int x = pos.x() / w;
    int y = pos.y() / h;

    if ( x < 0 ) x = 0;
    if ( y < 0 ) y = 0;
    if ( x >= SIZE ) x = SIZE-1;
    if ( y >= SIZE ) y = SIZE-1;

    int rot = ui->view->getOptions()->getIntDef(PROP_ROTATE_ANGLE, 0);
    switch ( rot )
    {
    case 1:
        x = SIZE - 1 - x;
        std::swap(x,y);
        break;
    case 2:
        y = SIZE - 1 - y;
        x = SIZE - 1 - x;
        break;
    case 3:
        y = SIZE - 1 - y;
        std::swap(x,y);
        break;
    default:
        break;
    }

    int action = ui->view->getOptions()->getIntDef(aList.getTapZonesParamName(type,x,y).toAscii().constData(), 0);
    if ( action != 0 && action < aList.size() )
    {
        aList.at(action)->trigger();
    }
}

void MainWindow::processSwipesAction( int type )
{
    static const int matrix[4][4] =
    { { 0,1,2,3 }, { 3,2,0,1 }, { 1,0,3,2 }, { 2,3,1,0 } };

    if ( type < 0 || type > 3) return;

    int rot = ui->view->getOptions()->getIntDef(PROP_ROTATE_ANGLE, 0);

    int action = ui->view->getOptions()->getIntDef(aList.getSwipesParamName(matrix[rot][type]).toAscii().constData(), 0);
    if ( action != 0 && action < aList.size() )
    {
        aList.at(action)->trigger();
    }
}

bool MainWindow::findText( const QString& pattern, int origin, bool reverse, bool caseInsensitive )
{
    if ( pattern.isEmpty() )
        return false;
    if ( pattern != SearchDialog::lastText && origin == 1 )
        origin = 0;
    SearchDialog::lastText = pattern;
    LVArray<ldomWord> words;
    lvRect rc;
    ui->view->getDocView()->GetPos( rc );
    int pageHeight = rc.height();
    int start = -1;
    int end = -1;
    if ( reverse )
    {
        // reverse
        if ( origin == 0 )
        {
            // from end current page to first page
            end = rc.bottom;
        }
        else
        if ( origin == -1 )
        {
            // from last page to end of current page
            start = rc.bottom;
        }
        else
        {   // origin == 1
            // from prev page to first page
            end = rc.top;
        }
    }
    else
    {
        // forward
        if ( origin == 0 )
        {
            // from current page to last page
            start = rc.top;
        }
        else
        if ( origin == -1 )
        {
            // from first page to current page
            end = rc.top;
        }
        else
        {   // origin == 1
            // from next page to last
            start = rc.bottom;
        }
    }
    CRLog::debug("CRViewDialog::findText: Current page: %d .. %d", rc.top, rc.bottom);
    CRLog::debug("CRViewDialog::findText: searching for text '%s' from %d to %d origin %d", pattern.toUtf8().constData(), start, end, origin );
    if ( ui->view->getDocView()->getDocument()->findText( qt2cr(pattern), caseInsensitive, reverse, start, end, words, 200, pageHeight ) )
    {
        CRLog::debug("CRViewDialog::findText: pattern found");
        ui->view->getDocView()->clearSelection();
        ui->view->getDocView()->selectWords( words );
        ldomMarkedRangeList * ranges = ui->view->getDocView()->getMarkedRanges();
        if ( ranges )
        {
            if ( ranges->length()>0 )
            {
                int pos = ranges->get(0)->start.y;
                if ( pos != ui->view->getDocView()->GetPos() )
                {
                    ui->view->getDocView()->savePosToNavigationHistory();
                    ui->view->getDocView()->SetPos(pos);
                }
            }
        }
        return true;
    }
    CRLog::debug("CRViewDialog::findText: pattern not found");
    return false;
}

void MainWindow::setDefaultTapZoneAction(PropsRef props, int type, int x, int y, int action)
{
    int a = 0;
    QString name(aList.getTapZonesParamName(type,x,y));
    if (!props->getInt(name.toAscii().constData(), a))
    {
        props->setInt(name.toAscii().constData(), action);
    }
}

void MainWindow::doCommand(int cmd, int param)
{
    switch(cmd) 
    {
    case CMD_TOGGLE_HEADER:
        ui->view->toggleProperty(PROP_STATUS_LINE);
        break;
    case CMD_REFRESH:
        ui->view->repaint();
        break;
    case CMD_ZOOM_FONT:
        ui->view->zoomFont(param);
        break;
    case CMD_ZOOM_HEADER_FONT:
        ui->view->zoomHeaderFont(param);
        break;
    case CMD_JUMP_FROM_PAGE: {
        int pagenum = ui->view->getCurPage() + param;
        ui->view->doCommand(DCMD_GO_PAGE, pagenum);
        ui->view->update();
        break;
    }
    case CMD_CHANGE_FONT_GAMMA:
        ui->view->ChangeFontGamma(param);
        break;
    case CMD_CHANGE_FONT:
        ui->view->ChangeFont(param);
        break;
    case CMD_CHANGE_HEADER_FONT:
        ui->view->ChangeHeaderFont(param);
        break;
    case CMD_INTERLINE_SPACE:
        ui->view->ChangeInterlineSpace(param);
        break;
    case CMD_EXIT:
        on_actionClose_triggered();
        break;
    case DCMD_ROTATE_BY:
        ui->view->Rotate(param);
        break;
    case DCMD_LINK_BACK:
    case DCMD_LINK_FORWARD:
        ui->view->doCommand(cmd, 1);
        break;
    case DCMD_PAGEDOWN:
    case DCMD_PAGEUP:
    case DCMD_BEGIN:
    case DCMD_END:
    case DCMD_MOVE_BY_CHAPTER:
    case DCMD_TOGGLE_BOLD:
        ui->view->doCommand(cmd, param);
    }
}

QString MainWindow::getOpenFilePath()
{
    QString path;
    if ( !ui->view->GetLastPathName( &path ) )
    {
#if defined(KINDLE)
        path = "/mnt/us";
#elif defined(KOBO)
        path = "/mnt/onboard";
#elif defined(INKVIEW)
        path = "/mnt/ext1";
#else
        path = "/mnt";
#endif
    }
    else
    {
        path = QFileInfo(path).absolutePath();
    }
    return path;
}

void MainWindow::writeFifoCommand(const char* cmd)
{
    QFile file(qgetenv("VLASOVSOFT_FIFO"));
    if ( file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Unbuffered) )
    {
        file.write(cmd);
        file.close();
    }
}

void MainWindow::on_actionScreenRotation_triggered()
{
    int angle = ui->view->getOptions()->getIntDef(PROP_ROTATE_ANGLE, 0);
    RotationDlg rd(this, angle);
    rd.exec(false);
    setRotation(rd.getRotation());
}

void MainWindow::on_actionFrontlight_triggered()
{
    writeFifoCommand("fld");
}

void MainWindow::on_actionFrontlightToggle_triggered()
{
    writeFifoCommand("flt");
}

void MainWindow::on_actionFrontlightPlus_triggered()
{
    writeFifoCommand("fl+");
}

void MainWindow::on_actionFrontlightMinus_triggered()
{
    writeFifoCommand("fl-");
}

void MainWindow::on_actionFirstPage_triggered()
{
    ui->view->doCommand( DCMD_BEGIN, 1 );
}

void MainWindow::on_actionLastPage_triggered()
{
    ui->view->doCommand( DCMD_END, 1 );
}

void MainWindow::on_actionNextChapter_triggered()
{
    ui->view->doCommand( DCMD_MOVE_BY_CHAPTER, 1 );
}

void MainWindow::on_actionPreviousChapter_triggered()
{
    ui->view->doCommand( DCMD_MOVE_BY_CHAPTER, -1 );
}

void MainWindow::on_actionBack_triggered()
{
    ui->view->doCommand( DCMD_LINK_BACK, 1 );
}

void MainWindow::on_actionForward_triggered()
{
    ui->view->doCommand( DCMD_LINK_FORWARD, 1 );
}

void MainWindow::on_actionPrevious10Pages_triggered()
{
    handlePageTurnScreenUpdate();
    ui->view->doCommand(DCMD_PAGEUP, 10);
    printf("PAGE-10(%d) ; battery: %d", updateCounter, Platform::get()->getBatteryLevel()); fflush(stdout);
}

void MainWindow::on_actionNext10Pages_triggered()
{
    handlePageTurnScreenUpdate();
    ui->view->doCommand(DCMD_PAGEDOWN, 10);
    printf("PAGE+10(%d) ; battery: %d", updateCounter, Platform::get()->getBatteryLevel()); fflush(stdout);
}

void MainWindow::on_actionZoomIn_triggered()
{
    ui->view->zoomFont(+1);
}

void MainWindow::on_actionZoomOut_triggered()
{
    ui->view->zoomFont(-1);
}

void MainWindow::on_actionPosition_triggered()
{
    GoToDialog dlg(this, 1+ui->view->getCurPage(), ui->view->getPageCount());
    if ( dlg.exec() && dlg.getCurrentPage()-1 != ui->view->getCurPage() )
    {
        ui->view->GoToPage(dlg.getCurrentPage()-1);
    }
}

void MainWindow::on_actionToggleInversion_triggered()
{
    ui->view->toggleProperty(PROP_DISPLAY_INVERSE);
}

void MainWindow::on_actionToggleHeader_triggered()
{
    ui->view->toggleProperty(PROP_STATUS_LINE);
}

void MainWindow::on_actionSuspend_triggered()
{
    writeFifoCommand("suspend");
}

void MainWindow::onGesture(QPoint pt, GesturesController::GestureType t)
{
    switch ( t )
    {
    case GesturesController::gtTapShort:
        if ( ui->view->isTranslationMode() )
            ui->view->onTranslateTap(pt);
        else
            processTapZoneAction(0, pt);
        break;
    case GesturesController::gtTapLong:
        {
            lvPoint lvpt (pt.x(), pt.y());
            ldomXPointer p(ui->view->getDocView()->getNodeByPoint( lvpt ));
            lString16 href(p.getHRef());
            if ( !href.empty() )
            {
                if ( ui->view->getDocView()->goLink( href ) )
                {
                    ui->view->update();
                }
            }
            else
            {
                processTapZoneAction(1, pt);
            }
        }
        break;
    case GesturesController::gtSwipeBT:
    case GesturesController::gtSwipeTB:
    case GesturesController::gtSwipeLR:
    case GesturesController::gtSwipeRL:
        processSwipesAction( t );
        break;
    default:
        break;
    }
}

void MainWindow::on_actionScreenRotation0_triggered()
{
    setRotation(0);
}

void MainWindow::on_actionScreenRotation90_triggered()
{
    setRotation(1);
}

void MainWindow::on_actionScreenRotation180_triggered()
{
    setRotation(2);
}

void MainWindow::on_actionScreenRotation270_triggered()
{
    setRotation(3);
}

void MainWindow::setRotation(int val)
{
    PropsRef props( ui->view->getOptions() );
    props->setInt(PROP_ROTATE_ANGLE, val);
    ui->view->setOptions(props);
    gc.setRotation( val );
}

void MainWindow::on_actionScreenRotationPlus90_triggered()
{
    int angle = ui->view->getOptions()->getIntDef( PROP_ROTATE_ANGLE, 0 );
    angle = (angle + 1) % 4;
    setRotation(angle);
}

void MainWindow::on_actionScreenRotationMinus90_triggered()
{
    int angle = ui->view->getOptions()->getIntDef( PROP_ROTATE_ANGLE, 0 );
    angle = angle - 1;
    if ( angle < 0 ) angle = 3;
    setRotation(angle);
}

void MainWindow::on_actionScreenRotationPlus180_triggered()
{
    int angle = ui->view->getOptions()->getIntDef( PROP_ROTATE_ANGLE, 0 );
    angle = (angle + 2) % 4;
    setRotation(angle);
}

void MainWindow::on_actionDeleteCurrentDocument_triggered()
{
    LVPtrVector<CRFileHistRecord>&
            files = ui->view->getDocView()->getHistory()->getRecords();

    if ( !files.empty() && ui->view->getDocView()->isDocumentOpened() )
    {
        QMessageBox::StandardButton result =
                 ::questionBox( this, tr("Delete"), tr("Do you really want to delete the current document?"),
                                QMessageBox::Yes|QMessageBox::No,
                                QMessageBox::No);

         if ( QMessageBox::Yes == result )
         {
             ui->view->getDocView()->createDefaultDocument( qt2cr(tr("Removed")), qt2cr(tr("Document was removed") ) );
             QString fileName(cr2qt(files[0]->getFilePathName()));
             files.remove(0);
             QFile::remove(fileName);
             update();
         }
    }
}

void MainWindow::on_actionDictionary_triggered()
{
    ui->view->onTranslateStart();
}
