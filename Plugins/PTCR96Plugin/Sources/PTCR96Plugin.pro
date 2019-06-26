#-------------------------------------------------
#
# Project created by QtCreator 2013-11-01T08:55:41
#
#-------------------------------------------------

QT       += core gui widgets

TARGET = PTCR96Plugin
TEMPLATE = lib
CONFIG += plugin

SOURCES += \ 
    PTCR96.cpp \
    ptcr96controller.cpp \
    formopu.cpp \
    filetuloader.cpp \
    speedtester.cpp

HEADERS += \ 
    PTCR96.h \
    ptcr96controller.h \
    formopu.h \
    filetuloader.h \
    speedtester.h

OTHER_FILES += \
    PTCR96Plugin.json

INCLUDEPATH += ../../../MultiPortDialog/Sources

TRANSLATIONS    = PTCR96Plugin_ru.ts
CODECFORTR      = UTF-8
CODECFORSRC     = UTF-8

RESOURCES += \
    res.qrc

FORMS += \
    formopu.ui
