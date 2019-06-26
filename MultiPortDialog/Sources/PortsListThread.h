#ifndef PORTSLISTTHREAD_H
#define PORTSLISTTHREAD_H

#include <QThread>
#include <QStringList>

class PortsListThread : public QThread {
    Q_OBJECT
public:
    explicit PortsListThread(QObject *parent = 0);
    void stop();

private:
    bool terminate;
    QStringList ports;

protected:
    void run();

signals:
    void portList(QStringList);

public slots:

};

#endif // PORTSLISTTHREAD_H
