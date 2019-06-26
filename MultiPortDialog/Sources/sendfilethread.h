#ifndef SENDFILETHREAD_H
#define SENDFILETHREAD_H

#include <QThread>

class QMutex;

class SendFileThread : public QThread {
    Q_OBJECT

private:
    int rate;
    bool terminate;
    QString filename;
    QMutex* writeMutex;

public:
    explicit SendFileThread(QMutex* mutex, QObject* parent = 0);
    void setFileName(QString newFilename, int baudRate);
    void stop();

protected:
    void run();

signals:
    void sigInfo(QString str);
    void sigStarted(bool started);
    void sigPercent(int);
    void sigSendData(QByteArray data);

public slots:
};

#endif // SENDFILETHREAD_H
