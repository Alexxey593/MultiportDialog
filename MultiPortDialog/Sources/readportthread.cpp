#include "readportthread.h"
#include <QMutexLocker>
//#include <QtAddOnSerialPort/serialport.h>

ReadPortThread::ReadPortThread(QSerialPort *port, QMutex *mutex, QObject *parent) :
    QThread(parent)
{
    CommPort = port;
    showDataMutex = mutex;
    terminate = false;
    innerMutex = new QMutex;
    connect(CommPort, SIGNAL(readyRead()), this, SLOT(receiveData()));
}

ReadPortThread::~ReadPortThread()
{
    delete innerMutex;
}

void ReadPortThread::stop()
{
    terminate = true;
}

QByteArray ReadPortThread::ReplaceBadSymbols(QByteArray BadString)
{
    QByteArray GoodString;
    foreach(quint8 c, BadString)
    {
        if((isspace(c) || c == '\0') && c != ' ' && c != '\n' && c != '\r')
            GoodString.append('.');
        else
            GoodString.append(c);
    }
    return GoodString;
}

void ReadPortThread::receiveData()
{
    QByteArray s = CommPort->readAll();
    if(s.isEmpty())
        return;
    emit sigBytesReceived(s.size());
    innerMutex->lock();
    buff.append(s);
    innerMutex->unlock();
}

void ReadPortThread::run()
{
    terminate = false;
    int idle = 0;
    while(!terminate)
    {
        if(buff.isEmpty())
        {
            msleep(1);
            continue;
        }
        if(idle++ > 10)
        {
            idle = 0;
            msleep(10);
        }
        QByteArray chunk;
        if(buff.size() > 128)
        {
            innerMutex->lock();
            chunk = buff.left(128);
            buff.remove(0, 128);
            innerMutex->unlock();
        }
        else
        {
            innerMutex->lock();
            chunk = buff;
            buff.clear();
            innerMutex->unlock();
        }
        showDataMutex->lock();
        showDataMutex->unlock();
        emit sigReceivedData(chunk, ReplaceBadSymbols(chunk));
    }
}

void ReadPortThread::clear()
{
    innerMutex->lock();
    buff.clear();
    innerMutex->unlock();
}
