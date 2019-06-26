#include "sendfilelinethread.h"
#include <QElapsedTimer>
#include <QFile>
#include <QMutex>
#include <QTextStream>

SendFileLineThread::SendFileLineThread(QMutex* mutex, QObject* parent)
    : QThread(parent)
{
    rate = 9600;
    terminate = false;
    writeMutex = mutex;
}

void SendFileLineThread::setFileName(const QString& newFilename, int baudRate, unsigned long pauseMs, const QString& eol)
{
    filename = newFilename;
    rate = baudRate;
    m_pauseMs = pauseMs;
    if (eol == "-")
        m_eol = QByteArray();
    else if (eol == "0D")
        m_eol = QByteArray("\x0d", 1);
    else if (eol == "0A")
        m_eol = QByteArray("\x0a", 1);
    else if (eol == "0D0A")
        m_eol = QByteArray("\x0d\x0a", 2);
}

void SendFileLineThread::stop()
{
    terminate = true;
}

void SendFileLineThread::run()
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
    QTextStream stream(&file);
    emit sigStarted(true);

    qint64 filesize = file.size();
    int packSize = 512;
    if (rate < 512 * 10)
        packSize = rate / 10;
    int oldPercents = -1;
    QElapsedTimer elapse;
    while (!stream.atEnd()) {
        if (terminate) {
            emit sigInfo(tr("Cancel", "Отмена"));
            break;
        }
        elapse.start();
        QString line = stream.readLine();
        QByteArray data = line.toLocal8Bit();
        while (data.size() > 0) {
            if (terminate) {
                emit sigInfo(tr("Cancel", "Отмена"));
                break;
            }
            QByteArray chunck;
            if (data.size() > packSize) {
                chunck = data.left(packSize);
                data.remove(0, packSize);
            } else {
                chunck = data;
                data.clear();
            }
            writeMutex->lock();
            emit sigSendData(chunck);
            writeMutex->unlock();
            unsigned long pause = static_cast<unsigned long>(chunck.size() * 10.0 / (double(rate) / 1000.0) + 1.0);
            this->msleep(pause);
            int percents = static_cast<int>((stream.pos() * 100) / filesize);
            if (percents != oldPercents) {
                emit sigPercent(percents);
                oldPercents = percents;
            }
        }
        if (!stream.atEnd()) {
            writeMutex->lock();
            emit sigSendData(m_eol);
            writeMutex->unlock();
            unsigned long ms = static_cast<unsigned long>(elapse.elapsed());
            if (ms < m_pauseMs)
                this->msleep(m_pauseMs - ms);
        }
    }
    file.close();
    emit sigStarted(false);
    emit sigInfo(tr("Transmission completed", "Передача завершена"));
}
