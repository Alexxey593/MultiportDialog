#include "timerthread.h"

TimerThread::TimerThread(QObject *parent) :
    QThread(parent), pause(1)
{
}

void TimerThread::stop()
{
    terminate = true;
}

void TimerThread::setPause(quint32 p)
{
    if(p < 1)
        pause = 1;
    else
        pause = p;
}

void TimerThread::run()
{
    terminate = false;
    while(!terminate) {
        if(!terminate)
            emit sigTick();
        msleep(pause);
    }
}

void TimerThread::setFrequency(double f)
{
    if(f <= 0 || f > 1000)
        pause = 1;
    else
        pause = 1000.0 / f;
}
