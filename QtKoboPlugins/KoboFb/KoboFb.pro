QT += core gui
TARGET = KoboFb
TEMPLATE = lib
CONFIG += plugin

SOURCES += KoboFb.cpp screenplugin.cpp
HEADERS += KoboFb.h screenplugin.h ScreenManager.h ScreenManagerCallback.h
INCLUDEPATH += ../../ScreenManager
DEPENDPATH  += ../../ScreenManager
LIBS        += -L../../ScreenManager/obj_qt_kobo -lScreenManager
