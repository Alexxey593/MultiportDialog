#include "formopu.h"
#include "ui_formopu.h"
#include <QFileDialog>
#include "filetuloader.h"
#include "speedtester.h"

FormOPU::FormOPU(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormOPU), started(false)
{
    ui->setupUi(this);
    opuControl = new PTCR96Controller(this);
    connect(opuControl, SIGNAL(dataToOPU(QByteArray)), SIGNAL(sendData(QByteArray)));
    connect(opuControl, SIGNAL(info(QString)), ui->plainTextEdit, SLOT(appendPlainText(QString)));
    connect(opuControl, SIGNAL(panChanged(double)), ui->lcdPanCurr, SLOT(display(double)));
    connect(opuControl, SIGNAL(tiltChanged(double)), ui->lcdTiltCurr, SLOT(display(double)));
// connect(opuControl, SIGNAL(tiltChanged(double)), ui->lbCurrTilt, SLOT(setNum(double)));
    connect(ui->sliderPan, SIGNAL(valueChanged(int)), opuControl, SLOT(setPanSpeed(int)));
    connect(ui->sliderTilt, SIGNAL(valueChanged(int)), opuControl, SLOT(setTiltSpeed(int)));
    connect(ui->btZero, SIGNAL(clicked()), opuControl, SLOT(moveToZero()));
    connect(ui->btHome, SIGNAL(clicked()), opuControl, SLOT(moveToHome()));
    connect(opuControl, SIGNAL(error(PTCR96Controller::ErrorsTypes)), SLOT(errorReceived(PTCR96Controller::ErrorsTypes)));

    tuLoader = new FileTULoader(this);
    connect(tuLoader, SIGNAL(info(QString)), ui->plainTextEdit, SLOT(appendPlainText(QString)));
    connect(tuLoader, SIGNAL(opuPos(double, double)), opuControl, SLOT(moveToCoords(double, double)));
    connect(tuLoader, SIGNAL(playedTime(QString)), ui->lbTime, SLOT(setText(QString)));

    speedTest = new SpeedTester(this);
    connect(opuControl, SIGNAL(panChanged(double)), speedTest, SLOT(panChanged(double)));
    connect(opuControl, SIGNAL(tiltChanged(double)), speedTest, SLOT(tiltChanged(double)));
    connect(opuControl, SIGNAL(busy(bool)), speedTest, SLOT(opuBusy(bool)));
    connect(speedTest, SIGNAL(gotoZero()), opuControl, SLOT(moveToZero()));
    connect(speedTest, SIGNAL(setPanSpeed(int)), opuControl, SLOT(setPanSpeed(int)));
    connect(speedTest, SIGNAL(setTiltSpeed(int)), opuControl, SLOT(setTiltSpeed(int)));
    connect(speedTest, SIGNAL(stop()), opuControl, SLOT(stopOpu()));
    connect(speedTest, SIGNAL(info(QString)), ui->plainTextEdit, SLOT(appendPlainText(QString)));
}

FormOPU::~FormOPU()
{
    delete ui;
}

void FormOPU::dataFromOPU(QByteArray data)
{
    opuControl->dataFromOPU(data);
}

void FormOPU::on_btStart_clicked()
{
    started = !started;
    if (started) {
        ui->btStart->setText(tr("STOP"));
        opuControl->start();
    } else {
        ui->btStart->setText(tr("START"));
        opuControl->stop();
    }
}

void FormOPU::closeEvent(QCloseEvent *e)
{
    if (started)
        on_btStart_clicked();
    emit closed();
    e->accept();
}

void FormOPU::on_sliderPan_sliderReleased()
{
    ui->sliderPan->setValue(0);
}

void FormOPU::on_sliderTilt_sliderReleased()
{
    ui->sliderTilt->setValue(0);
}

void FormOPU::on_btAbs_clicked()
{
    opuControl->moveToCoords(ui->sbPanAbs->value(), ui->sbTiltAbs->value());
}

void FormOPU::on_btDelta_clicked()
{
    opuControl->moveToDelta(ui->sbPanDelta->value(), ui->sbTiltDelta->value());
}

void FormOPU::on_btLoadTU_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this, "Open TU", "d:/My_Docs/Qt/ComPortDialog/Plugins/PTCR96Plugin/files/");
    if (!filename.isEmpty())
        tuLoader->openFile(filename);
}

void FormOPU::on_btPlayTU_clicked()
{
    tuLoader->play();
}

void FormOPU::on_btStop_clicked()
{
    tuLoader->stop();
}

void FormOPU::errorReceived(PTCR96Controller::ErrorsTypes err)
{
    if (err & PTCR96Controller::errPanTO)
        ui->lbPanTO->setText("Pan TO");
    else
        ui->lbPanTO->setText("");
    if (err & PTCR96Controller::errPanDE)
        ui->lbPanDE->setText("Pan DE");
    else
        ui->lbPanDE->setText("");
    if (err & PTCR96Controller::errTiltTO)
        ui->lbTiltTO->setText("Tilt TO");
    else
        ui->lbTiltTO->setText("");
    if (err & PTCR96Controller::errTiltDE)
        ui->lbTiltDE->setText("Tilt DE");
    else
        ui->lbTiltDE->setText("");
//    QTimer::singleShot(1000, this, SLOT(on_btReset_clicked()));
} // errorReceived

void FormOPU::on_btReset_clicked()
{
    opuControl->reset();
}

void FormOPU::on_cbSlowSpeed_toggled(bool checked)
{
    opuControl->setSlowSpeed(checked);
}

void FormOPU::on_btStartTest_clicked()
{
    speedTest->startTest(ui->sbTestSpeed->value());
}

void FormOPU::on_btSetPanFreq_clicked()
{
    opuControl->setMinMaxPanFreq(ui->sbPanMinFreq->value(), ui->sbTiltMaxFreq->value());
}

void FormOPU::on_btSetTiltFreq_clicked()
{
    opuControl->setMinMaxTiltFreq(ui->sbTiltMinFreq->value(), ui->sbTiltMaxFreq->value());
}
