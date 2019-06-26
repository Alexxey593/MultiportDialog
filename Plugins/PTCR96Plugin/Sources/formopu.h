#ifndef FORMOPU_H
#define FORMOPU_H

#include <QWidget>
#include <QCloseEvent>
#include "ptcr96controller.h"

namespace Ui {
class FormOPU;
}

class FileTULoader;
class SpeedTester;

class FormOPU : public QWidget
{
    Q_OBJECT

public:
    explicit FormOPU(QWidget *parent = 0);
    ~FormOPU();

private:
    Ui::FormOPU *ui;
    PTCR96Controller *opuControl;
    bool started;
    FileTULoader *tuLoader;
    SpeedTester *speedTest;

signals:
    void sendData(QByteArray);
    void closed();

public slots:
    void dataFromOPU(QByteArray data);

private slots:
    void on_btStart_clicked();
    void on_sliderPan_sliderReleased();
    void on_sliderTilt_sliderReleased();
    void on_btAbs_clicked();
    void on_btDelta_clicked();
    void on_btLoadTU_clicked();
    void on_btPlayTU_clicked();
    void on_btStop_clicked();
    void errorReceived(PTCR96Controller::ErrorsTypes err);

    void on_btReset_clicked();

    void on_cbSlowSpeed_toggled(bool checked);

    void on_btStartTest_clicked();

    void on_btSetPanFreq_clicked();

    void on_btSetTiltFreq_clicked();

protected:
    void closeEvent(QCloseEvent *e);
};

#endif // FORMOPU_H
