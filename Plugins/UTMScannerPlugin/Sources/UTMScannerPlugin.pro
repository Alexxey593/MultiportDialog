#-------------------------------------------------
#
# Project created by QtCreator 2013-11-01T08:55:41
#
#-------------------------------------------------

QT       += core gui widgets

TARGET = UTMScannerPlugin
TEMPLATE = lib
CONFIG += plugin

SOURCES += \ 
    utmscanner.cpp \
    formscanner.cpp \
    utmscannercontroller.cpp \
    utmscannerwidget.cpp

HEADERS += \ 
    utmscanner.h \
    formscanner.h \
    utmscannercontroller.h \
    utmscannerwidget.h

OTHER_FILES += \
    UTMScannerPlugin.json

INCLUDEPATH += ../../../MultiPortDialog/Sources


TRANSLATIONS    = UTMScannerPlugin_ru.ts
CODECFORTR      = UTF-8
CODECFORSRC     = UTF-8

RESOURCES += \
    res.qrc

FORMS += \
    formscanner.ui
