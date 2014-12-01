TEMPLATE  = lib
CONFIG   += lib debug
CONFIG += dll plugin
SOURCES += ScreenManager.cpp
HEADERS += ScreenManager.h ScreenManagerCallback.h

kobo {
    ARCH = kobo
    DEFINES += KOBO
}
kindle {
    ARCH = kindle
    DEFINES += KINDLE
}   
x86_64_qws {
    ARCH = x86_64_qws
    DEFINES += X86
}
inkview_x86 {
    ARCH = inkview_x86
    DEFINES += X86 INKVIEW INKVIEW_PRO
}
inkview_arm {
    ARCH = inkview_arm
    DEFINES += INKVIEW INKVIEW_PRO
}

DESTDIR = obj_qt_$$ARCH
OBJECTS_DIR = $$DESTDIR/obj
MOC_DIR = $$DESTDIR/moc
RCC_DIR = $$DESTDIR/rcc
UI_DIR = $$DESTDIR/ui
