#include "mainwindow.h"
#include "sendfilethread.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMutexLocker>

#ifndef Q_OS_ANDROID
#include "DialogInfo.h"
#include "DialogPortSettings.h"
#include "PortsListThread.h"
#include "readportthread.h"
#include <QtSerialPort/QSerialPortInfo>
#endif

// #include "timerthread.h"
#include <QIntValidator>
#include <QSettings>
#include <QTcpServer>
#include <QTcpSocket>
#include <QUdpSocket>

#include "CPDInterfaces.h"
#include <QListWidgetItem>

#include "dialogpreferences.h"
#include "dialogsethighlight.h"

#include "hexvalidator.h"
#include "sendfilelinethread.h"

#ifdef USE_SSH

#endif

// QSpinBox {border: 1px inset gray}\nQSpinBox::up-button {subcontrol-position: right; width: 20px; height: 20px;}\nQSpinBox::down-button {subcontrol-position: left; width: 20px; height: 20px;}

bool portNameLessThan(const QString& s1, const QString& s2)
{
    QString num1;
    QString num2;
    for (int i = s1.length() - 1; i >= 0; i--) {
        if (s1.at(i).isDigit())
            num1.prepend(s1.at(i));
        else
            break;
    }
    for (int i = s2.length() - 1; i >= 0; i--) {
        if (s2.at(i).isDigit())
            num2.prepend(s2.at(i));
        else
            break;
    }
    return num1.toInt() < num2.toInt();
} // portNameLessThan

MainWindow::MainWindow(QString language, QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , dialogPref(0)
    , lang(language)
    , longestHltr(0)
    , pluginIsOn(false)
{
    ui->setupUi(this);
    ui->statusBar->addPermanentWidget(new QFrame(ui->statusBar));
    lbByteTransmitted = new QLabel("Out: 0 B", ui->statusBar);
    ui->statusBar->addPermanentWidget(lbByteTransmitted);
    lbByteTransmitted->setMinimumWidth(80);
    lbCurrSpeedOut = new QLabel("Out: 0 B/s", ui->statusBar);
    ui->statusBar->addPermanentWidget(lbCurrSpeedOut);
    lbCurrSpeedOut->setMinimumWidth(80);
    lbByteReceived = new QLabel("In: 0 B", ui->statusBar);
    ui->statusBar->addPermanentWidget(lbByteReceived);
    lbByteReceived->setMinimumWidth(80);
    lbCurrSpeed = new QLabel("In: 0 B/s", ui->statusBar);
    ui->statusBar->addPermanentWidget(lbCurrSpeed);
    lbCurrSpeed->setMinimumWidth(80);
    ui->textBrowser->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
#ifndef USE_SSH
    ui->tabPorts->removeTab(4);
#endif

    yellowButtonStyle = "QPushButton {background-color: yellow; \n border-style: inset; \n border-width: 1px; \n  border-radius: 5px; \n border-color: black;}";

    intValidator = new QIntValidator(0, 4000000, this);
    connect(ui->comboSpeed, /*&QComboBox::currentIndexChanged, this, &MainWindow::checkCustomRatePolicy);*/ SIGNAL(currentIndexChanged(int)), this, SLOT(checkCustomRatePolicy(int)));
    connect(ui->comboSpeed, SIGNAL(activated(int)), this, SLOT(checkCustomRatePolicy(int)));
    //        connect(ui->comboSpeed, &QComboBox::activated, this, &MainWindow::checkCustomRatePolicy);
    connect(ui->comboCom, SIGNAL(currentIndexChanged(int)), this, SLOT(checkCustomComPolicy(int)));
    connect(ui->comboCom, SIGNAL(activated(int)), this, SLOT(checkCustomComPolicy(int)));
    connect(ui->listASCII, &MyListWidget::middleButtonClicked, this, &MainWindow::on_btSendASCIILine_clicked);
    connect(ui->listHex, &MyListWidget::middleButtonClicked, this, &MainWindow::on_btSendHexLine_clicked);
    connect(ui->cbConsoleByLine, &QCheckBox::toggled, ui->console, &Console::setSendByLine);

#ifndef Q_OS_ANDROID
    dialogInfo = new DialogInfo(this);
    dialogPortSettings = new DialogPortSettings(this);
    connect(dialogPortSettings, &DialogPortSettings::accepted, this, &MainWindow::portSettingsAccepted);
#endif

    dialogPref = new DialogPreferences(lang, this);
    connect(dialogPref, &DialogPreferences::accepted, this, &MainWindow::settingsAccepted);
    settingsAccepted();

    formHighlight = new DialogSetHighlight(this);
    connect(formHighlight, &DialogSetHighlight::finished, this, &MainWindow::highlighterAccepted);

    writeMutex = new QMutex;
    // Валидатор для байтовой строки
    QRegExp rxBin("^[0,1]{8,8}$");
    QValidator* valBin = new QRegExpValidator(rxBin, this);
    ui->leByte->setValidator(valBin);
    // Валидатор для Hex строки
    // QRegExp rxHex("[0-9a-fA-F]+");
    // QValidator *valHex = new QRegExpValidator(rxHex, this);
    // ui->leHex->setValidator(valHex);
    HexValidator* valHex = new HexValidator(this);
    ui->leHex->setValidator(valHex);

    ui->hexBrowser->setOverwriteMode(false);
    ui->hexBrowser->setHighlighting(false);
    ui->hexBrowser->setReadOnly(true);
    ui->hexBrowser->setSelectByOneByte(true);

    timerInfo = new QTimer(this);
    connect(timerInfo, &QTimer::timeout, this, &MainWindow::slotTimerInfo); //SIGNAL(timeout()), this, SLOT(slotTimerInfo()));
    timerInfo->start(200);

    codec = QTextCodec::codecForLocale();

    // Инициализация портов
    currPort.type = PortState::COM;
    currPort.state = PortState::closed;
#ifndef Q_OS_ANDROID
    com = new QSerialPort(this);
    connect(com, &QSerialPort::readyRead, this, &MainWindow::slotReadPort); //SIGNAL(readyRead()), this, SLOT(slotReadPort()));
    connect(com, &QSerialPort::bytesWritten, this, &MainWindow::writtenBytes); //SIGNAL(bytesWritten(qint64)), this, SLOT(writtenBytes(qint64)));
    connect(com, /*&QSerialPort::error, this, &MainWindow::slotPortError);*/ SIGNAL(error(QSerialPort::SerialPortError)), SLOT(slotPortError()));
#endif
    tcp = new QTcpSocket(this);
    connect(tcp, &QTcpSocket::readyRead, this, &MainWindow::slotReadPort); //SIGNAL(readyRead()), this, SLOT(slotReadPort()));
    connect(tcp, &QTcpSocket::bytesWritten, this, &MainWindow::writtenBytes); //SIGNAL(bytesWritten(qint64)), this, SLOT(writtenBytes(qint64)));
    connect(tcp, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slotPortError()));
    connect(tcp, &QTcpSocket::stateChanged, this, &MainWindow::socketState); //SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(socketState(QAbstractSocket::SocketState)));

    tcpServer = new QTcpServer(this);
    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(slotTcpClientConnected()));
    connect(tcpServer, SIGNAL(acceptError(QAbstractSocket::SocketError)), this, SLOT(slotPortError()));

    // TODO: UDP to another thread
    udp = new QUdpSocket(this);
    connect(udp, SIGNAL(readyRead()), this, SLOT(slotReadPort()));
    connect(udp, SIGNAL(bytesWritten(qint64)), this, SLOT(writtenBytes(qint64)));
    connect(udp, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slotPortError()));
    connect(udp, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(socketState(QAbstractSocket::SocketState)));
    udpSendAdress = QHostAddress::Broadcast;
    udpSendPort = 0;

#ifdef USE_SSH
    ssh = NULL;
    sshDialog = new DialogSSHPref(this);
    connect(ui->btSSHPref, &QPushButton::clicked, sshDialog, &DialogSSHPref::show);
    connect(sshDialog, &DialogSSHPref::accepted, this, &MainWindow::sshPrefAccepted);
    connect(ui->actionSSHPref, &QAction::triggered, sshDialog, &DialogSSHPref::show);
#endif

    sendFileThread = new SendFileThread(writeMutex);
    connect(sendFileThread, SIGNAL(sigInfo(QString)), this, SLOT(showInfo(QString)));
    connect(sendFileThread, SIGNAL(sigStarted(bool)), this, SLOT(slotFileThread(bool)));
    connect(sendFileThread, SIGNAL(sigPercent(int)), ui->progressBar, SLOT(setValue(int)));
    connect(sendFileThread, SIGNAL(sigSendData(QByteArray)), this, SLOT(sendSerialDataWithMutex(QByteArray)));

    fileLinesTransmitting = false;
    sendFileLinesThread = new SendFileLineThread(writeMutex);
    connect(sendFileLinesThread, SIGNAL(sigInfo(QString)), this, SLOT(showInfo(QString)));
    connect(sendFileLinesThread, SIGNAL(sigStarted(bool)), this, SLOT(slotFileLinesThread(bool)));
    connect(sendFileLinesThread, SIGNAL(sigPercent(int)), ui->progressBar, SLOT(setValue(int)));
    connect(sendFileLinesThread, SIGNAL(sigSendData(QByteArray)), this, SLOT(sendSerialDataWithMutex(QByteArray)));

    connect(ui->console, SIGNAL(getData(QByteArray)), this, SLOT(sendSerialDataWithMutex(QByteArray)));
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));

// Проверка портов
#ifndef Q_OS_ANDROID
    QStringList portsList;
    foreach (const QSerialPortInfo& info, QSerialPortInfo::availablePorts()) {
        portsList << info.portName();
    }
    portsList.sort();
    slotPortsChanged(portsList);
    ports = portsList;
    portsThread = new PortsListThread(this);
    connect(portsThread, SIGNAL(portList(QStringList)), SLOT(slotPortsChanged(QStringList)));
    portsThread->start();
