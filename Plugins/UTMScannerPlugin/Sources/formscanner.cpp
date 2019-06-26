#include "formscanner.h"
#include "ui_formscanner.h"
#include "utmscannercontroller.h"

FormScanner::FormScanner(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormScanner)
{
    ui->setupUi(this);
    scanner = new UTMScannerController(this);
    connect(scanner, SIGNAL(dataToSensor(QByteArray)), SIGNAL(sendData(QByteArray)));
    connect(scanner, SIGNAL(info(QString)), ui->pteLog, SLOT(appendPlainText(QString)));
//    connect(scanner, SIGNAL(scanLine(QList<qint32>)), SLOT(showScanData(QList<qint32>)));
    connect(scanner, SIGNAL(scanLine(QList<qint32>)), ui->scannerView, SLOT(setDistances(QList<qint32>)));
}

FormScanner::~FormScanner()
{
    delete ui;
}

void FormScanner::dataFromSensor(QByteArray &data)
{
    scanner->dataFromSensor(data);
}

void FormScanner::closeEvent(QCloseEvent *e)
{
    emit closed();
    e->accept();
}

void FormScanner::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
        emit languageChanged();
    } else
        QWidget::changeEvent(event);
}

void FormScanner::showScanData(QList<qint32> list)
{
    ui->pteLog->appendPlainText(tr("Scan data:"));
    ui->pteLog->appendPlainText(QString::number(list.size()));
//    int i = 1;
//    foreach (qint32 num, list) {
//        ui->pteLog->appendPlainText(QString::number(i++) + " : " + QString::number(num));
//    }
}

void FormScanner::on_btVersion_clicked()
{
    scanner->sendCommandToSensor(UTMScannerController::ctInfoVersion);
}

void FormScanner::on_btParameters_clicked()
{
    scanner->sendCommandToSensor(UTMScannerController::ctInfoParameters);
}

void FormScanner::on_btState_clicked()
{
    scanner->sendCommandToSensor(UTMScannerController::ctInfoState);
}

void FormScanner::on_btMeasurementState_clicked()
{
    scanner->sendCommandToSensor(UTMScannerController::ctToMeasurementState);
}

void FormScanner::on_btStandbyState_clicked()
{
    scanner->sendCommandToSensor(UTMScannerController::ctToStandbyState);
}

void FormScanner::on_btReset_clicked()
{
    scanner->sendCommandToSensor(UTMScannerController::ctReset);
}

void FormScanner::on_btPartialReset_clicked()
{
    scanner->sendCommandToSensor(UTMScannerController::ctPartialReset);
}

void FormScanner::on_btReboot_clicked()
{
    scanner->sendCommandToSensor(UTMScannerController::ctReboot);
}

void FormScanner::on_btScan1SrameDist_clicked()
{
    scanner->sendCommandToSensor(UTMScannerController::ctMeasureLong, QByteArray("0000108000", 10));
}

void FormScanner::on_btScanContDist_clicked()
{
    scanner->sendCommandToSensor(UTMScannerController::ctMeasureLongCont, QByteArray("0000108000000", 13));
}
