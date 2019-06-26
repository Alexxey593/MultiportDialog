#ifndef JPEGRECEIVER_H
#define JPEGRECEIVER_H

#include <QMutex>
#include <QImage>
#include <QThread>

class JpegReceiver : public QThread {
    Q_OBJECT
public:
    explicit JpegReceiver(QObject* parent = 0);
    void setMaxBufSize(int maxBufSize);
    void stop();

private:
    int m_maxBufSize;
    bool terminate;
    QMutex mutex;
    QByteArray dataBuffer;

signals:
    void imageReceived(QImage image);
    void message(QString message);
    void bufferSize(int bufSize);

public slots:
    void receiveData(QByteArray& data);

protected:
    void run();
};

#endif // JPEGRECEIVER_H
