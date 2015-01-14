#include "Platform.h"
#include "mainwindow.h"
#include "orientation.h"
#include "StyleEbook.h"
#include "props.h"

int z_verbose=0;
extern "C" void z_error(char * msg);
void z_error(char * msg) {
    fprintf(stderr, "%s\n", msg);
    exit(1);
}

MyApplication *pMyApp;

int main(int argc, char *argv[])
{
    int res = 0;
    {
        lString16 exedir = LVExtractPath(LocalToUnicode(lString8(argv[0])));
        LVAppendPathDelimiter(exedir);
        lString16 datadir = exedir + lString16("data");
        LVAppendPathDelimiter(datadir);

        lString16Collection fontDirs;
        fontDirs.add(qt2cr(qgetenv("QT_QWS_FONTDIR")));

#if defined(KOBO)
        fontDirs.add(lString16("/mnt/onboard/fonts"));
#endif

        CRPropRef props = LVCreatePropsContainer();
        {
            LVStreamRef cfg = LVOpenFileStream(UnicodeToUtf8(datadir + lString16("cr3.ini")).data(), LVOM_READ);
            if (!cfg.isNull())
                props->loadFromStream(cfg.get());
        }

        lString16 lang = props->getStringDef(PROP_APP_UI_LANG, "");
        InitCREngineLog(props);

        if (!InitCREngine(argv[0], fontDirs))
        {
            printf("Cannot init CREngine - exiting\n");
            return 2;
        }
        // to catch crashes and remove current cache file on crash (SIGSEGV etc.)
        // TODO crSetSignalHandler();

        // initialize resources in the libraries
        Q_INIT_RESOURCE(keyboard);
        Q_INIT_RESOURCE(eink);

        QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
        MyApplication a(argc, argv);
        pMyApp = &a;

        StyleEbook* pStyle = new StyleEbook(a.style());
        pStyle->setScrollBarSize(props->getIntDef(PROP_APP_UI_SCROLLBAR_SIZE, 40));
        pStyle->setButtonIconSize(props->getIntDef(PROP_APP_UI_BUTTON_ICON_SIZE, 48));
        a.setStyle(pStyle);

        QFont fnt(a.font());
        fnt.setPointSize(props->getIntDef(PROP_APP_UI_DEF_FONT_SIZE, 12));
        a.setFont(fnt);

        QMap<QString, QString> langToCode;
        langToCode["Russian"]   = "ru";
        langToCode["French"]    = "fr";
        langToCode["Hungarian"] = "hu";
        langToCode["Italian"]   = "it";
        langToCode["German"]    = "de";
        langToCode["Ukrainian"] = "uk";
        langToCode["Bulgarian"] = "bg";
        langToCode["Romanian"]  = "ro";
        langToCode["Spanish"]   = "es";
        langToCode["Chinese"]   = "zh_CN";

        QTranslator myTranslator, qtTranslator;
        if (!lang.empty() && lang.compare("English"))
        {
            QString lng = cr2qt(lang);
            if (myTranslator.load(lng, Platform::get()->getLocalTranslationsPath()))
            {
                // default translator for Qt standard dialogs
                if (qtTranslator.load("qt_" + langToCode[lng], Platform::get()->getCommonTranslationsPath()))
                {
                    QApplication::installTranslator(&qtTranslator);
                }
                // load after default to allow overriding translations
                QApplication::installTranslator(&myTranslator);
            }
        }

        MainWindow mainWin;
        a.setMainWindow(&mainWin);
        mainWin.showFullScreen();
        mainWin.doStartupActions();
        res = a.exec();
    }

    ShutdownCREngine();

    return res;
}

void ShutdownCREngine()
{
    HyphMan::uninit();
    ShutdownFontManager();
    CRLog::setLogger(NULL);
}