#endif // ifndef Q_OS_ANDROID

    bytesReceived = bytesReceivedNow = 0;
    bytesWritten = bytesWrittenNow = 0;
    hexTransmitting = false;
    textTransmitting = false;
    fileTransmitting = false;

    QDir dir(qApp->applicationDirPath());
    if (!dir.exists("Records"))
        dir.mkdir("Records");
    if (!dir.exists("Plugins"))
        dir.mkdir("Plugins");
    isRecording = false;
    recFileName = qApp->applicationDirPath() + "/Records/rec.dat";

    // readMutex = new QMutex;
    this->startTimer(5);

    // ttSend = new TimerThread;
    // connect(ttSend, SIGNAL(sigTick()), SLOT(slotSendStrings()));
    timerSend.setTimerType(Qt::PreciseTimer);
    connect(&timerSend, SIGNAL(timeout()), SLOT(slotSendStrings()));
    currIndexOfArrList = -1;
    drawData = true;

    loadPref();
    if (sshPref.protocol == DialogSSHPref::RemoteShell)
        ui->lbSSHStatus->setText(tr("Shell: -"));
    else if (sshPref.protocol == DialogSSHPref::RemoteProcess)
        ui->lbSSHStatus->setText(tr("Process %1: -").arg(sshPref.processName));
    else if (sshPref.protocol == DialogSSHPref::Tunnel)
        ui->lbSSHStatus->setText(tr("Tunnel: -"));
    else if (sshPref.protocol == DialogSSHPref::SFTP)
        ui->lbSSHStatus->setText(tr("SFTP: unsupported"));

    loadPlugins();
    highlighterAccepted();

#ifndef Q_OS_WIN
    ui->rbSBCom->hide();
    ui->rbComboCom->setChecked(true);
    ui->rbComboCom->hide();
    ui->sbCom->hide();
    ui->btPing->hide();
    ui->lbPing->hide();
    ui->imSignal->hide();
#else
    connect(&pinger, SIGNAL(pingReceived(int)), SLOT(pingReceived(int)));
#endif
    //    ui->simpleTextBrowser->hide();

    prevPins = QSerialPort::NoSignal;
    ui->DTR->setText("DTR");
    ui->DCD->setText("DCD");
    ui->DSR->setText("DSR");
    ui->RNG->setText("RNG");
    ui->RTS->setText("RTS");
    ui->CTS->setText("CTS");
    ui->STD->setText("STD");
    ui->SRD->setText("SRD");
    //    QFont fnt = this->font();
    //    fnt.setPixelSize(10);
    //    ui->DTR->setFont(fnt);
    //    ui->DCD->setFont(fnt);
    //    ui->DSR->setFont(fnt);
    //    ui->RNG->setFont(fnt);
    //    ui->RTS->setFont(fnt);
    //    ui->CTS->setFont(fnt);
    //    ui->STD->setFont(fnt);
    //    ui->SRD->setFont(fnt);
    connect(ui->RTS, &Indicator::clicked, this, &MainWindow::switchRTS);
    timerComPins = new QTimer(this);
    connect(timerComPins, &QTimer::timeout, this, &MainWindow::checkComPins);
}

MainWindow::~MainWindow()
{
    delete ui;
}

QByteArray MainWindow::ReplaceBadSymbols(QByteArray BadString)
{
    QByteArray GoodString;
    foreach (quint8 c, BadString) {
        if ((isspace(c) || (c == '\0')) && (c != ' ') && (c != '\n') && (c != '\r'))
            GoodString.append('.');
        else
            GoodString.append(c);
    }
    return GoodString;
}

QByteArray MainWindow::ParseString(QString str)
{
    if (str.length() < 4)
        return str.toLocal8Bit();
    QRegExp re = QRegExp("\\\\x[0-9a-fA-F]{2,2}");
    QVector<QPair<int, QByteArray> > expPositions;
    while (str.contains(re)) {
        int pos = str.indexOf(re);
        if (pos >= 0) {
            QString expStr = str.mid(pos + 2, 2);
            QByteArray hex = expStr.toLocal8Bit();
            QByteArray ba = QByteArray::fromHex(hex);
            expPositions.append(qMakePair(pos, ba));
            str.remove(pos, 4);
        }
    }
    QByteArray newBA = str.toLocal8Bit();
    for (int i = 0; i < expPositions.size(); ++i) {
        newBA.insert(expPositions.at(i).first + i, expPositions.at(i).second);
    }
    return newBA;
} // ParseString

QString MainWindow::insertSpaces(QString str)
{
    for (int i = str.length() - 3; i > 0; i -= 3)
        str.insert(i, " ");
    return str;
}

void MainWindow::savePref()
{
    QSettings settings("AVStolyarov", "MultiPortDialog");
    settings.beginGroup("MainWindow");
    settings.setValue("MainWindowGeometry", saveGeometry());
    settings.setValue("splitterSizes", ui->splitterMain->saveState());
    settings.setValue("splitterVertSizes", ui->splitterVert->saveState());
    settings.setValue("tabPortsIndex", ui->tabPorts->currentIndex());
    settings.setValue("ToolBoxIndex", ui->toolBox->currentIndex());
    settings.setValue("ByteText", ui->leByte->displayText());
    settings.setValue("ASCIIText", ui->leText->text());
    settings.setValue("HexText", ui->leHex->text());
    settings.setValue("TextPause", ui->sbTextPause->value());
    settings.setValue("HexPause", ui->sbHexPause->value());
    settings.setValue("DrawHex", ui->cbDrawHex->isChecked());
    settings.setValue("DrawText", ui->cbDrawText->isChecked());
    settings.setValue("ConsoleByLine", ui->cbConsoleByLine->isChecked());
    settings.setValue("EnterValueIndex", ui->comboConsoleEnter->currentIndex());
    settings.setValue("FileLinePause", ui->sbFileLinePause->value());
    settings.setValue("FileLineEnd", ui->comboFileEOL->currentIndex());

    settings.beginGroup("ComPort");
    settings.setValue("PortIndex", ui->comboCom->currentIndex());
    settings.setValue("SpeedIndex", ui->comboSpeed->currentIndex());
    QString str = "-";
    if (ui->comboSpeed->currentIndex() == 15) {
        QLineEdit* line = ui->comboSpeed->lineEdit();
        if (line)
            str = line->text().remove(' ');
        if (str.isEmpty())
            str = "-";
    }
    settings.setValue("UserSpeed", str);
    settings.endGroup();

    settings.beginGroup("TCP");
    settings.setValue("TCP", ui->leIP->text());
    settings.setValue("TCPPort", ui->sbPort->value());
    settings.endGroup();

    settings.beginGroup("TCPServer");
    settings.setValue("IP", ui->leTCPServerIP->text());
    settings.setValue("Port", ui->sbTCPServerPort->value());
    settings.endGroup();

    settings.beginGroup("UDP");
    settings.setValue("UDPIP", ui->leUdpIP->text());
    settings.setValue("UDPPort", ui->sbUdpPort->value());
    settings.setValue("UDPBindIP", ui->leUdpIPBind->text());
    settings.setValue("UDPBindPort", ui->sbUdpPortBind->value());
    settings.setValue("BindUDP", ui->cbUseUdpBind->isChecked());
    settings.endGroup();

    settings.beginGroup("SSH");
    settings.setValue("IP", ui->leSSHIP->text());
    settings.setValue("Port", ui->sbSSHPort->value());
    settings.setValue("Protocol", static_cast<int>(sshPref.protocol));
    settings.setValue("LocalIP", sshPref.localIP);
    settings.setValue("LocalPort", sshPref.localPort);
    settings.setValue("ProcName", sshPref.processName);
    settings.setValue("AuthType", static_cast<int>(sshPref.params.authenticationType));
    settings.setValue("Username", sshPref.params.userName);
    settings.setValue("Password", sshPref.params.password);
    settings.setValue("PrivateKey", sshPref.params.privateKeyFile);
    settings.endGroup();

    settings.endGroup();

    QFile pFile(qApp->applicationDirPath() + "/pref.dat");
    if (!pFile.open(QIODevice::WriteOnly)) {
        showInfo(tr("Error saving settings", "Ошибка сохранения настроек."));
        return;
    }
    QDataStream PrefFile(&pFile);
    QList<QListWidgetItem> list;
    for (int i = 0; i < ui->listASCII->count(); i++)
        list.append(*ui->listASCII->item(i));
    PrefFile << list;
    list.clear();
    for (int i = 0; i < ui->listHex->count(); i++)
        list.append(*ui->listHex->item(i));
    PrefFile << list;
    pFile.close();
    showInfo(tr("Settings saved", "Настройки сохранены."));
} // savePref

