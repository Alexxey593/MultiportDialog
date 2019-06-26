#ifndef DIALOGPORTSETTINGS_H
#define DIALOGPORTSETTINGS_H

#include <QDialog>
#include <QtSerialPort/QSerialPort>

namespace Ui {
class DialogPortSettings;
}

class DialogPortSettings : public QDialog {
    Q_OBJECT

public:

    struct Settings {
        QSerialPort::DataBits dataBits;
        QString stringDataBits;
        QSerialPort::Parity parity;
        QString stringParity;
        QSerialPort::StopBits stopBits;
        QString stringStopBits;
        QSerialPort::FlowControl flowControl;
        QString stringFlowControl;
        qint64 buffSize;
        bool RTS;
        bool DTR;
        bool breakEnabled;
    };

    explicit DialogPortSettings(QWidget *parent = 0);
    ~DialogPortSettings();

    Settings settings() const;

private slots:
    void on_btOK_clicked();
    void on_btCancel_clicked();

private:
    Ui::DialogPortSettings *ui;
    Settings currentSettings;

private:
    void fillPortsParameters();
    void updateSettings();
    void restoreSettings();

};

#endif // DIALOGPORTSETTINGS_H
