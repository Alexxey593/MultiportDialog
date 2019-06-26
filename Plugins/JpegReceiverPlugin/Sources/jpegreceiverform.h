#ifndef JPEGRECEIVERFORM_H
#define JPEGRECEIVERFORM_H

#include <QWidget>
#include "jpegreceiver.h"
#include <QTimer>

namespace Ui {
class JpegReceiverForm;
}

class JpegReceiverForm : public QWidget {
    Q_OBJECT

public:
    explicit JpegReceiverForm(QWidget *parent = 0);
    ~JpegReceiverForm();

public slots:
    void receiveData(QByteArray &data);
    void start();
    void stop();

private:
    Ui::JpegReceiverForm *ui;
    JpegReceiver jpegReceiver;
    QImage image;
    QTimer timerInfo;
    int fps;

private slots:
    void receiveImage(QImage im);
    void receiveBufSize(int bufSize);
    void on_sbBufSize_valueChanged(int arg1);
    void on_btClear_clicked();
    void onTimerInfo();

protected:
    void closeEvent(QCloseEvent *e);
    void changeEvent(QEvent *event);
    void resizeEvent(QResizeEvent *e);

signals:
    void closed();
    void languageChanged();
};
#endif // JPEGRECEIVERFORM_H
