#-------------------------------------------------
#
# Project created by QtCreator 2013-11-01T08:55:41
#
#-------------------------------------------------

QT       += core gui widgets printsupport

TARGET = DataCountGraphPlugin
TEMPLATE = lib
CONFIG += plugin

SOURCES += datacountvisualizer.cpp \
    memcustomplot.cpp \
    qcustomplot.cpp

HEADERS += datacountvisualizer.h \
    memcustomplot.h \
    qcustomplot.h
OTHER_FILES += DataCountGraphPlugin.json

INCLUDEPATH += ../../../MultiPortDialog/Sources

RESOURCES += \
    DataCountGraphPluginRes.qrc

TRANSLATIONS    = DataCountGraphPlugin_ru.ts
CODECFORTR      = UTF-8
CODECFORSRC     = UTF-8
