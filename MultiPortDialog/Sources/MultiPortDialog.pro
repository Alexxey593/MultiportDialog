#-------------------------------------------------
#
# Project created by QtCreator 2012-04-26T11:57:33
#
#-------------------------------------------------

QT       += core gui network widgets

!android: {
    QT += serialport

    SOURCES +=  DialogInfo.cpp \
                DialogPortSettings.cpp \
                PortsListThread.cpp

    HEADERS +=  DialogInfo.h \
                DialogPortSettings.h \
                PortsListThread.h

    FORMS +=    DialogInfo.ui \
                DialogPortSettings.ui
}

TARGET = MultiPortDialog
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    hexedit/xbytearray.cpp \
    hexedit/qhexedit_p.cpp \
    hexedit/qhexedit.cpp \
    hexedit/commands.cpp \
    sendfilethread.cpp \    
    console.cpp \    
    mylistwidget.cpp \
    dialogpreferences.cpp \
    dialoghighlighter.cpp \
    colorpicker.cpp \
    dialogsethighlight.cpp \
    hexvalidator.cpp \
    sendfilelinethread.cpp \
    dialogsshpref.cpp \
#    simpletextview/simpletextview.cpp \
#    simpletextview/simpletextviewer_p.cpp \
    indicator.cpp

HEADERS  += mainwindow.h \
    hexedit/xbytearray.h \
    hexedit/qhexedit_p.h \
    hexedit/qhexedit.h \
    hexedit/commands.h \
    sendfilethread.h \
    console.h \
    mylistwidget.h \
    CPDInterfaces.h \
    dialogpreferences.h \
    dialoghighlighter.h \
    colorpicker.h \
    dialogsethighlight.h \
    hexvalidator.h \
    sendfilelinethread.h \
    dialogsshpref.h \
#    simpletextview/simpletextview.h \
#    simpletextview/simpletextviewer_p.h \
    indicator.h

FORMS    += mainwindow.ui \    
    dialogpreferences.ui \
    dialoghighlighter.ui \
    dialogsethighlight.ui \
    dialogsshpref.ui

INCLUDEPATH += ./hexedit \
               ../../../Shared/Network \
               ./simpletextview

win32: {
    LIBS += Ws2_32.lib
    SOURCES += ../../../Shared/Network/pinger.cpp
    HEADERS  += ../../../Shared/Network/pinger.h
}

RESOURCES += \
    im.qrc

#RC_FILE = myapp.rc
RC_ICONS = port.ico
QMAKE_TARGET_DESCRIPTION = "COM, TCP, UDP, SSH ports terminal"
QMAKE_TARGET_COPYRIGHT = "Stolyarov A.V."

VERSION = 4.0.0

TRANSLATIONS    = translation/MultiPortDialog_ru.ts
CODECFORTR      = UTF-8
CODECFORSRC     = UTF-8

# SSH
DEFINES += USE_SSH
SOURCES += ssh/sftpchannel.cpp \
    ssh/sftpdefs.cpp \
    ssh/sftpfilesystemmodel.cpp \
    ssh/sftpincomingpacket.cpp \
    ssh/sftpoperation.cpp \
    ssh/sftpoutgoingpacket.cpp \
    ssh/sftppacket.cpp \
    ssh/sshcapabilities.cpp \
    ssh/sshchannel.cpp \
    ssh/sshchannelmanager.cpp \
    ssh/sshconnection.cpp \
    ssh/sshconnectionmanager.cpp \
    ssh/sshcryptofacility.cpp \
    ssh/sshdirecttcpiptunnel.cpp \
    ssh/sshforwardedtcpiptunnel.cpp \
    ssh/sshhostkeydatabase.cpp \
    ssh/sshincomingpacket.cpp \
    ssh/sshinit.cpp \
    ssh/sshkeycreationdialog.cpp \
    ssh/sshkeyexchange.cpp \
    ssh/sshkeygenerator.cpp \
    ssh/sshkeypasswordretriever.cpp \
    ssh/sshlogging.cpp \
    ssh/sshoutgoingpacket.cpp \
    ssh/sshpacket.cpp \
    ssh/sshpacketparser.cpp \
    ssh/sshremoteprocess.cpp \
    ssh/sshremoteprocessrunner.cpp \
    ssh/sshsendfacility.cpp \
    ssh/sshtcpipforwardserver.cpp \
    ssh/sshtcpiptunnel.cpp
HEADERS += ssh/sftpchannel.h \
    ssh/sftpchannel_p.h \
    ssh/sftpdefs.h \
    ssh/sftpfilesystemmodel.h \
    ssh/sftpincomingpacket_p.h \
    ssh/sftpoperation_p.h \
    ssh/sftpoutgoingpacket_p.h \
    ssh/sftppacket_p.h \
    ssh/ssh_global.h \
    ssh/sshbotanconversions_p.h \
    ssh/sshcapabilities_p.h \
    ssh/sshchannel_p.h \
    ssh/sshchannelmanager_p.h \
    ssh/sshconnection.h \
    ssh/sshconnection_p.h \
    ssh/sshconnectionmanager.h \
    ssh/sshcryptofacility_p.h \
    ssh/sshdirecttcpiptunnel.h \
    ssh/sshdirecttcpiptunnel_p.h \
    ssh/ssherrors.h \
    ssh/sshexception_p.h \
    ssh/sshforwardedtcpiptunnel.h \
    ssh/sshforwardedtcpiptunnel_p.h \
    ssh/sshhostkeydatabase.h \
    ssh/sshincomingpacket_p.h \
    ssh/sshinit_p.h \
    ssh/sshkeycreationdialog.h \
    ssh/sshkeyexchange_p.h \
    ssh/sshkeygenerator.h \
    ssh/sshkeypasswordretriever_p.h \
    ssh/sshlogging_p.h \
    ssh/sshoutgoingpacket_p.h \
    ssh/sshpacket_p.h \
    ssh/sshpacketparser_p.h \
    ssh/sshpseudoterminal.h \
    ssh/sshremoteprocess.h \
    ssh/sshremoteprocess_p.h \
    ssh/sshremoteprocessrunner.h \
    ssh/sshsendfacility_p.h \
    ssh/sshtcpipforwardserver.h \
    ssh/sshtcpipforwardserver_p.h \
    ssh/sshtcpiptunnel_p.h

FORMS    += ssh/sshkeycreationdialog.ui

include(./botan/botan.pri)
