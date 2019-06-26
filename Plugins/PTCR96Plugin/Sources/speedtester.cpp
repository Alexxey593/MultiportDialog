#include "speedtester.h"
#include <QTimer>

SpeedTester::SpeedTester(QObject *parent) : QObject(parent), m_opuBusy(false), m_currSpeed(0), currPan(0.0), currTilt(0.0),
                                            state(stateNothing)
{}

SpeedTester::~SpeedTester()
{}

void SpeedTester::startTest(int speed)
{
// emit info(QString("Start test for byte %1").arg(m_currSpeed));
    m_currSpeed = speed;
// if ((currPan > 0.1) || (currPan < -0.1)) {
// state = statePrepareToStart;
// emit gotoZero();
// return;
// }
// if (speed > 255) {
// emit info("Достигнут предел скорости");
// state = stateNothing;
// return;
// }
// state = stateInProgress;
// emit setPanSpeed(m_currSpeed);
    if ((currTilt > 0.05) || (currTilt < -0.05)) {
        state = statePrepareToStart;
        emit gotoZero();
        return;
    }
    if (speed > 255) {
        emit info("Достигнут предел скорости");
        state = stateNothing;
        return;
    }
    state = stateInProgress;
    emit setTiltSpeed(m_currSpeed);
    timer.start();
} // startTest

void SpeedTester::panChanged(double pan)
{
    currPan = pan;
// if (state == stateInProgress) {
// if (pan >= 90) {
// qint64 ms = timer.elapsed();
//// emit stop();
// emit setPanSpeed(0);
// state = stateNothing;
// double secs = ms / 1000.0;
// double speed = pan / secs;
// speedList.append(qMakePair(m_currSpeed, speed));
// emit info(QString("%1\t%2").arg(m_currSpeed).arg(speed));
//// startTest(m_currSpeed + 1);
// m_currSpeed++;
// QTimer::singleShot(3000, this, SLOT(startTestDelayed()));
// }
// }
} // panChanged

void SpeedTester::tiltChanged(double tilt)
{
    currTilt = tilt;
    if (state == stateInProgress) {
        if (tilt >= 70) {
            qint64 ms = timer.elapsed();
// emit stop();
            emit setTiltSpeed(0);
            state = stateNothing;
            double secs = ms / 1000.0;
            double speed = tilt / secs;
            speedList.append(qMakePair(m_currSpeed, speed));
            emit info(QString("%1\t%2").arg(m_currSpeed).arg(speed));
// startTest(m_currSpeed + 1);
            m_currSpeed++;
            QTimer::singleShot(3000, this, SLOT(startTestDelayed()));
        }
    }
} // tiltChanged

void SpeedTester::opuBusy(bool busy)
{
    m_opuBusy = busy;
// emit info(QString("busy: %1, state: %2, currPan: %3").arg(busy).arg(state).arg(currPan));
    if (!busy && (state == statePrepareToStart) /* && ((currTilt > -0.05) && (currTilt < 0.05))*/)
        QTimer::singleShot(1000, this, SLOT(startTestDelayed()));
}

void SpeedTester::startTestDelayed()
{
    startTest(m_currSpeed);
}