void MainWindow::loadPref()
{
    QSettings settings("AVStolyarov", "MultiPortDialog");

    bool savePos = true;
    if (this->dialogPref)
        savePos = dialogPref->settings().savePos;

    settings.beginGroup("MainWindow");
    if (savePos) {
        QByteArray arr = settings.value("MainWindowGeometry").toByteArray();
        if (!arr.isEmpty())
            restoreGeometry(arr);
        ui->splitterMain->restoreState(settings.value("splitterSizes").toByteArray());
        ui->splitterVert->restoreState(settings.value("splitterVertSizes").toByteArray());
    }
    ui->tabPorts->setCurrentIndex(settings.value("tabPortsIndex", 0).toInt());
    ui->toolBox->setCurrentIndex(settings.value("ToolBoxIndex", 0).toInt());
    ui->leByte->setText(settings.value("ByteText", "00000000").toString());
    ui->leText->setText(settings.value("ASCIIText", "").toString());
    ui->leHex->setText(settings.value("HexText", "").toString());
    ui->sbTextPause->setValue(settings.value("TextPause", 1000).toInt());
    ui->sbHexPause->setValue(settings.value("HexPause", 1000).toInt());
    ui->cbDrawHex->setChecked(settings.value("DrawHex", true).toBool());
    ui->cbDrawText->setChecked(settings.value("DrawText", true).toBool());
    ui->cbConsoleByLine->setChecked(settings.value("ConsoleByLine", false).toBool());
    ui->comboConsoleEnter->setCurrentIndex(settings.value("EnterValueIndex", 1).toInt());
    ui->sbFileLinePause->setValue(settings.value("FileLinePause", 0).toInt());
    ui->comboFileEOL->setCurrentIndex(settings.value("FileLineEnd", 1).toInt());

    settings.beginGroup("ComPort");
    ui->comboCom->setCurrentIndex(settings.value("PortIndex", 0).toInt());
    ui->comboSpeed->setCurrentIndex(settings.value("SpeedIndex", 0).toInt());
    if (ui->comboSpeed->currentIndex() == 15) {
        checkCustomRatePolicy(15);
        ui->comboSpeed->setEditText(settings.value("UserSpeed", "-").toString());
    }
    settings.endGroup();

    settings.beginGroup("TCP");
    ui->leIP->setText(settings.value("TCP", "127.0.0.1").toString());
    ui->sbPort->setValue(settings.value("TCPPort", 9000).toInt());
    settings.endGroup();

    settings.beginGroup("TCPServer");
    ui->leTCPServerIP->setText(settings.value("IP", "").toString());
    ui->sbTCPServerPort->setValue(settings.value("Port", 9000).toInt());
    settings.endGroup();

    settings.beginGroup("UDP");
    ui->leUdpIP->setText(settings.value("UDPIP", "").toString());
    ui->sbUdpPort->setValue(settings.value("UDPPort", 9000).toInt());
    ui->leUdpIPBind->setText(settings.value("UDPBindIP", "").toString());
    ui->sbUdpPortBind->setValue(settings.value("UDPBindPort", 9000).toInt());
    ui->cbUseUdpBind->setChecked(settings.value("BindUDP", true).toBool());
    settings.endGroup();

    settings.beginGroup("SSH");
    ui->leSSHIP->setText(settings.value("IP", "127.0.0.1").toString());
    ui->sbSSHPort->setValue(settings.value("Port", 22).toInt());
    sshPref.protocol = static_cast<DialogSSHPref::ProtocolType>(settings.value("Protocol", 0).toInt());
    sshPref.localIP = settings.value("LocalIP", "127.0.0.1").toString();
    sshPref.localPort = settings.value("LocalPort", 22).toInt();
    sshPref.processName = settings.value("ProcName", "exec").toString();
    sshPref.params.authenticationType = static_cast<QSsh::SshConnectionParameters::AuthenticationType>(settings.value("AuthType", 0).toInt());
    sshPref.params.userName = settings.value("Username", "").toString();
    sshPref.params.password = settings.value("Password", "").toString();
    sshPref.params.privateKeyFile = settings.value("PrivateKey", "").toString();
    settings.endGroup();
    sshDialog->setSshPref(sshPref);

    settings.endGroup();

    QFile pFile(qApp->applicationDirPath() + "/pref.dat");
    if (!pFile.open(QIODevice::ReadOnly)) {
        showInfo(tr("pref.dat not found.", "Файл настроек не найден."));
        return;
    }
    QDataStream PrefFile(&pFile);
    QList<QListWidgetItem> list;
    PrefFile >> list;
    foreach (QListWidgetItem item, list) {
        QListWidgetItem* newItem = new QListWidgetItem();
        *newItem = item;
        newItem->setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsDragEnabled);
        ui->listASCII->addItem(newItem);
    }
    PrefFile >> list;
    foreach (QListWidgetItem item, list) {
        QListWidgetItem* newItem = new QListWidgetItem();
        *newItem = item;
        newItem->setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsDragEnabled);
        ui->listHex->addItem(newItem);
    }

    pFile.close();
    showInfo(tr("Settings loaded.", "Настройки загружены."));
} // loadPref

void MainWindow::startRec()
{
    QString fn = QDate::currentDate().toString("yyyy.MM.dd") + "_" + QTime::currentTime().toString("hh-mm-ss") + ".dat";
    recFileName = qApp->applicationDirPath() + "/Records/" + fn;
    recFile.setFileName(recFileName);
    if (!recFile.open(QIODevice::WriteOnly | QIODevice::Append)) {
        ui->lbRecFile->setText("-");
        return;
    }
    recStream.setDevice(&recFile);
    ui->lbRecFile->setText(tr("Recording", "Запись"));
    isRecording = true;
    ui->btRec->setIcon(QIcon(":/im/Stop.png"));
} // startRec

void MainWindow::stopRec()
{
    recFile.close();
    recStream.setDevice(0);
    ui->lbRecFile->setText("-");
    isRecording = false;
    ui->btRec->setIcon(QIcon(":/im/Record.png"));
}

#ifndef Q_OS_ANDROID
QString MainWindow::portErrorString(QSerialPort::SerialPortError err)
{
    switch (err) {
    case QSerialPort::NoError:
        return tr("No error occurred.", "Нет ошибок.");

    case QSerialPort::DeviceNotFoundError:
        return tr("Attempting to open an non-existing device.", "Нет такого устройства.");

    case QSerialPort::PermissionError:
        return tr("Attempting to open an already opened device by another process or a user not having enough permission and credentials to open.",
            "Отказано в доступе.");

    case QSerialPort::OpenError:
        return tr("Attempting to open an already opened device in this object.", "Устройство уже открыто.");

    case QSerialPort::NotOpenError:
        return tr("Operation is executed that can only be successfully performed if the device is open.", "Устройство не открыто.");

    //    case QSerialPort::ParityError:
    //        return tr("Parity error detected by the hardware while reading data.", "Ошибка четности.");

    //    case QSerialPort::FramingError:
    //        return tr("Framing error detected by the hardware while reading data.", "Ошибка кадра.");

    //    case QSerialPort::BreakConditionError:
    //        return tr("Break condition detected by the hardware on the input line.", "Ошибка состояния прерывания.");

    case QSerialPort::WriteError:
        return tr("An I/O error occurred while writing the data.", "Ошибка записи.");

    case QSerialPort::ReadError:
        return tr("An I/O error occurred while reading the data.", "Ошибка чтения.");

    case QSerialPort::ResourceError:
        return tr("An I/O error occurred when a resource becomes unavailable, e.g. when the device is unexpectedly removed from the system.",
            "Ошибка ресурсов.");

    case QSerialPort::UnsupportedOperationError:
        return tr("The requested device operation is not supported or prohibited by the running operating system.",
            "Операция не поддерживается портом.");

    case QSerialPort::TimeoutError:
        return tr("A timeout error occurred.", "Таймаут.");

    case QSerialPort::UnknownError:
        return tr("An unidentified error occurred.", "Неизвестная ошибка порта.");
    } // switch
    return tr("An unidentified error occurred.", "Неизвестная ошибка.");
} // portErrorString
#endif // ifndef Q_OS_ANDROID

void MainWindow::loadPlugins()
{
    QDir pluginsDir = QDir(qApp->applicationDirPath());
    pluginsDir.cd("Plugins");

    foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
        try {
            QString absName = pluginsDir.absoluteFilePath(fileName);
            QPluginLoader loader(absName);
            QObject* plugin = loader.instance();
            if (plugin)
                populateMenus(plugin);
        } catch (...) {
            continue;
        }
    }
    ui->menuPlugins->setEnabled(!ui->menuPlugins->actions().isEmpty());
} // loadPlugins

void MainWindow::populateMenus(QObject* plugin)
{
    DataProcessingInterface* iPlugin = qobject_cast<DataProcessingInterface*>(plugin);
    if (iPlugin) {
        dataProcInterfaces.append(iPlugin);
        if (plugin->metaObject()->indexOfSignal("sendData(QByteArray)") >= 0)
            connect(plugin, SIGNAL(sendData(QByteArray)), SLOT(sendData(QByteArray)));

        QTranslator* translator = iPlugin->pluginTranslator(lang);
        if (!translator->isEmpty())
            qApp->installTranslator(translator);

        foreach (QString procName, iPlugin->DataProcessors()) {
            QString friendlyName = iPlugin->friedlyName(procName).isEmpty() ? procName : iPlugin->friedlyName(procName);
            QAction* action = new QAction(iPlugin->icon(procName), friendlyName, plugin);
            action->setData(procName);
            connect(action, SIGNAL(triggered()), this, SLOT(startPlugin()));
            ui->menuPlugins->addAction(action);
        }
    }
} // populateMenus

void MainWindow::timerEvent(QTimerEvent*)
{
    if (arrBuf.isEmpty())
        return;

    foreach (DataProcessingInterface* iPlugin, dataProcInterfaces) {
        foreach (QString procName, iPlugin->DataProcessors())
            iPlugin->processData(procName, arrBuf);
    }

    if (ui->cbDrawText->isChecked()) {
        static int noLinebreak = 0;
        QString string = codec->toUnicode(ReplaceBadSymbols(arrBuf));
        if (!string.contains('\n') && !string.contains('\r'))
            noLinebreak += string.count();
        if (noLinebreak > 2000) {
            string += '\n';
            noLinebreak = 0;
        }
        ui->textBrowser->moveCursor(QTextCursor::End);
        ui->textBrowser->insertPlainText(string);
        ui->textBrowser->ensureCursorVisible();

        //        ui->simpleTextBrowser->appendData(arrBuf);
    }
    if (ui->cbDrawHex->isChecked()) {
        ui->hexBrowser->append(arrBuf);
        /* if (!highlighters.isEmpty() && ui->cbHighlight->isChecked()) { // Подсветка
            arrBuf.prepend(prevPacket);
            for (int i = 0; i < highlighters.size(); i++) {
                int pos = -1;
                do {
                    pos = arrBuf.indexOf(highlighters.at(i).first, pos + 1);
                    if (pos >= 0) {
                        int dataSize = ui->hexBrowser->data().size();
                        int globalPos = dataSize - arrBuf.size() + pos;
                        if (globalPos < 0)
                            globalPos = 0;
                        ui->hexBrowser->addColorizedArea(globalPos, globalPos + highlighters.at(i).first.size() - 1, highlighters.at(i).second);
                    }
                } while (pos >= 0);
            }
            prevPacket = arrBuf.right(longestHltr - 1);
        }*/
    }
    arrBuf.clear();
} // timerEvent

void MainWindow::closeEvent(QCloseEvent* e)
{
    savePref();
#ifdef Q_OS_WIN
    pinger.stop();
#endif
    foreach (DataProcessingInterface* iPlugin, dataProcInterfaces) {
        if (iPlugin) {
            foreach (QString procName, iPlugin->DataProcessors())
                iPlugin->stopProcessor(procName);
        }
    }
    dataProcInterfaces.clear();
    closePort();
    timerInfo->stop();
#ifndef Q_OS_ANDROID
    portsThread->stop();
    portsThread->wait(3000);
#endif
    if (isRecording)
        stopRec();
    sendFileThread->stop();
    sendFileThread->wait(3000);
    // ttSend->stop();
    // ttSend->wait();
    timerSend.stop();

    // delete readMutex;
    delete writeMutex;

    e->accept();
} // closeEvent