bool getDirectoryFonts( lString16Collection & pathList, lString16Collection & ext, lString16Collection & fonts, bool absPath )
{
    int foundCount = 0;
    lString16 path;
    for ( unsigned di=0; di<pathList.length();di++ ) {
        path = pathList[di];
        LVContainerRef dir = LVOpenDirectory(path.c_str());
        if ( !dir.isNull() ) {
            CRLog::trace("Checking directory %s", UnicodeToUtf8(path).c_str() );
            for ( int i=0; i < dir->GetObjectCount(); i++ ) {
                const LVContainerItemInfo * item = dir->GetObjectInfo(i);
                lString16 fileName = item->GetName();
                lString8 fn = UnicodeToLocal(fileName);
                    //printf(" test(%s) ", fn.c_str() );
                if ( !item->IsContainer() ) {
                    bool found = false;
                    lString16 lc = fileName;
                    lc.lowercase();
                    for ( unsigned j=0; j<ext.length(); j++ ) {
                        if ( lc.endsWith(ext[j]) ) {
                            found = true;
                            break;
                        }
                    }
                    if ( !found )
                        continue;
                    lString16 fn;
                    if ( absPath ) {
                        fn = path;
                        if ( !fn.empty() && fn[fn.length()-1]!=PATH_SEPARATOR_CHAR)
                            fn << PATH_SEPARATOR_CHAR;
                    }
                    fn << fileName;
                    foundCount++;
                    fonts.add( fn );
                }
            }
        }
    }
    return foundCount > 0;
}

bool InitCREngine( const char * exename, lString16Collection & fontDirs)
{
    CRLog::trace("InitCREngine(%s)", exename);

    InitFontManager(lString8::empty_str);

    // Load font definitions into font manager
    // fonts are in files font1.lbf, font2.lbf, ... font32.lbf
    // use fontconfig

    lString16Collection fontExt;
    fontExt.add(lString16(".ttf"));
    fontExt.add(lString16(".otf"));
    fontExt.add(lString16(".pfa"));
    fontExt.add(lString16(".pfb"));

    lString16Collection fonts;

    getDirectoryFonts( fontDirs, fontExt, fonts, true );
    // load fonts from file
    CRLog::debug("%d font files found", fonts.length());
    if (!fontMan->GetFontCount()) {
        for (unsigned fi=0; fi<fonts.length(); fi++ ) {
            lString8 fn = UnicodeToLocal(fonts[fi]);
            CRLog::trace("loading font: %s", fn.c_str());
            if ( !fontMan->RegisterFont(fn) )
                CRLog::trace("    failed\n");
        }
    }

    if (!fontMan->GetFontCount()) {
        printf("Fatal Error: Cannot open font file(s) .ttf \nCannot work without font\n" );
        return false;
    }
    printf("%d fonts loaded.\n", fontMan->GetFontCount());
    return true;
}

void InitCREngineLog(CRPropRef props)
{
    if(props.isNull())
    {
        CRLog::setStdoutLogger();
        CRLog::setLogLevel( CRLog::LL_FATAL);
        return;
    }
    lString16 logfname = props->getStringDef(PROP_LOG_FILENAME, "stdout");
    lString16 loglevelstr = props->getStringDef(PROP_LOG_LEVEL, "OFF");
    bool autoFlush = props->getBoolDef(PROP_LOG_AUTOFLUSH, false);

    CRLog::log_level level = CRLog::LL_INFO;
    if ( loglevelstr==lString16("OFF") )
    {
        level = CRLog::LL_FATAL;
        logfname.clear();
    }
    else
    if ( loglevelstr==lString16("FATAL") )
    {
        level = CRLog::LL_FATAL;
    }
    else
    if ( loglevelstr==lString16("ERROR") )
    {
        level = CRLog::LL_ERROR;
    }
    else
    if ( loglevelstr==lString16("WARN") )
    {
        level = CRLog::LL_WARN;
    }
    else
    if ( loglevelstr==lString16("INFO") )
    {
        level = CRLog::LL_INFO;
    }
    else
    if ( loglevelstr==lString16("DEBUG") )
    {
        level = CRLog::LL_DEBUG;
    }
    else
    if ( loglevelstr==lString16("TRACE") )
    {
        level = CRLog::LL_TRACE;
    }
    if ( !logfname.empty() )
    {
        if ( logfname==lString16("stdout") )
            CRLog::setStdoutLogger();
        else if ( logfname==lString16("stderr") )
            CRLog::setStderrLogger();
        else
            CRLog::setFileLogger(UnicodeToUtf8( logfname ).c_str(), autoFlush);
    }
    CRLog::setLogLevel(level);
    CRLog::trace("Log initialization done.");
}
