#include <QMutexLocker>
#include <QtAddOnSerialPort/serialport.h>
#include "sendthread.h"

void SendThread::run()
{
    terminate = false;
    if(!CommPort->isOpen())
    {
        emit sigInfo(tr("Порт не открыт"));
        return;
    }
    emit sigStarted(true);
    while(pauseList.size() < arrList.size())
    {
        pauseList.append(0);
    }
    pauseList[arrList.size()-1] = 0;
    for(int i = 0; i < arrList.size(); i++)
    {
        if(terminate)
        {
            emit sigInfo(tr("Отмена"));
            break;
        }
        sendMutex->lock();
        if(CommPort->isOpen())
        {
            int ret = CommPort->write(arrList.at(i));
            if(ret != arrList.at(i).size())
                emit sigInfo(tr("Error sending message %1").arg(i+1));
            emit sigInfo(tr("Message %1").arg(i+1));
        }
        sendMutex->unlock();
        msleep(pauseList.at(i));
    }
//    CommPort->clear(SerialPort::Output);
//    CommPort->flush();
    emit sigStarted(false);
    emit sigInfo(tr("Передача завершена"));
}

void SendThread::stop()
{
    terminate = true;
}


SendThread::SendThread(SerialPort *port, QMutex *mutex, QObject *parent) :
    QThread(parent)
{
    CommPort = port;
    sendMutex = mutex;
    terminate = false;
}

void SendThread::setData(QList<QByteArray> &messages, QList<int> &pauses)
{
    arrList = messages;
    pauseList = pauses;
}
