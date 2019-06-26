#include "jpegreceiverform.h"
#include "ui_jpegreceiverform.h"
#include <QCloseEvent>

JpegReceiverForm::JpegReceiverForm(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::JpegReceiverForm)
    , fps(0)
{
    ui->setupUi(this);
    connect(&jpegReceiver, &JpegReceiver::imageReceived, this, &JpegReceiverForm::receiveImage);
    connect(&jpegReceiver, &JpegReceiver::bufferSize, this, &JpegReceiverForm::receiveBufSize);
    connect(&jpegReceiver, &JpegReceiver::message, ui->pteLog, &QPlainTextEdit::appendPlainText);
    connect(&timerInfo, &QTimer::timeout, this, &JpegReceiverForm::onTimerInfo);
}

JpegReceiverForm::~JpegReceiverForm()
{
    delete ui;
}

void JpegReceiverForm::receiveData(QByteArray& data)
{
    jpegReceiver.receiveData(data);
}

void JpegReceiverForm::start()
{
    jpegReceiver.start();
    timerInfo.start(1000);
}

void JpegReceiverForm::stop()
{
    jpegReceiver.stop();
    timerInfo.stop();
    fps = 0;
    ui->lbFPS->setText(QString("FPS: %1").arg(fps));
}

void JpegReceiverForm::receiveImage(QImage im)
{
    fps++;
    image = im;
    if ((im.width() > ui->image->width()) || (im.height() > ui->image->height()))
        ui->image->setScaledContents(true);
    else
        ui->image->setScaledContents(false);
    ui->image->setPixmap(QPixmap::fromImage(im));
    ui->image->setMinimumSize(QSize(50, 50));
    ui->lbResol->setText(QString("%1x%2").arg(im.width()).arg(im.height()));
}

void JpegReceiverForm::receiveBufSize(int bufSize)
{
    ui->pbBuf->setValue(bufSize);
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents | QEventLoop::ExcludeSocketNotifiers, 50);
}

void JpegReceiverForm::closeEvent(QCloseEvent* e)
{
    emit closed();
    e->accept();
}

void JpegReceiverForm::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
        emit languageChanged();
    }
    else
        QWidget::changeEvent(event);
}

void JpegReceiverForm::resizeEvent(QResizeEvent* e)
{
    if (!image.isNull()) {
        if ((image.width() > ui->image->width()) || (image.height() > ui->image->height()))
            ui->image->setScaledContents(true);
        else
            ui->image->setScaledContents(false);
    }
    QWidget::resizeEvent(e);
}

void JpegReceiverForm::on_sbBufSize_valueChanged(int arg1)
{
    jpegReceiver.setMaxBufSize(arg1);
    ui->pbBuf->setMaximum(arg1);
}

void JpegReceiverForm::on_btClear_clicked()
{
    ui->pteLog->clear();
    ui->image->clear();
}

void JpegReceiverForm::onTimerInfo()
{
    ui->lbFPS->setText(QString("FPS: %1").arg(fps));
    fps = 0;
}
