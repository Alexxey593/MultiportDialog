#-------------------------------------------------
#
# Project created by QtCreator 2015-08-19T08:59:40
#
#-------------------------------------------------

QT       += widgets

TARGET = JpegReceiverPlugin
TEMPLATE = lib
CONFIG += plugin

SOURCES += jpegreceiverplugin.cpp \
    jpegreceiverform.cpp \
    jpegreceiver.cpp

HEADERS += jpegreceiverplugin.h \
    jpegreceiverform.h \
    jpegreceiver.h

OTHER_FILES += \
    JpegReceiverPlugin.json

INCLUDEPATH += ../../../MultiPortDialog/Sources

TRANSLATIONS    = JpegReceiverPlugin_ru.ts
CODECFORTR      = UTF-8
CODECFORSRC     = UTF-8

FORMS += \
    jpegreceiverform.ui

RESOURCES += \
    res.qrc