void MainWindow::slotReadPort()
{
    QByteArray s;
    if (currPort.type == PortState::COM) {
#ifndef Q_OS_ANDROID
        s = com->readAll();
#endif
    } else if (currPort.type == PortState::TCP)
        s = tcp->readAll();
    else if (currPort.type == PortState::TCPServer) {
        QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
        if (socket && socket->isValid())
            s = socket->readAll();
    } else if (currPort.type == PortState::UDP) {
        QByteArray datagram;
        while (udp->hasPendingDatagrams()) {
            datagram.resize(udp->pendingDatagramSize());
            qint64 readedSize = udp->readDatagram(datagram.data(), datagram.size() /*, &senderIp, &senderPort*/);
            if (readedSize == -1)
                continue;
            else if (readedSize != datagram.size())
                showInfo(tr("ReadedSize (%1) != DatagramSize (%2)").arg(readedSize).arg(datagram.size()));
            s.append(datagram);
        }
    }
#ifdef USE_SSH
    else if (currPort.type == PortState::SSH) {
        if (sshPref.protocol == DialogSSHPref::RemoteShell || sshPref.protocol == DialogSSHPref::RemoteProcess) {
            s = sshProcess->readAllStandardOutput();
        }
    }
#endif
    if (s.isEmpty())
        return;

    bytesReceived += s.size();
    bytesReceivedNow += s.size();
    if (isRecording)
        recStream.writeRawData(s.data(), s.length());
    if (drawData || pluginIsOn) {
        // readMutex->lock();
        arrBuf.append(s);
        // readMutex->unlock();
    }
    //    foreach (DataProcessingInterface* iPlugin, dataProcInterfaces) {
    //        foreach (QString procName, iPlugin->DataProcessors())
    //            iPlugin->processData(procName, s);
    //    }
} // slotReadPort

void MainWindow::slotPortError()
{
    if (currPort.type == PortState::COM) {
#ifndef Q_OS_ANDROID
        //        showInfo(CommPort->errorString());
        qDebug() << "ComError: " << com->errorString();
#endif
    } else if (currPort.type == PortState::TCP)
        showInfo(tcp->errorString());
    else if (currPort.type == PortState::TCPServer)
        showInfo(tcpServer->errorString());
    else if (currPort.type == PortState::UDP)
        showInfo(udp->errorString());
#ifdef USE_SSH
    else if (currPort.type == PortState::SSH) {
        QString errStr = ssh->errorString();
        if (ssh->state() == QSsh::SshConnection::Unconnected) {
            closePort();
        }
        showInfo(errStr);
    }
#endif
} // slotPortError

void MainWindow::slotPortsChanged(QStringList portsList)
{
#ifndef Q_OS_ANDROID
    qSort(portsList.begin(), portsList.end(), portNameLessThan);
    QString oldStr = ui->comboCom->currentText();
    bool isCustom = ui->comboCom->currentIndex() == (ui->comboCom->count() - 1);
    int ind = -1;
    int i = -1;
    ui->comboCom->clear();
    bool checkOpenedPort = com->isOpen() && currPort.type == PortState::COM;
    bool wasPort = false;
    if (!portsList.isEmpty()) {
        foreach (QString str, portsList) {
            ++i;
            if (str == oldStr)
                ind = i;
            ui->comboCom->addItem(str);
            if (checkOpenedPort)
                if (com->portName().endsWith(str))
                    wasPort = true;
        }
        if (!isCustom)
            ui->comboCom->setCurrentIndex(ind);
    }
    ui->comboCom->addItem(tr("Manual"));
    if (isCustom) {
        ui->comboCom->setCurrentIndex(ui->comboCom->count() - 1);
        auto le = ui->comboCom->lineEdit();
        le->setText(oldStr);
    }
    if (checkOpenedPort && !wasPort && !isCustom)
        closePort();
#else // ifndef Q_OS_ANDROID
    Q_UNUSED(portsList);
#endif // ifndef Q_OS_ANDROID
} // slotPortsChanged

void MainWindow::openPort()
{
    if (ui->tabPorts->currentIndex() == 0) { // COM-port
#ifndef Q_OS_ANDROID
        currPort.type = PortState::COM;
        if (ui->comboSpeed->currentText().isEmpty()) {
            showInfo(tr("Baudrate not setted", "Не выбрана скорость порта"));
            return;
        }
        QString PortName;
        if (ui->comboCom->currentText().isEmpty()) {
            showInfo(tr("Port not setted", "Не выбран порт"));
            return;
        }
#ifdef Q_OS_WIN
        PortName = QString("//./%1").arg(ui->comboCom->currentText());
        com->setPortName(PortName);
#else
        PortName = QString("%1").arg(ui->comboCom->currentText());
        bool found = false;
        foreach (const QSerialPortInfo& info, QSerialPortInfo::availablePorts()) {
            if (info.portName() == PortName) {
                CommPort->setPort(info);
                found = true;
                break;
            }
        }
        if (!found)
            CommPort->setPortName(PortName);
#endif // ifdef Q_OS_WIN
        // CommPort->setPortName(PortName);
        DialogPortSettings::Settings p = dialogPortSettings->settings();
        int speed = ui->comboSpeed->currentText().remove(' ').toInt();
        if (!com->setBaudRate(speed)) {
            showInfo(tr("Set baudrate error (%1)", "Ошибка установки скорости (%1)").arg(portErrorString(com->error())));
            com->close();
            return;
        }
        com->setDataBits(p.dataBits);
        com->setParity(p.parity);
        com->setStopBits(p.stopBits);
        com->setFlowControl(p.flowControl);
        com->setReadBufferSize(p.buffSize);
        if (!com->open(QIODevice::ReadWrite)) {
            showInfo(tr("Port opening error (%1)", "Ошибка открытия порта (%1)").arg(portErrorString(com->error())));
        } else {
            ui->tabPorts->setEnabled(false);
            ui->actionPortPref->setEnabled(false);
            ui->console->setEnabled(true);
            ui->btStart->setText(tr("Off", "Выкл."));
            showInfo(tr("Port opened", "Порт открыт"));
            ui->btStart->setIcon(QIcon(":/im/Exit.png"));
            currPort.state = PortState::opened;
            com->setRequestToSend(p.RTS);
            com->setDataTerminalReady(p.DTR);
            com->setBreakEnabled(p.breakEnabled);
            timerComPins->start(10);
        }
#endif // ifndef Q_OS_ANDROID
    } else if (ui->tabPorts->currentIndex() == 1) { // TCP
        currPort.type = PortState::TCP;
        if (tcp->state() == QAbstractSocket::ConnectedState)
            tcp->disconnectFromHost();
        tcp->connectToHost(ui->leIP->text(), ui->sbPort->value());
        tcp->waitForConnected(5000);
        if (tcp->state() != QAbstractSocket::ConnectedState) {
            ui->statusBar->showMessage(tr("Connection error", "Ошибка соединения"), 5000);
            tcp->abort();
        } else {
            ui->tabPorts->setEnabled(false);
            ui->actionPortPref->setEnabled(false);
            ui->console->setEnabled(true);
            ui->btStart->setText(tr("Off", "Выкл."));
            showInfo(tr("Port opened", "Порт открыт"));
            ui->btStart->setIcon(QIcon(":/im/Exit.png"));
            currPort.state = PortState::opened;
        }
    } else if (ui->tabPorts->currentIndex() == 2) { // TCPServer
        currPort.type = PortState::TCPServer;
        QHostAddress adr;
        if (ui->leTCPServerIP->text().isEmpty())
            adr = QHostAddress::Any;
        else
            adr = QHostAddress(ui->leTCPServerIP->text());
        if (!tcpServer->listen(adr, ui->sbTCPServerPort->value())) {
            // ui->ledConnect->setColor(Qt::gray);
            showInfo(tr("Bind error.", "Ошибка прослушки."));
        } else {
            ui->tabPorts->setEnabled(false);
            ui->actionPortPref->setEnabled(false);
            ui->console->setEnabled(true);
            ui->btStart->setText(tr("Off", "Выкл."));
            showInfo(tr("Port binded.", "Порт прослушивается."));
            ui->btStart->setIcon(QIcon(":/im/Exit.png"));
            currPort.state = PortState::opened;
        }
    } else if (ui->tabPorts->currentIndex() == 3) { // UDP
        currPort.type = PortState::UDP;
        udpSendPort = ui->sbUdpPort->value();
        if (ui->leUdpIP->text().isEmpty())
            udpSendAdress = QHostAddress::Broadcast;
        else {
            udpSendAdress = QHostAddress(ui->leUdpIP->text());
            if (udpSendAdress.isNull()) {
                showInfo(tr("Wrong Receiver IP.", "Неверный формат адреса отправки."));
                udpSendAdress = QHostAddress::Broadcast;
                return;
            }
        }
        if (ui->cbUseUdpBind->isChecked()) {
            QHostAddress adr;
            if (ui->leUdpIPBind->text().isEmpty())
                adr = QHostAddress::Any;
            else {
                adr = QHostAddress(ui->leUdpIPBind->text());
                if (adr.isNull()) {
                    showInfo(tr("Wrong server IP.", "Неверный формат адреса приема."));
                    return;
                }
            }
            if (!udp->bind(adr, ui->sbUdpPortBind->value())) {
                showInfo(tr("Binding error (%1).", "Ошибка прослушки (%1).").arg(udp->errorString()));
                udp->abort();
                return;
            } else {
#ifdef Q_OS_WIN
                int v = -1;
                if (::setsockopt(udp->socketDescriptor(), SOL_SOCKET, SO_RCVBUF, (char*)&v, sizeof(v)) == -1)
                    showInfo("setsockopt(udp) error");
#endif
            }
        }
        ui->tabPorts->setEnabled(false);
        ui->actionPortPref->setEnabled(false);
        ui->console->setEnabled(true);
        ui->btStart->setText(tr("Off", "Выкл."));
        showInfo(tr("Port opened", "Порт открыт"));
        ui->btStart->setIcon(QIcon(":/im/Exit.png"));
        currPort.state = PortState::opened;
    }
#ifdef USE_SSH
    else if (ui->tabPorts->currentIndex() == 4) { // SSH
        currPort.type = PortState::SSH;
        sshPref.params.host = ui->leSSHIP->text();
        sshPref.params.port = ui->sbSSHPort->value();
        ssh = QSsh::acquireConnection(sshPref.params);
        if (ssh) {
            connect(ssh, SIGNAL(error(QSsh::SshError)), this, SLOT(slotPortError()), Qt::UniqueConnection);
            connect(ssh, &QSsh::SshConnection::connected, this, &MainWindow::sshConnected);
            connect(ssh, &QSsh::SshConnection::dataAvailable, this, &MainWindow::slotReadPort);
            ui->tabPorts->setEnabled(false);
            ui->console->setEnabled(false);
            ui->btStart->setText(tr("Off", "Выкл."));
            ui->btStart->setIcon(QIcon(":/im/Exit.png"));
            ui->btStart->setEnabled(false);
            currPort.state = PortState::opening;
            ssh->connectToHost();
        } else {
            showInfo(tr("Error in creating SSH session"));
        }
    }
    ui->actionSSHPref->setEnabled(false);
#endif
} // openPort

