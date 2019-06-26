#ifndef UTMSCANNERCONTROLLER_H
#define UTMSCANNERCONTROLLER_H

#include <QObject>

class UTMScannerController : public QObject
{
    Q_OBJECT
public:
    enum CommandType{
        ctUnknown,
        ctWrongFormat,
        ctInfoVersion,
        ctInfoParameters,
        ctInfoState,
        ctToMeasurementState,
        ctToStandbyState,
        ctReset,
        ctPartialReset,
        ctReboot,
        ctMeasureLong,
        ctMeasureShort,
        ctMeasureLongCont,
        ctMeasureShortCont
    };

    enum CEDType{
        ced2 = 2,
        ced3 = 3,
        ced4 = 4
    };

private:
    QByteArray buffer;
    void parseResponse(QByteArray &data);
    QList<qint32> decode6BitCode(QByteArray code, CEDType ced);

public:
    explicit UTMScannerController(QObject *parent = 0);
    void sendCommandToSensor(CommandType type, QByteArray parameters = QByteArray(), QByteArray userString = QByteArray());
    static QString commandTypeToString(CommandType type);
    static CommandType commandToCommandType(QByteArray commandCode);
    static QString statusToString(QByteArray status);    

signals:
    void dataToSensor(QByteArray data);
    void info(QString str);
    void scanLine(QList<qint32> listOfValues);

public slots:
    void dataFromSensor(QByteArray &data);

};

#endif // UTMSCANNERCONTROLLER_H
