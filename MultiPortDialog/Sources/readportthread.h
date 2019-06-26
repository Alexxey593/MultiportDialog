#ifndef READPORTTHREAD_H
#define READPORTTHREAD_H

#include <QThread>

#include <QtSerialPort/QSerialPort>

class QMutex;

class ReadPortThread : public QThread
{
    Q_OBJECT
public:
    explicit ReadPortThread(QSerialPort* port, QMutex* mutex, QObject *parent = 0);
    ~ReadPortThread();
    void stop();

private:
    QSerialPort* CommPort;
    QMutex* showDataMutex;
    bool terminate;
    QByteArray buff;
    QMutex* innerMutex;

    QByteArray ReplaceBadSymbols(QByteArray BadString);

private slots:
    void receiveData();

protected:
    void run();
    
signals:
    void sigReceivedData(QByteArray rawData, QByteArray goodString);
    void sigInfo(QString str);
    void sigBytesReceived(int);
    void sigBytesInBuff(int);
    
public slots:
    void clear();
    
};

#endif // READPORTTHREAD_H
