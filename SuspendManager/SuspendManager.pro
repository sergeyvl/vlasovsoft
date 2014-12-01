TEMPLATE  = lib
CONFIG   += lib
CONFIG += dll plugin debug
SOURCES += SuspendManager.cpp
HEADERS += SuspendManager.h

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
    DEFINES += X86 INKVIEW INKVIEW_PRO
}

DESTDIR = obj_qt_$$ARCH
OBJECTS_DIR = $$DESTDIR/obj
MOC_DIR = $$DESTDIR/moc
RCC_DIR = $$DESTDIR/rcc
UI_DIR = $$DESTDIR/ui