void MainWindow::closePort()
{
    try {
        if (fileTransmitting) {
            sendFileThread->stop();
            sendFileThread->wait();
        }
        currIndexOfArrList = -1;
        if (currPort.type == PortState::COM) {
#ifndef Q_OS_ANDROID
            timerComPins->stop();
            ui->DTR->clearColor();
            ui->DCD->clearColor();
            ui->DSR->clearColor();
            ui->RNG->clearColor();
            ui->RTS->clearColor();
            ui->CTS->clearColor();
            ui->STD->clearColor();
            ui->SRD->clearColor();
            prevPins = QSerialPort::NoSignal;
            if (com->isOpen())
                com->close();
#endif
        } else if (currPort.type == PortState::TCP) {
            if (tcp->state() == QAbstractSocket::ConnectedState)
                tcp->disconnectFromHost();
            if (tcp->state() != QAbstractSocket::UnconnectedState)
                tcp->waitForDisconnected();
        } else if (currPort.type == PortState::TCPServer) {
            foreach (QTcpSocket* socket, tcpSocketsList) {
                if (socket && socket->isValid())
                    socket->disconnectFromHost();
            }
            tcpSocketsList.clear();
            ui->lbTCTCliensCount->setText(tr("<html><head/><body><p><span style= font-size:12pt; font-weight:600;>Clients number: 0</span></p></body></html>",
                "<html><head/><body><p><span style= font-size:12pt; font-weight:600;>Количество клиентов: 0</span></p></body></html>"));
            tcpServer->close();
        } else if (currPort.type == PortState::UDP) {
            if (udp->state() == QAbstractSocket::ConnectedState)
                udp->disconnectFromHost();
            if (udp->state() != QAbstractSocket::UnconnectedState)
                udp->waitForDisconnected();
        }
#ifdef USE_SSH
        else if (currPort.type == PortState::SSH) {
            if (ssh) {
                if (sshPref.protocol == DialogSSHPref::RemoteShell || sshPref.protocol == DialogSSHPref::RemoteProcess) {
                    if (!sshProcess.isNull()) {
                        if (sshProcess->isRunning()) {
                            sshProcess->kill();
                            sshProcess->close();
                        }
                        sshProcess.clear();
                        if (sshPref.protocol == DialogSSHPref::RemoteShell)
                            ui->lbSSHStatus->setText(tr("Shell: -"));
                        else if (sshPref.protocol == DialogSSHPref::RemoteProcess)
                            ui->lbSSHStatus->setText(tr("Process %1: -").arg(sshPref.processName));
                    }
                } else if (sshPref.protocol == DialogSSHPref::Tunnel) {
                    if (!sshTunnel.isNull()) {
                        if (sshTunnel->isOpen())
                            sshTunnel->close();
                    }
                    sshTunnel.clear();
                    ui->lbSSHStatus->setText(tr("Tunnel: -"));
                }
                ssh->disconnectFromHost();
                QSsh::releaseConnection(ssh);
                ssh = NULL;
            }
        }
#endif
        currPort.state = PortState::closed;
        ui->tabPorts->setEnabled(true);
        ui->actionPortPref->setEnabled(true);
        ui->btStart->setText(tr("On"));
        ui->btStart->setStyleSheet("");
        ui->btStart->setEnabled(true);
        // ui->ledConnect->setColor(Qt::gray);
        // ui->ledConnect->update();
        ui->btStart->setIcon(QIcon(":/im/Play.png"));
        ui->console->setEnabled(false);
        ui->actionSSHPref->setEnabled(true);
        showInfo(tr("Port closed", "Порт закрыт"));
    } catch (...) {
        return;
    }
} // closePort

void MainWindow::slotTimerInfo()
{
    lbByteReceived->setText("In: " + insertSpaces(tr("%1").arg(bytesReceived, 0, 'f', 0)) + tr(" B"));
    lbByteTransmitted->setText("Out: " + insertSpaces(tr("%1").arg(bytesWritten, 0, 'f', 0)) + tr(" B"));
    ui->lbBufSize->setText(tr("Buffer: ") + insertSpaces(tr("%1").arg((double)arrBuf.size(), 0, 'f', 0)) + tr(" B"));
    static int tick = 0;
    tick++;
    if (tick == 5) {
        tick = 0;
        lbCurrSpeed->setText("In: " + insertSpaces(tr("%1").arg(bytesReceivedNow)) + tr(" B/s"));
        bytesReceivedNow = 0;
        lbCurrSpeedOut->setText("Out: " + insertSpaces(tr("%1").arg(bytesWrittenNow)) + tr(" B/s"));
        bytesWrittenNow = 0;
    }
    bool plug = false;
    foreach (DataProcessingInterface* iPlugin, dataProcInterfaces) {
        if (iPlugin) {
            foreach (QString procName, iPlugin->DataProcessors())
                if (iPlugin->isStarted())
                    plug = true;
        }
    }
    pluginIsOn = plug;
} // slotTimerInfo

void MainWindow::showInfo(QString str)
{
    ui->statusBar->showMessage(str, 3000);
}

void MainWindow::slotFileThread(bool started)
{
    if (started) {
        ui->btSendFile->setIcon(QIcon(":/im/Stop.png"));
        fileTransmitting = true;
    } else {
        ui->btSendFile->setIcon(QIcon(":/im/send.png"));
        fileTransmitting = false;
    }
}

void MainWindow::slotFileLinesThread(bool started)
{
    if (started) {
        ui->btSendFileLines->setIcon(QIcon(":/im/Stop.png"));
        fileLinesTransmitting = true;
    } else {
        ui->btSendFileLines->setIcon(QIcon(":/im/send.png"));
        fileLinesTransmitting = false;
    }
}

bool MainWindow::sendSerialDataWithMutex(QByteArray data)
{
    if (currPort.state != PortState::opened)
        return false;

    qint64 res = -1;
    writeMutex->lock();
    if (currPort.type == PortState::COM) {
#ifndef Q_OS_ANDROID
        res = com->write(data);
#endif
    } else if (currPort.type == PortState::TCP)
        res = tcp->write(data);
    else if (currPort.type == PortState::TCPServer) {
        foreach (QTcpSocket* socket, tcpSocketsList) {
            if (socket && socket->isValid())
                res = socket->write(data);
        }
    } else if (currPort.type == PortState::UDP)
        res = udp->writeDatagram(data, udpSendAdress, udpSendPort);
#ifdef USE_SSH
    else if (currPort.type == PortState::SSH) {
        if (sshPref.protocol == DialogSSHPref::Tunnel) {
            res = sshTunnel->write(data);
        } else if (sshPref.protocol == DialogSSHPref::RemoteProcess || sshPref.protocol == DialogSSHPref::RemoteShell) {
            res = sshProcess->write(data);
        }
        bytesWritten += res;
        bytesWrittenNow += res;
    }

#endif
    writeMutex->unlock();
    return res >= 0;
} // sendSerialDataWithMutex

bool MainWindow::sendData(QByteArray data)
{
    if (currPort.state != PortState::opened)
        return false;

    qint64 res = -1;
    if (currPort.type == PortState::COM) {
#ifndef Q_OS_ANDROID
        res = com->write(data);
#endif
    } else if (currPort.type == PortState::TCP)
        res = tcp->write(data);
    else if (currPort.type == PortState::TCPServer) {
        foreach (QTcpSocket* socket, tcpSocketsList) {
            if (socket && socket->isValid())
                res = socket->write(data);
        }
    } else if (currPort.type == PortState::UDP)
        res = udp->writeDatagram(data, udpSendAdress, udpSendPort);
#ifdef USE_SSH
    else if (currPort.type == PortState::SSH) {
        if (sshPref.protocol == DialogSSHPref::Tunnel) {
            res = sshTunnel->write(data);
        } else if (sshPref.protocol == DialogSSHPref::RemoteProcess || sshPref.protocol == DialogSSHPref::RemoteShell) {
            res = sshProcess->write(data);
        }
        bytesWritten += res;
        bytesWrittenNow += res;
    }

#endif
    return res >= 0;
} // sendData

bool MainWindow::sendData(char cByte)
{
    if (currPort.state != PortState::opened)
        return false;

    bool res = false;
    if (currPort.type == PortState::COM) {
#ifndef Q_OS_ANDROID
        res = com->putChar(cByte);
#endif
    } else if (currPort.type == PortState::TCP)
        res = tcp->putChar(cByte);
    else if (currPort.type == PortState::TCPServer) {
        foreach (QTcpSocket* socket, tcpSocketsList) {
            if (socket && socket->isValid())
                res = socket->putChar(cByte);
        }
    } else if (currPort.type == PortState::UDP)
        res = udp->writeDatagram(&cByte, 1, udpSendAdress, udpSendPort);
#ifdef USE_SSH
    else if (currPort.type == PortState::SSH) {
        if (sshPref.protocol == DialogSSHPref::Tunnel) {
            res = sshTunnel->putChar(cByte);
        } else if (sshPref.protocol == DialogSSHPref::RemoteProcess || sshPref.protocol == DialogSSHPref::RemoteShell) {
            res = sshProcess->putChar(cByte);
        }
        bytesWritten += res;
        bytesWrittenNow += res;
    }
#endif
    return res;
} // sendData

