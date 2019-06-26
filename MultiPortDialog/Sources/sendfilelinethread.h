#ifndef SENDFILELINETHREAD_H
#define SENDFILELINETHREAD_H

#include <QThread>

class QMutex;

class SendFileLineThread : public QThread {
    Q_OBJECT

private:
    int rate;
    bool terminate;
    QString filename;
    QMutex* writeMutex;
    unsigned long m_pauseMs;
    QByteArray m_eol;

public:
    explicit SendFileLineThread(QMutex* mutex, QObject* parent = 0);
    void setFileName(const QString& newFilename, int baudRate, unsigned long pauseMs, const QString& eol);
    void stop();

protected:
    void run();

signals:
    void sigInfo(const QString& str);
    void sigStarted(bool started);
    void sigPercent(int);
    void sigSendData(const QByteArray& data);
};

#endif // SENDFILELINETHREAD_H
