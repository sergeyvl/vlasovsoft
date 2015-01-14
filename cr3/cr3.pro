TARGET = cr3
VERSION = 0.1.4
TEMPLATE = app

include(../common/common.pri)

DEFINES += USE_FONTCONFIG=0 USE_FREETYPE=1 LDOM_USE_OWN_MEM_MAN=1 COLOR_BACKBUFFER=1 USE_DOM_UTF8_STORAGE=1 NDEBUG=1 QT_KEYPAD_NAVIGATION

INCLUDEPATH += $$(QTDIR)/include/freetype2 include \
    ../../crengine/thirdparty/antiword

SOURCES += main.cpp \
    mainwindow.cpp \
    searchdlg.cpp \
    filepropsdlg.cpp \
    cr3widget.cpp \
    crqtutil.cpp \
    tocdlg.cpp \
    recentdlg.cpp \
    settings.cpp \
    bookmarklistdlg.cpp \
    openfiledlg.cpp \
    StyleEbook.cpp \
    rotationdlg.cpp \
    orientation.cpp \
    actionlist.cpp \
    mainmenu.cpp \
    gotodialog.cpp \
    progbareventfilter.cpp \
    navigationbar.cpp \
    keymapper.cpp \
    actiondisplayer.cpp \
    recentremovedlg.cpp \
    dictionary.cpp

HEADERS += mainwindow.h \
    cr3widget.h \
    crqtutil.h \
    tocdlg.h \
    recentdlg.h \
    settings.h \
    bookmarklistdlg.h \
    searchdlg.h \
    filepropsdlg.h \
    openfiledlg.h \
    StyleEbook.h \
    rotationdlg.h \
    orientation.h \
    actionlist.h \
    mainmenu.h \
    gotodialog.h \
    progbareventfilter.h \
    navigationbar.h \
    props.h \
    keymapper.h \
    actiondisplayer.h \
    recentremovedlg.h \
    dictionary.h

FORMS += mainwindow.ui \
    tocdlg.ui \
    recentdlg.ui \
    settings.ui \
    bookmarklistdlg.ui \
    searchdlg.ui \
    filepropsdlg.ui \
    openfiledlg.ui \
    rotationdlg.ui \
    gotodialog.ui \
    navigationbar.ui \
    recentremovedlg.ui \
    dictionary.ui

TRANSLATIONS +=       \
    i18n/Russian.ts   \
    i18n/Ukrainian.ts \
    i18n/French.ts    \
    i18n/Italian.ts   \
    i18n/Hungarian.ts \
    i18n/German.ts    \
    i18n/Romanian.ts  \
    i18n/Spanish.ts   \
    i18n/Bulgarian.ts \
    i18n/Catalan.ts   \
    i18n/Chinese.ts

RESOURCES += cr3.qrc

lrelease.input = TRANSLATIONS + QT_TRANSLATIONS
lrelease.output = $$DESTDIR/i18n/${QMAKE_FILE_BASE}.qm
lrelease.commands = $$[QT_INSTALL_BINS]/lrelease -silent -compress ${QMAKE_FILE_IN} -qm $$lrelease.output
lrelease.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += lrelease
DEPENDPATH += i18n

LIBS += -lQtGui -lQtCore -lQtNetwork -lpthread -ldl

LIBS += -Lqtbuild_$$ARCH/crengine \
        -Lqtbuild_$$ARCH/thirdparty/antiword \
        -Lqtbuild_$$ARCH/thirdparty/chmlib \
        -Lqtbuild_$$ARCH/thirdparty/libpng \
        -Lqtbuild_$$ARCH/thirdparty/libjpeg \
        -Lqtbuild_$$ARCH/thirdparty/freetype \
        -lcrengine -lantiword -lchmlib -lpng -ljpeg -lfreetype -lcommon