void MainWindow::writtenBytes(qint64 size)
{
    bytesWritten += size;
    bytesWrittenNow += size;
}

void MainWindow::slotSendStrings()
{
    static int loopNum = 0;
    if ((currIndexOfArrList < 0) || (arrList.size() <= currIndexOfArrList)) {
        // ttSend->stop();
        timerSend.stop();
        currIndexOfArrList = -1;
        arrList.clear();
        showInfo(tr("Transmission stopped", "Передача остановлена"));
        if (textTransmitting) {
            ui->btSendText->setText(tr("Send ASCII", "Послать ASCII"));
            textTransmitting = false;
        }
        if (hexTransmitting) {
            ui->btSendHex->setText(tr("Send Hex", "Послать Hex"));
            hexTransmitting = false;
        }
        loopNum = 0;
        return;
    }
    sendSerialDataWithMutex(arrList.at(currIndexOfArrList));
    currIndexOfArrList++;
    showInfo(tr("Command %1 (%2) sended", "Отправлена команда %1 (%2)").arg(currIndexOfArrList).arg(loopNum + 1));
    if (arrList.size() <= currIndexOfArrList) {
        if ((textTransmitting && ui->btLoopASCII->isChecked()) || (hexTransmitting && ui->btLoopHex->isChecked())) {
            currIndexOfArrList = 0;
            loopNum++;
        } else {
            // ttSend->stop();
            timerSend.stop();
            currIndexOfArrList = -1;
            arrList.clear();
            showInfo(tr("Transmission finished", "Передача завершена"));
            if (textTransmitting) {
                ui->btSendText->setText(tr("Send ASCII", "Послать ASCII"));
                textTransmitting = false;
            }
            if (hexTransmitting) {
                ui->btSendHex->setText(tr("Send Hex", "Послать Hex"));
                hexTransmitting = false;
            }
            loopNum = 0;
            return;
        }
    }
} // slotSendStrings

void MainWindow::checkCustomRatePolicy(int idx)
{
    ui->comboSpeed->setEditable(idx == 15);
    if (idx == 15) {
        ui->comboSpeed->clearEditText();
        QLineEdit* edit = ui->comboSpeed->lineEdit();
        edit->setValidator(intValidator);
    }
}

void MainWindow::checkCustomComPolicy(int idx)
{
    ui->comboCom->setEditable(idx == (ui->comboCom->count() - 1));
    if (idx == (ui->comboCom->count() - 1)) {
        ui->comboCom->clearEditText();
        //        QLineEdit* edit = ui->comboCom->lineEdit();
        //        edit->setValidator(intValidator);
    }
}

void MainWindow::portSettingsAccepted()
{
}

void MainWindow::startPlugin()
{
    QAction* action = qobject_cast<QAction*>(sender());
    DataProcessingInterface* iPlugin = qobject_cast<DataProcessingInterface*>(action->parent());
    if (iPlugin) {
        iPlugin->startProcessor(action->data().toString());
        pluginIsOn = true;
    }
}

void MainWindow::settingsAccepted()
{
    ui->hexBrowser->setBytesPerLine(dialogPref->settings().bytesPerLine);
    codec = QTextCodec::codecForName(dialogPref->settings().codec);
    ui->hexBrowser->setCodecName(dialogPref->settings().codec);
    // this->ui->retranslateUi(this);
    setFontSize(dialogPref->settings().fontSize);
}

void MainWindow::socketState(QAbstractSocket::SocketState state)
{
    switch (state) {
    case QAbstractSocket::UnconnectedState:
        showInfo(tr("The socket is not connected.", "Порт отключен."));
        if ((currPort.type == PortState::TCP) && (currPort.state == PortState::opened))
            closePort();
        break;

    case QAbstractSocket::HostLookupState:
        showInfo(tr("The socket is performing a host name lookup.", "Поиск порта..."));
        break;

    case QAbstractSocket::ConnectingState:
        showInfo(tr("The socket has started establishing a connection.", "Соединение..."));
        break;

    case QAbstractSocket::ConnectedState:
        showInfo(tr("A connection is established.", "Порт открыт."));
        break;

    case QAbstractSocket::BoundState:
        showInfo(tr("The socket is bound to an address and port.", "Порт связан."));
        break;

    case QAbstractSocket::ClosingState:
        showInfo(tr("The socket is about to close.", "Закрытие порта..."));
        break;

    default:
        break;
    } // switch
} // socketState

void MainWindow::slotTcpClientConnected()
{
    QTcpSocket* socket = tcpServer->nextPendingConnection();
    if (tcpSocketsList.contains(socket))
        tcpSocketsList.removeAt(tcpSocketsList.indexOf(socket));
    tcpSocketsList.append(socket);
    connect(socket, SIGNAL(disconnected()), this, SLOT(slotTcpClientDisconnected()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(slotReadPort()));
    connect(socket, SIGNAL(bytesWritten(qint64)), SLOT(writtenBytes(qint64)));
    showInfo(tr("Clien %1 connected", "Подключен клиент %1").arg(socket->peerAddress().toString()));
    ui->lbTCTCliensCount->setText(tr("<html><head/><body><p><span style= font-size:12pt; font-weight:600;>Clients number: %1</span></p></body></html>",
        "<html><head/><body><p><span style= font-size:12pt; font-weight:600;>Количество клиентов: %1</span></p></body></html>")
                                      .arg(
                                          tcpSocketsList.count()));
} // slotTcpClientConnected

void MainWindow::slotTcpClientDisconnected()
{
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket)
        return;

    if (!tcpSocketsList.contains(socket)) {
        showInfo(tr("Client %1 not in list", "Клиента %1 нет в списке").arg(socket->peerAddress().toString()));
        return;
    }
    tcpSocketsList.removeAt(tcpSocketsList.indexOf(socket));
    showInfo(tr("Client %1 disconnected", "Клиент %1 отключен").arg(socket->peerAddress().toString()));
    ui->lbTCTCliensCount->setText(tr("<html><head/><body><p><span style= font-size:12pt; font-weight:600;>Clients number: %1</span></p></body></html>",
        "<html><head/><body><p><span style= font-size:12pt; font-weight:600;>Количество клиентов: %1</span></p></body></html>")
                                      .arg(
                                          tcpSocketsList.count()));
    socket->deleteLater();
} // slotTcpClientDisconnected

void MainWindow::highlighterAccepted()
{
    if (!ui->cbHighlight->isChecked())
        return;
    highlighters = formHighlight->getHighlighters();
    longestHltr = 0;
    ui->hexBrowser->resetColorizedAreas();
    for (int i = 0; i < highlighters.size(); i++) {
        int len = highlighters.at(i).first.length();
        if (len > longestHltr)
            longestHltr = len;
        ui->hexBrowser->addColorizedArea(highlighters.at(i).first, highlighters.at(i).second);
    }
}

void MainWindow::on_btStart_clicked()
{
    ui->btStart->setStyleSheet(yellowButtonStyle);
    ui->btStart->setEnabled(false);
    qApp->processEvents();
    if (currPort.state == PortState::opened)
        closePort();
    else
        openPort();
    if (currPort.state != PortState::opening) {
        ui->btStart->setStyleSheet("");
        ui->btStart->setEnabled(true);
    }
    qApp->processEvents();
} // on_btStart_clicked

void MainWindow::on_btSendByte_clicked()
{
    if (currPort.state != PortState::opened) {
        showInfo(tr("Port not opened", "Порт не открыт"));
        return;
    }
    QString BinStr = ui->leByte->displayText();
    if (BinStr.size() < 8) {
        for (int i = 0; i < 8 - BinStr.size(); i++)
            BinStr += "0";
    }
    if (BinStr.size() > 8)
        BinStr = BinStr.mid(0, 8);
    ui->leByte->setText(BinStr);
    char cByte = 0x00;
    for (int i = 0; i < 8; i++) {
        if (BinStr[i] == '1')
            cByte |= 1 << (7 - i);
    }
    if (!sendData(cByte))
        showInfo(tr("Sending error", "Ошибка отправки"));
} // on_btSendByte_clicked

void MainWindow::on_leByte_textChanged(const QString& arg1)
{
    Q_UNUSED(arg1);
    QString BinStr = ui->leByte->displayText();
    if (BinStr.size() < 8) {
        for (int i = 0; i < 8 - BinStr.size(); i++)
            BinStr += "0";
    }
    if (BinStr.size() > 8)
        BinStr = BinStr.mid(0, 8);
    char cByte = 0x00;
    for (int i = 0; i < 8; i++) {
        if (BinStr[i] == '1')
            cByte |= 1 << (7 - i);
    }
    QByteArray arr;
    arr.append(cByte);
    arr = arr.toHex().toUpper();
    arr.prepend("0x");
    ui->lbByte->setText(arr);
} // on_leByte_textChanged

void MainWindow::on_btClear_clicked()
{
    ui->hexBrowser->clear();
    //    ui->hexBrowser->resetColorizedAreas();
    ui->textBrowser->clear();
    ui->console->clear();
    bytesReceived = 0;
    bytesWritten = 0;
    // QMutexLocker ml(readMutex);
    arrBuf.clear();
}

void MainWindow::on_btText_clicked()
{
    if (currPort.state != PortState::opened) {
        showInfo(tr("Port not opened", "Порт не открыт"));
        return;
    }
    if (!sendData(ParseString(ui->leText->text())))
        showInfo(tr("Sending error", "Ошибка отправки"));
}

void MainWindow::on_btHex_clicked()
{
    if (currPort.state != PortState::opened) {
        showInfo(tr("Port not opened", "Порт не открыт"));
        return;
    }
    QString str = ui->leHex->text();
    str.remove(' ');
    if (str.length() % 2) {
        showInfo(tr("Not even digits count", "Нечетное количество символов"));
        return;
    }
    ui->leHex->setText(str);
    if (!sendData(QByteArray::fromHex(str.toLocal8Bit())))
        showInfo(tr("Sending error", "Ошибка отправки"));
} // on_btHex_clicked

