#include <QDataStream>
#include <QFile>
#include "sendfilethread.h"
#include <QMutex>
#include <QDebug>

SendFileThread::SendFileThread(QMutex* mutex, QObject* parent)
    : QThread(parent)
{
    rate = 9600;
    terminate = false;
    writeMutex = mutex;
}

void SendFileThread::setFileName(QString newFilename, int baudRate)
{
    filename = newFilename;
    rate = baudRate;
}

void SendFileThread::stop()
{
    terminate = true;
}

void SendFileThread::run()
{
    terminate = false;
    QFile file(filename);
    if (!file.exists()) {
        emit sigInfo(tr("File not found", "Файл не найден."));
        return;
    }
    if (!file.open(QIODevice::ReadOnly)) {
        emit sigInfo(tr("File not opened.", "Файл не открыт."));
        return;
    }
    QDataStream stream(&file);
    emit sigStarted(true);

    quint64 filesize = file.size();
    quint64 transmitted = 0;
    int packSize = 512;
    if (rate < 512 * 10)
        packSize = rate / 10;
    int oldPercents = -1;
    int pause = packSize * 10.0 / (double(rate) / 1000.0) + 1.0;
    qDebug() << "packSize = " << packSize << "Pause = " << pause;
    char* buf = new char[packSize];
    while (!stream.atEnd()) {
        if (terminate) {
            emit sigInfo(tr("Cancel", "Отмена"));
            break;
        }
        int len = stream.readRawData(buf, packSize);
        if (len <= 0)
            emit sigInfo(tr("Error"));
        writeMutex->lock();
        emit sigSendData(QByteArray(buf, len));
        writeMutex->unlock();
        this->msleep(pause);
        transmitted += len;
        int percents = (transmitted * 100) / filesize;
        if (percents != oldPercents) {
            emit sigPercent(percents);
            oldPercents = percents;
        }
    }
    file.close();
    delete[] buf;
    emit sigStarted(false);
    emit sigInfo(tr("Transmission completed", "Передача завершена"));
}
