#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include <QFile>
#ifndef Q_OS_ANDROID
#include <QtSerialPort/QtSerialPort>
#endif
#include <QAbstractSocket>
#include <QHostAddress>

#ifdef Q_OS_WIN
#include "pinger.h"
#endif

#ifdef USE_SSH
#include "ssh/sshconnectionmanager.h"
#include "ssh/sshconnection.h"
#include "ssh/sshdirecttcpiptunnel.h"
#include "ssh/sftpchannel.h"
#include "ssh/sshremoteprocess.h"
#include "dialogsshpref.h"
#endif

namespace Ui {
class MainWindow;
}

struct PortState {
    enum PortType {
        COM,
        TCP,
        TCPServer,
        UDP,
        SSH
    };

    enum State {
        opened,
        opening,
        closed
    };

    PortType type;
    State state;
};

// TODO: create readThread and buffer

class QHexEdit;
class QLabel;
class QTimer;
class QMutex;
class SendFileThread;
class DialogInfo;
class DialogPortSettings;
class PortsListThread;
class QIntValidator;
class QTcpSocket;

class QListWidgetItem;
class DataProcessingInterface;
class DialogPreferences;
class DialogSetHighlight;

class QTcpServer;
class QUdpSocket;

class SendFileLineThread;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QString language = "en", QWidget* parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow* ui;
#ifndef Q_OS_ANDROID
    DialogInfo* dialogInfo;
    DialogPortSettings* dialogPortSettings;
    QSerialPort* com;
    PortsListThread* portsThread;
    QStringList ports;
#endif
    QHexEdit* hexBrowser;
    QLabel* lbPortInfo;
    QLabel* lbByteReceived;
    QLabel* lbCurrSpeed;
    QLabel* lbByteTransmitted;
    QLabel* lbCurrSpeedOut;
    QTimer* timerInfo;
    QTimer* timerComPins;
    QSerialPort::PinoutSignals prevPins;

    double bytesReceived, bytesReceivedNow;
    double bytesWritten, bytesWrittenNow;
    SendFileThread* sendFileThread;
    SendFileLineThread* sendFileLinesThread;
    bool hexTransmitting, textTransmitting, fileTransmitting, fileLinesTransmitting;
    QString recFileName;
    QFile recFile;
    QDataStream recStream;
    bool isRecording;
    //    TimerThread *ttSend;
    QTimer timerSend;
    QList<QByteArray> arrList;
    qint16 currIndexOfArrList;
    QIntValidator* intValidator;
    PortState currPort;
    QTcpSocket* tcp;
    QTcpServer* tcpServer;
    QList<QTcpSocket*> tcpSocketsList;
    QUdpSocket* udp;
    QHostAddress udpSendAdress;
    quint16 udpSendPort;

    QMutex* writeMutex;
    QByteArray arrBuf;
    // QByteArray chunk;

    QByteArray ReplaceBadSymbols(QByteArray BadString);
    QByteArray ParseString(QString str);
    QString insertSpaces(QString str); // Вставить пробелы в большие числа
    void savePref();
    void loadPref();
    void startRec();
    void stopRec();

#ifndef Q_OS_ANDROID
    QString portErrorString(QSerialPort::SerialPortError err);
#endif

    bool drawData;
    QTextCodec* codec;
    QString yellowButtonStyle;

    DialogPreferences* dialogPref;
    QString lang;
    DialogSetHighlight* formHighlight;
    QList<QPair<QByteArray, QColor> > highlighters;
    int longestHltr;
    QByteArray prevPacket;

#ifdef Q_OS_WIN
    Pinger pinger;
#endif

    bool pluginIsOn;
    QList<DataProcessingInterface*> dataProcInterfaces;
    void loadPlugins();
    void populateMenus(QObject* plugin);

#ifdef USE_SSH
    QSsh::SshConnection* ssh;
    QSharedPointer<QSsh::SshRemoteProcess> sshProcess;
    QSharedPointer<QSsh::SftpChannel> sshFtp;
    QSharedPointer<QSsh::SshDirectTcpIpTunnel> sshTunnel;
    DialogSSHPref* sshDialog;
    DialogSSHPref::SSHPref sshPref;
#endif

protected:
    void timerEvent(QTimerEvent*);
    void closeEvent(QCloseEvent* e);

private slots:
    void slotReadPort();
    void slotPortError();
    void slotPortsChanged(QStringList portsList);
    void openPort();
    void closePort();
    void slotTimerInfo();
    void showInfo(QString str);
    void slotFileThread(bool started);
    void slotFileLinesThread(bool started);
    bool sendSerialDataWithMutex(QByteArray data);
    bool sendData(QByteArray data);
    bool sendData(char cByte);
    void writtenBytes(qint64 size);
    void slotSendStrings();
    void checkCustomRatePolicy(int idx);
    void checkCustomComPolicy(int idx);
    void portSettingsAccepted();
    void startPlugin();
    void settingsAccepted();
    void socketState(QAbstractSocket::SocketState state);
    void slotTcpClientConnected();
    void slotTcpClientDisconnected();
    void highlighterAccepted();

    void on_btStart_clicked();
    void on_btSendByte_clicked();
    void on_leByte_textChanged(const QString& arg1);
    void on_btClear_clicked();
    void on_btText_clicked();
    void on_btHex_clicked();
    void on_btSendText_clicked();
    void on_btSendHex_clicked();
    void on_btOpenFile_clicked();
    void on_btSendFile_clicked();
    void on_btRec_clicked();
    void on_btInfo_clicked();
    void on_btPref_clicked();
    void on_btCheckHex_clicked();
    void on_cbDrawHex_toggled(bool checked);
    void on_cbDrawText_toggled(bool checked);
    void on_btAddASCII_clicked();
    void on_btRemoveASCII_clicked();
    void on_btSendASCIILine_clicked();
    void on_btAddHex_clicked();
    void on_btRemoveHex_clicked();
    void on_listHex_itemChanged(QListWidgetItem* item);
    void on_btSendHexLine_clicked();
    void on_actionAbout_triggered();
    void on_actionPortPref_triggered();
    void on_actionPref_triggered();
    void on_btSetHighlight_clicked();
    void pingReceived(int value);
    void on_btPing_toggled(bool checked);
    void on_leIP_textChanged(const QString& arg1);
    void setFontSize(int fontSize);
    void on_comboConsoleEnter_currentTextChanged(const QString& arg1);
    void on_leHex_textChanged(const QString& arg1);
    void on_btSendFileLines_clicked();
    void sshConnected();
    void sshDataReceived(QString str);
    void sshDisconnected();
    void sshProcessStarted();
    void sshPrefAccepted();
    void sshProcessClosed(int closeStatus);
    void sshProcessError();
    void sshTunnelClosed();
    void sshTunnelError(const QString& err);
    void on_cbHighlight_toggled(bool checked);
    void checkComPins();
    void switchRTS();
};
#endif // MAINWINDOW_H
