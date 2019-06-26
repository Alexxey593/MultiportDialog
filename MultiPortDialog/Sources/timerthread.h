#ifndef TIMERTHREAD_H
#define TIMERTHREAD_H

#include <QThread>

class TimerThread : public QThread {
    Q_OBJECT
private:
    bool terminate;
    quint32 pause;

public:
    explicit TimerThread(QObject *parent = 0);

protected:
    void run();

signals:
    void sigTick();

public slots:
    void setFrequency(double f);
    void stop();
    void setPause(quint32 p);

};

#endif // TIMERTHREAD_H
