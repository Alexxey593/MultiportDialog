#ifndef SPEEDTESTER_H
#define SPEEDTESTER_H

#include <QObject>
#include <QElapsedTimer>
#include <QPair>

class SpeedTester : public QObject {
    Q_OBJECT

public:
    enum State {
        stateNothing,
        statePrepareToStart,
        stateInProgress
    };

public:
    explicit SpeedTester(QObject *parent = 0);
    ~SpeedTester();

private:
    bool m_opuBusy;
    int m_currSpeed;
    QList<QPair<int, double> > speedList;
    double currPan, currTilt;
    State state;
    QElapsedTimer timer;

signals:
    void stop();
    void gotoZero();
    void setPanSpeed(int speed);
    void setTiltSpeed(int speed);
    void info(QString str);

public slots:
    void startTest(int speed = 1);
    void panChanged(double pan);
    void tiltChanged(double tilt);
    void opuBusy(bool busy);

private slots:
    void startTestDelayed();
};
#endif // SPEEDTESTER_H