void MainWindow::on_btSendText_clicked()
{
    if (currPort.state != PortState::opened) {
        showInfo(tr("Port not opened", "Порт не открыт"));
        return;
    }
    if (hexTransmitting) {
        showInfo(tr("Transmitting Hex is on", "Идет передача Hex"));
        return;
    }
    if (textTransmitting) {
        currIndexOfArrList = -1;
        // ttSend->stop();
        timerSend.stop();
        arrList.clear();
        ui->btSendText->setText(tr("Send ASCII", "Послать ASCII"));
        showInfo(tr("Transmission stopped", "Передача остановлена"));
        textTransmitting = false;
    } else {
        QList<QByteArray> lba;
        int count = ui->listASCII->count();
        for (int i = 0; i < count; i++) {
            if (ui->listASCII->item(i)->checkState() == Qt::Checked)
                lba.append(ParseString(ui->listASCII->item(i)->text()));
        }
        if (lba.isEmpty()) {
            showInfo(tr("Nothing to send", "Нечего отправлять."));
            return;
        }
        ui->btSendText->setText(tr("Stop"));
        arrList = lba;
        currIndexOfArrList = 0;
        textTransmitting = true;
        // ttSend->setPause(ui->sbTextPause->value());
        // ttSend->start();
        timerSend.start(ui->sbTextPause->value());
    }
} // on_btSendText_clicked

void MainWindow::on_btSendHex_clicked()
{
    if (currPort.state != PortState::opened) {
        showInfo(tr("Port not opened", "Порт не открыт"));
        return;
    }
    if (textTransmitting) {
        showInfo(tr("Transmitting ASCII is on", "Идет передача ASCII"));
        return;
    }
    if (hexTransmitting) {
        currIndexOfArrList = -1;
        // ttSend->stop();
        // ui->sbTextPause->value();
        timerSend.stop();
        arrList.clear();
        ui->btSendHex->setText(tr("Send Hex", "Послать Hex"));
        showInfo(tr("Transmission stopped", "Передача остановлена"));
        hexTransmitting = false;
    } else {
        QList<QByteArray> lba;
        int count = ui->listHex->count();
        for (int i = 0; i < count; i++) {
            if (ui->listHex->item(i)->checkState() == Qt::Checked)
                lba.append(QByteArray::fromHex(ui->listHex->item(i)->text().toUtf8()));
        }
        if (lba.isEmpty()) {
            showInfo(tr("Nothing to send", "Нечего отправлять."));
            return;
        }

        ui->btSendHex->setText(tr("Stop", "Стоп"));
        arrList = lba;
        currIndexOfArrList = 0;
        hexTransmitting = true;
        // ttSend->setPause(ui->sbHexPause->value());
        // ttSend->start();
        timerSend.start(ui->sbHexPause->value());
    }
} // on_btSendHex_clicked

void MainWindow::on_btOpenFile_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File", "Открыть файл"));
    ui->leFile->setText(fileName);
}

void MainWindow::on_btSendFile_clicked()
{
    if (fileLinesTransmitting) {
        sendFileLinesThread->stop();
        sendFileLinesThread->wait();
    }

    if (fileTransmitting) {
        sendFileThread->stop();
        sendFileThread->wait();
    } else {
        if (currPort.state != PortState::opened) {
            showInfo(tr("Port not opened", "Порт не открыт"));
            return;
        }
        QString filename = ui->leFile->text();
        if (filename.isEmpty()) {
            showInfo(tr("File not selected", "Не выбран файл"));
            return;
        }
        if (!QFile::exists(filename)) {
            showInfo(tr("File doesn't exists", "Файл не существует"));
            return;
        }
        ui->progressBar->setValue(0);
        if (currPort.type == PortState::COM) {
#ifndef Q_OS_ANDROID
            sendFileThread->setFileName(filename, com->baudRate());
#endif
        } else if (currPort.type == PortState::TCP)
            sendFileThread->setFileName(filename, 100000000);
        else if (currPort.type == PortState::TCPServer)
            sendFileThread->setFileName(filename, 100000000);
        else if (currPort.type == PortState::UDP)
            sendFileThread->setFileName(filename, 100000000);
        sendFileThread->start();
    }
} // on_btSendFile_clicked

void MainWindow::on_btRec_clicked()
{
    if (isRecording)
        stopRec();
    else
        startRec();
}

void MainWindow::on_btInfo_clicked()
{
#ifndef Q_OS_ANDROID
    QString PortName;
    if (ui->comboCom->currentText().isEmpty()) {
        showInfo(tr("Port not selected", "Не выбран порт"));
        return;
    }
    PortName = QString("%1").arg(ui->comboCom->currentText());
    bool wasPort = false;
    foreach (const QSerialPortInfo& info, QSerialPortInfo::availablePorts()) {
        if (info.portName() == PortName) {
            QStringList str;
            str << info.portName() << info.description() << info.manufacturer() << info.systemLocation() << QString::number(
                                                                                                                info.vendorIdentifier())
                << QString::number(info.productIdentifier());
            if (dialogInfo->setInfo(str))
                dialogInfo->show();
            else
                showInfo(tr("Writing info error", "Ошибка записи информации"));
            wasPort = true;
            break;
        }
    }
    if (!wasPort)
        showInfo(tr("Port not found", "Порт не обнаружен"));
#endif // ifndef Q_OS_ANDROID
} // on_btInfo_clicked

void MainWindow::on_btPref_clicked()
{
    ui->actionPortPref->trigger();
}

void MainWindow::on_btCheckHex_clicked()
{
    for (int i = 0; i < ui->listHex->count(); i++) {
        QString str = ui->listHex->item(i)->text();
        QByteArray arr = QByteArray::fromHex(str.toUtf8());
        ui->listHex->item(i)->setText(arr.toHex());
    }
}

void MainWindow::on_cbDrawHex_toggled(bool checked)
{
    Q_UNUSED(checked);
    if (ui->cbDrawHex->isChecked() || ui->cbDrawText->isChecked())
        drawData = true;
    else
        drawData = false;
}

void MainWindow::on_cbDrawText_toggled(bool checked)
{
    Q_UNUSED(checked);
    if (ui->cbDrawHex->isChecked() || ui->cbDrawText->isChecked())
        drawData = true;
    else
        drawData = false;
}

void MainWindow::on_btAddASCII_clicked()
{
    QListWidgetItem* item = new QListWidgetItem();
    item->setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsDragEnabled);
    item->setCheckState(Qt::Checked);
    ui->listASCII->insertItem(ui->listASCII->currentRow() + 1, item);
}

void MainWindow::on_btRemoveASCII_clicked()
{
    int row = ui->listASCII->currentRow();
    int count = ui->listASCII->count();
    if (count == 0)
        return;

    if (row < 0)
        ui->listASCII->takeItem(count - 1);
    else
        ui->listASCII->takeItem(row);
} // on_btRemoveASCII_clicked

void MainWindow::on_btSendASCIILine_clicked()
{
    if (currPort.state != PortState::opened) {
        showInfo(tr("Port not opened", "Порт не открыт"));
        return;
    }
    if (ui->listASCII->currentRow() < 0) {
        showInfo(tr("String not selected", "Не выбрана строка"));
        return;
    }
    if (!sendData(ParseString(ui->listASCII->currentItem()->text())))
        showInfo(tr("Sending error", "Ошибка отправки"));
} // on_btSendASCIILine_clicked

void MainWindow::on_btAddHex_clicked()
{
    QListWidgetItem* item = new QListWidgetItem();
    item->setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsDragEnabled);
    item->setCheckState(Qt::Checked);
    ui->listHex->insertItem(ui->listHex->currentRow() + 1, item);
}

void MainWindow::on_btRemoveHex_clicked()
{
    int row = ui->listHex->currentRow();
    int count = ui->listHex->count();
    if (count == 0)
        return;

    if (row < 0)
        ui->listHex->takeItem(count - 1);
    else
        ui->listHex->takeItem(row);
} // on_btRemoveHex_clicked

void MainWindow::on_listHex_itemChanged(QListWidgetItem* item)
{
    QString str = item->text();
    QByteArray arr = QByteArray::fromHex(str.toUtf8());
    item->setText(arr.toHex().toUpper());
}

void MainWindow::on_btSendHexLine_clicked()
{
    if (currPort.state != PortState::opened) {
        showInfo(tr("Port not opened", "Порт не открыт"));
        return;
    }
    if (ui->listHex->currentRow() < 0) {
        showInfo(tr("String not selected", "Не выбрана строка"));
        return;
    }
    if (!sendData(QByteArray::fromHex(ui->listHex->currentItem()->text().toLocal8Bit())))
        showInfo(tr("Sending error", "Ошибка отправки"));
} // on_btSendHexLine_clicked

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, tr("About MultiPortDialog"), tr("Author: Stolyarov Aleksey Vladimirovich\n") + tr("alexxey593@gmail.com\n\n") + tr("Version: 4.0.0"));
}

void MainWindow::on_actionPortPref_triggered()
{
#ifndef Q_OS_ANDROID
    dialogPortSettings->show();
#endif
}

void MainWindow::on_actionPref_triggered()
{
    dialogPref->show();
}

void MainWindow::on_btSetHighlight_clicked()
{
    formHighlight->show();
}

void MainWindow::pingReceived(int value)
{
    if (value < 0) {
        ui->imSignal->setPixmap(QPixmap(":/im/signal0.png"));
        ui->lbPing->setText("-");
    } else {
        ui->lbPing->setText(tr("%1ms").arg(value));
        if (value < 10)
            ui->imSignal->setPixmap(QPixmap(":/im/signal4.png"));
        else if (value < 20)
            ui->imSignal->setPixmap(QPixmap(":/im/signal3.png"));
        else if (value < 50)
            ui->imSignal->setPixmap(QPixmap(":/im/signal2.png"));
        else
            ui->imSignal->setPixmap(QPixmap(":/im/signal1.png"));
    }
} // pingReceived

void MainWindow::on_btPing_toggled(bool checked)
{
#ifdef Q_OS_WIN
    if (checked) {
        if (!ui->leIP->text().isEmpty())
            pinger.ping(ui->leIP->text());
    } else {
        pinger.stop();
        pingReceived(-1);
    }
#else
    Q_UNUSED(checked);
#endif
} // on_btPing_toggled

void MainWindow::on_leIP_textChanged(const QString& arg1)
{
    Q_UNUSED(arg1);
#ifdef Q_OS_WIN
    ui->btPing->setChecked(false);
#endif
}

void MainWindow::setFontSize(int fontSize)
{
    foreach (auto wdt, this->findChildren<QWidget*>()) {
        QFont font = wdt->font();
        font.setPointSize(fontSize);
        wdt->setFont(font);
    }
}

