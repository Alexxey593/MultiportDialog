#ifndef PTCR96CONTROLLER_H
#define PTCR96CONTROLLER_H

#include <QObject>
#include <QTimer>

class PTCR96Controller : public QObject
{
    Q_OBJECT
public:
    enum ErrorsType {
        errNoError = 0x00,
        errPanTO = 0x01,
        errPanDE = 0x02,
        errTiltTO = 0x04,
        errTiltDE = 0x08
    };
    Q_DECLARE_FLAGS(ErrorsTypes, ErrorsType)

private:
    QByteArray buffer;
    QTimer timer;
    double m_pan; // Текущее положение по горизонатали
    double m_tilt; // Текущее положение по вертикали
    QByteArray statusCommand;
    QByteArray statusData;
    quint8 m_panSpeed;
    quint8 m_tiltSpeed;
    quint8 m_statCmd;
    bool m_isMovingPan;
    bool m_isMovingTilt;
    bool m_busy;
    int destPan, destTilt;
    ErrorsTypes m_err;
//    bool m_slowSpeed;
    bool m_slowPan;
    bool m_slowTilt;

    void parseResponse(QByteArray &data);
    char getCRC(QByteArray &arr);
    QByteArray hideControllChars(QByteArray &arr);
    QByteArray showControllChars(QByteArray &arr);

public:
    explicit PTCR96Controller(QObject *parent = 0);
    bool isMovingPan() const;
    bool isMovingTilt() const;

signals:
    void dataToOPU(QByteArray data);
    void info(QString str);
    void panChanged(double);
    void tiltChanged(double);
    void error(PTCR96Controller::ErrorsTypes err);
    void busy(bool);

public slots:
    void dataFromOPU(QByteArray &data);
    void getStatus();
    void start();
    void stop();
    void setPanSpeed(int speed);
    void setTiltSpeed(int speed);
    void setSlowSpeed(bool slow);
    void setSlowPan(bool slow);
    void setSlowTilt(bool slow);
    void moveToCoords(double _pan, double _tilt);
    void moveToDelta(double _pan, double _tilt);
    void moveToZero();
    void moveToHome();
    void reset();
    void stopOpu();
    void setMinMaxPanFreq(int _minFreq, int _maxFreq);
    void setMinMaxTiltFreq(int _minFreq, int _maxFreq);
};
Q_DECLARE_OPERATORS_FOR_FLAGS(PTCR96Controller::ErrorsTypes)

#endif // PTCR96CONTROLLER_H
