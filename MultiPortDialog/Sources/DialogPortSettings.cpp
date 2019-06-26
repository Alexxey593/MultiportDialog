#include "DialogPortSettings.h"
#include "ui_DialogPortSettings.h"

DialogPortSettings::DialogPortSettings(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::DialogPortSettings)
{
    ui->setupUi(this);
    fillPortsParameters();

    updateSettings();
}

DialogPortSettings::~DialogPortSettings()
{
    delete ui;
}

DialogPortSettings::Settings DialogPortSettings::settings() const
{
    return currentSettings;
}

void DialogPortSettings::fillPortsParameters()
{
    // fill data bits
    ui->dataBitsBox->addItem(QLatin1String("5"), QSerialPort::Data5);
    ui->dataBitsBox->addItem(QLatin1String("6"), QSerialPort::Data6);
    ui->dataBitsBox->addItem(QLatin1String("7"), QSerialPort::Data7);
    ui->dataBitsBox->addItem(QLatin1String("8"), QSerialPort::Data8);
    ui->dataBitsBox->setCurrentIndex(3);

    // fill parity
    ui->parityBox->addItem(QLatin1String("None"), QSerialPort::NoParity);
    ui->parityBox->addItem(QLatin1String("Even"), QSerialPort::EvenParity);
    ui->parityBox->addItem(QLatin1String("Odd"), QSerialPort::OddParity);
    ui->parityBox->addItem(QLatin1String("Mark"), QSerialPort::MarkParity);
    ui->parityBox->addItem(QLatin1String("Space"), QSerialPort::SpaceParity);
    ui->parityBox->setCurrentIndex(0);

    // fill stop bits
    ui->stopBitsBox->addItem(QLatin1String("1"), QSerialPort::OneStop);
#ifdef Q_OS_WIN
    ui->stopBitsBox->addItem(QLatin1String("1.5"), QSerialPort::OneAndHalfStop);
#endif
    ui->stopBitsBox->addItem(QLatin1String("2"), QSerialPort::TwoStop);
    ui->parityBox->setCurrentIndex(0);

    // fill flow control
    ui->flowControlBox->addItem(QLatin1String("None"), QSerialPort::NoFlowControl);
    ui->flowControlBox->addItem(QLatin1String("RTS/CTS"), QSerialPort::HardwareControl);
    ui->flowControlBox->addItem(QLatin1String("XON/XOFF"), QSerialPort::SoftwareControl);
    ui->parityBox->setCurrentIndex(0);
    ui->sbBuffSize->setValue(65536);
    ui->cbDTR->setChecked(false);
    ui->cbRTS->setChecked(false);
    ui->cbBreak->setChecked(false);
}

void DialogPortSettings::updateSettings()
{
    // Data bits
    currentSettings.dataBits = static_cast<QSerialPort::DataBits>(
        ui->dataBitsBox->itemData(ui->dataBitsBox->currentIndex()).toInt());
    currentSettings.stringDataBits = ui->dataBitsBox->currentText();
    // Parity
    currentSettings.parity = static_cast<QSerialPort::Parity>(
        ui->parityBox->itemData(ui->parityBox->currentIndex()).toInt());
    currentSettings.stringParity = ui->parityBox->currentText();
    // Stop bits
    currentSettings.stopBits = static_cast<QSerialPort::StopBits>(
        ui->stopBitsBox->itemData(ui->stopBitsBox->currentIndex()).toInt());
    currentSettings.stringStopBits = ui->stopBitsBox->currentText();
    // Flow control
    currentSettings.flowControl = static_cast<QSerialPort::FlowControl>(
        ui->flowControlBox->itemData(ui->flowControlBox->currentIndex()).toInt());
    currentSettings.stringFlowControl = ui->flowControlBox->currentText();
    // BufferSize
    currentSettings.buffSize = ui->sbBuffSize->value();
    currentSettings.DTR = ui->cbDTR->isChecked();
    currentSettings.RTS = ui->cbRTS->isChecked();
    currentSettings.breakEnabled = ui->cbBreak->isChecked();
}

void DialogPortSettings::restoreSettings()
{
    ui->dataBitsBox->setCurrentIndex(ui->dataBitsBox->findText(currentSettings.stringDataBits));
    ui->parityBox->setCurrentIndex(ui->parityBox->findText(currentSettings.stringParity));
    ui->stopBitsBox->setCurrentIndex(ui->stopBitsBox->findText(currentSettings.stringStopBits));
    ui->flowControlBox->setCurrentIndex(ui->flowControlBox->findText(currentSettings.stringFlowControl));
    ui->sbBuffSize->setValue(currentSettings.buffSize);
    ui->cbDTR->setChecked(currentSettings.DTR);
    ui->cbRTS->setChecked(currentSettings.RTS);
    ui->cbBreak->setChecked(currentSettings.breakEnabled);
}

void DialogPortSettings::on_btOK_clicked()
{
    updateSettings();
    accept();
}

void DialogPortSettings::on_btCancel_clicked()
{
    restoreSettings();
    reject();
}
