#include "jpegreceiver.h"

JpegReceiver::JpegReceiver(QObject* parent)
    : QThread(parent)
    , m_maxBufSize(10000000)
    , terminate(false)
{
}

void JpegReceiver::setMaxBufSize(int maxBufSize)
{
    m_maxBufSize = maxBufSize;
}

void JpegReceiver::stop()
{
    terminate = true;
}

void JpegReceiver::receiveData(QByteArray& data)
{
    if (data.isEmpty())
        return;
    if (!terminate) {
        QMutexLocker locker(&mutex);
        dataBuffer.append(data);
    }
}

void JpegReceiver::run()
{
    terminate = false;
    QByteArray data;
    bool wasFF = false;

    QByteArray frameBuf;
    QByteArray frameData;
    QImage m_image;
    bool frameStarted;
    while (!terminate) {
        mutex.lock();
        data = dataBuffer;
        dataBuffer.clear();
        mutex.unlock();
        if (data.isEmpty()) {
            msleep(5);
            continue;
        }
        int counter = 0;
        while (!terminate) {
            counter++;
            if (counter > 50) {
                frameStarted = false;
                frameData.clear();
                frameBuf.clear();
                emit message(tr("Watchdog...", "Завис"));
                break;
            }
            if (!frameStarted) {
                if (wasFF && data.startsWith('\xd8'))
                    data.push_front('\xff');
                wasFF = false;
                int pos = data.indexOf("\xff\xd8");
                if (pos < 0) {
                    if (data.endsWith('\xff'))
                        wasFF = true;
                    break;
                }
                else {
                    frameBuf.append("\xff\xd8");
                    data = data.right(data.size() - pos - 2);
                    emit bufferSize(frameBuf.size());
                }
                frameStarted = true;
            }
            if (frameStarted) {
                if (wasFF && data.startsWith('\xd9')) {
                    data.push_front('\xff');
                    if (frameBuf.endsWith('\xff'))
                        frameBuf.chop(1);
                }
                wasFF = false;
                int pos = data.indexOf("\xff\xd9");
                int posStart = data.indexOf("\xff\xd8");
                if ((posStart >= 0) && ((posStart < pos) || (pos < 0))) {
                    frameBuf.clear();
                    frameStarted = false;
                    emit message(tr("End of file lost..."));
                    emit bufferSize(0);
                    continue;
                }
                if (pos < 0) {
                    frameBuf.append(data);
                    emit bufferSize(frameBuf.size());
                    if (data.endsWith('\xff'))
                        wasFF = true;
                    if (frameBuf.size() > m_maxBufSize) {
                        emit message(tr("Buffer overflow"));
                        frameBuf.clear();
                        frameStarted = false;
                        emit bufferSize(0);
                    }
                    break;
                }
                QByteArray endOfFrame = data.left(pos + 2);
                frameBuf.append(endOfFrame);
                emit bufferSize(frameBuf.size());
                frameStarted = false;
                frameData = frameBuf;
                frameBuf.clear();
                emit bufferSize(0);
                m_image.loadFromData(frameData);
                if (!m_image.isNull()) {
                    emit imageReceived(m_image);
                }
                else
                    emit message(tr("Broken image"));
                if (pos < (data.size() - 2)) {
                    data = data.right(data.size() - pos - 2);
                    continue;
                }
                else {
                    break;
                }
            }
        }
        msleep(5);
    }
}
