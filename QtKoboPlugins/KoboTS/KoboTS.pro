QT += core gui
TARGET = KoboTS
TEMPLATE = lib
CONFIG += plugin

SOURCES += tsplugin.cpp kobots.cpp
HEADERS += tsplugin.h kobots.h SuspendManager.h
INCLUDEPATH += ../../SuspendManager
DEPENDPATH  += ../../SuspendManager
LIBS        += -L../../SuspendManager/obj_qt_kobo -lSuspendManager