void MainWindow::on_comboConsoleEnter_currentTextChanged(const QString& arg1)
{
    if (arg1 == "-")
        ui->console->setEnterValue(QByteArray());
    else if (arg1 == "0D")
        ui->console->setEnterValue(QByteArray("\x0d", 1));
    else if (arg1 == "0A")
        ui->console->setEnterValue(QByteArray("\x0a", 1));
    else if (arg1 == "0D0A")
        ui->console->setEnterValue(QByteArray("\x0d\x0a", 2));
}

void MainWindow::on_leHex_textChanged(const QString& arg1)
{
    QPalette pal = ui->leHex->palette();
    QString str = arg1;
    str.remove(' ');
    if (str.length() % 2)
        pal.setColor(QPalette::Base, QColor(255, 220, 220));
    else
        pal.setColor(QPalette::Base, Qt::white);
    ui->leHex->setPalette(pal);
}

void MainWindow::on_btSendFileLines_clicked()
{
    if (fileTransmitting) {
        sendFileThread->stop();
        sendFileThread->wait();
    }

    if (fileLinesTransmitting) {
        sendFileLinesThread->stop();
        sendFileLinesThread->wait();
    } else {
        if (currPort.state != PortState::opened) {
            showInfo(tr("Port not opened", "Порт не открыт"));
            return;
        }
        QString filename = ui->leFile->text();
        if (filename.isEmpty()) {
            showInfo(tr("File not selected", "Не выбран файл"));
            return;
        }
        if (!QFile::exists(filename)) {
            showInfo(tr("File doesn't exists", "Файл не существует"));
            return;
        }
        ui->progressBar->setValue(0);
        if (currPort.type == PortState::COM) {
#ifndef Q_OS_ANDROID
            sendFileLinesThread->setFileName(filename, com->baudRate(), ui->sbFileLinePause->value(), ui->comboFileEOL->currentText());
#endif
        } else
            sendFileLinesThread->setFileName(filename, 100000000, ui->sbFileLinePause->value(), ui->comboFileEOL->currentText());
        sendFileLinesThread->start();
    }
}

void MainWindow::sshConnected()
{
    showInfo(tr("Port opened", "Порт открыт"));
    ui->tabPorts->setEnabled(false);
    ui->console->setEnabled(true);
    ui->btStart->setEnabled(true);
    ui->btStart->setStyleSheet("");
    currPort.state = PortState::opened;
    if (sshPref.protocol == DialogSSHPref::Tunnel) {
        sshTunnel = ssh->createDirectTunnel(sshPref.localIP, sshPref.localPort, ui->leSSHIP->text(), ui->sbSSHPort->value());
        if (sshTunnel.isNull())
            showInfo("Tunnel is NULL");
        else {
            connect(sshTunnel.data(), &QSsh::SshDirectTcpIpTunnel::initialized, this, &MainWindow::sshProcessStarted);
            connect(sshTunnel.data(), &QSsh::SshDirectTcpIpTunnel::aboutToClose, this, &MainWindow::sshTunnelClosed);
            connect(sshTunnel.data(), &QSsh::SshDirectTcpIpTunnel::error, this, &MainWindow::sshTunnelError);
            sshTunnel->initialize();
        }
    } else if (sshPref.protocol == DialogSSHPref::RemoteProcess) {
        sshProcess = ssh->createRemoteProcess(sshPref.processName.toLocal8Bit());
        if (sshProcess.isNull()) {
            showInfo("SSH Process is NULL");
        } else {
            connect(sshProcess.data(), &QSsh::SshRemoteProcess::readyReadStandardOutput, this, &MainWindow::slotReadPort);
            connect(sshProcess.data(), &QSsh::SshRemoteProcess::started, this, &MainWindow::sshProcessStarted);
            connect(sshProcess.data(), &QSsh::SshRemoteProcess::closed, this, &MainWindow::sshProcessClosed);
            connect(sshProcess.data(), &QSsh::SshRemoteProcess::readyReadStandardError, this, &MainWindow::sshProcessError);
            sshProcess->start();
        }
    } else if (sshPref.protocol == DialogSSHPref::RemoteShell) {
        sshProcess = ssh->createRemoteShell();
        if (sshProcess.isNull()) {
            showInfo("SSH Shell is NULL");
        } else {
            connect(sshProcess.data(), &QSsh::SshRemoteProcess::readyReadStandardOutput, this, &MainWindow::slotReadPort);
            connect(sshProcess.data(), &QSsh::SshRemoteProcess::started, this, &MainWindow::sshProcessStarted);
            connect(sshProcess.data(), &QSsh::SshRemoteProcess::closed, this, &MainWindow::sshProcessClosed);
            connect(sshProcess.data(), &QSsh::SshRemoteProcess::readyReadStandardError, this, &MainWindow::sshProcessError);
            sshProcess->start();
        }
    }
    qApp->processEvents();
}

void MainWindow::sshDataReceived(QString str)
{
    QByteArray data = str.toLocal8Bit();
    bytesReceived += data.size();
    bytesReceivedNow += data.size();
    if (isRecording)
        recStream.writeRawData(data.data(), data.length());
    if (drawData || pluginIsOn) {
        arrBuf.append(data);
    }
}

void MainWindow::sshDisconnected()
{
    if (currPort.type == PortState::SSH)
        closePort();
}

void MainWindow::sshProcessStarted()
{
    showInfo("SSH Process started");
    if (sshPref.protocol == DialogSSHPref::RemoteShell)
        ui->lbSSHStatus->setText(tr("Shell: started"));
    else if (sshPref.protocol == DialogSSHPref::RemoteProcess)
        ui->lbSSHStatus->setText(tr("Process %1: started").arg(sshPref.processName));
    else if (sshPref.protocol == DialogSSHPref::Tunnel)
        ui->lbSSHStatus->setText(tr("Tunnel: initialized"));
    else
        ui->lbSSHStatus->setText(tr("?: started"));
}

void MainWindow::sshPrefAccepted()
{
    sshPref = sshDialog->getSshPref();
    if (sshPref.protocol == DialogSSHPref::RemoteShell)
        ui->lbSSHStatus->setText(tr("Shell: -"));
    else if (sshPref.protocol == DialogSSHPref::RemoteProcess)
        ui->lbSSHStatus->setText(tr("Process %1: -").arg(sshPref.processName));
    else if (sshPref.protocol == DialogSSHPref::Tunnel)
        ui->lbSSHStatus->setText(tr("Tunnel: -"));
    else if (sshPref.protocol == DialogSSHPref::SFTP)
        ui->lbSSHStatus->setText(tr("SFTP: unsupported"));
    else
        ui->lbSSHStatus->setText(tr("?: -"));
}

void MainWindow::sshProcessClosed(int closeStatus)
{
    showInfo("SSH Process closed");
    QString reason = tr("closed");
    switch (closeStatus) {
    case 0:
        reason = tr("failed to start");
        break;
    case 1:
        reason = tr("crash exit");
        break;
    case 2:
        reason = tr("normal exit");
        break;
    default:
        break;
    }
    if (sshPref.protocol == DialogSSHPref::RemoteShell)
        ui->lbSSHStatus->setText(tr("Shell: %1").arg(reason));
    else if (sshPref.protocol == DialogSSHPref::RemoteProcess)
        ui->lbSSHStatus->setText(tr("Process %1: %2").arg(sshPref.processName).arg(reason));
    else
        ui->lbSSHStatus->setText(QString("?: %1").arg(reason));
}

void MainWindow::sshProcessError()
{
    QByteArray err = sshProcess->readAllStandardError();
    if (sshPref.protocol == DialogSSHPref::RemoteShell)
        ui->lbSSHStatus->setText(tr("Shell: %1").arg(QString(err)));
    else if (sshPref.protocol == DialogSSHPref::RemoteProcess)
        ui->lbSSHStatus->setText(tr("Process %1: %2").arg(sshPref.processName).arg(QString(err)));
    else
        ui->lbSSHStatus->setText(QString("?: %1").arg(QString(err)));
}

void MainWindow::sshTunnelClosed()
{
    ui->lbSSHStatus->setText(tr("Tunnel: closed"));
}

void MainWindow::sshTunnelError(const QString& err)
{
    showInfo(tr("Tunnel error: %1").arg(err));
}
void MainWindow::on_cbHighlight_toggled(bool checked)
{
    if (checked) {
        highlighterAccepted();
    } else {
        ui->hexBrowser->resetColorizedAreas();
    }
}

void MainWindow::checkComPins()
{
    if (currPort.type == PortState::COM) {
        if (currPort.state == PortState::opened) {
            QSerialPort::PinoutSignals pins = com->pinoutSignals();
            if (pins != prevPins) {
                prevPins = pins;
                if (pins & QSerialPort::DataTerminalReadySignal)
                    ui->DTR->setColor(Qt::green);
                else
                    ui->DTR->clearColor();
                if (pins & QSerialPort::DataCarrierDetectSignal)
                    ui->DCD->setColor(Qt::green);
                else
                    ui->DCD->clearColor();
                if (pins & QSerialPort::DataSetReadySignal)
                    ui->DSR->setColor(Qt::green);
                else
                    ui->DSR->clearColor();
                if (pins & QSerialPort::RingIndicatorSignal)
                    ui->RNG->setColor(Qt::green);
                else
                    ui->RNG->clearColor();
                if (pins & QSerialPort::RequestToSendSignal)
                    ui->RTS->setColor(Qt::green);
                else
                    ui->RTS->clearColor();
                if (pins & QSerialPort::ClearToSendSignal)
                    ui->CTS->setColor(Qt::green);
                else
                    ui->CTS->clearColor();
                if (pins & QSerialPort::SecondaryTransmittedDataSignal)
                    ui->STD->setColor(Qt::green);
                else
                    ui->STD->clearColor();
                if (pins & QSerialPort::SecondaryReceivedDataSignal)
                    ui->SRD->setColor(Qt::green);
                else
                    ui->SRD->clearColor();
            }
        }
    }
}

void MainWindow::switchRTS()
{
    if (currPort.type == PortState::COM) {
        if (currPort.state == PortState::opened) {
            com->setRequestToSend(!(prevPins & QSerialPort::RequestToSendSignal));
        }
    }
}
