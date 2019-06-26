#ifndef FORMSCANNER_H
#define FORMSCANNER_H

#include <QWidget>

class UTMScannerController;

namespace Ui {
class FormScanner;
}

class FormScanner : public QWidget
{
    Q_OBJECT

public:
    explicit FormScanner(QWidget *parent = 0);
    ~FormScanner();

private:
    Ui::FormScanner *ui;
    UTMScannerController *scanner;

public slots:
    void dataFromSensor(QByteArray &data);

protected:
    void closeEvent(QCloseEvent *e);
    void changeEvent(QEvent *event);

signals:
    void sendData(QByteArray data);
    void closed();
    void languageChanged();

private slots:
    void showScanData(QList<qint32> list);

    void on_btVersion_clicked();
    void on_btParameters_clicked();
    void on_btState_clicked();
    void on_btMeasurementState_clicked();
    void on_btStandbyState_clicked();
    void on_btReset_clicked();
    void on_btPartialReset_clicked();
    void on_btReboot_clicked();
    void on_btScan1SrameDist_clicked();
    void on_btScanContDist_clicked();
};

#endif // FORMSCANNER_H
