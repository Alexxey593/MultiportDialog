#ifndef PINGER_H
#define PINGER_H

#include <QObject>
#include <QProcess>
#include <QTextCodec>

class Pinger : public QObject {
    Q_OBJECT

private:
    QProcess* pingProc;
    QTextCodec* codec;
    QString buff;

public:
    explicit Pinger(QObject* parent = nullptr);

signals:
    void pingReceived(int ms);
    void started();
    void finished();
    void info(QString info);

public slots:
    void ping(const QString& ip);
    void stop();

private slots:
    void error(QProcess::ProcessError error);
    void stateChanged(QProcess::ProcessState newState);
    void finished(int exitCode, QProcess::ExitStatus exitStatus);
    void dataReady();
    void parseString(const QString& str);
};

#endif // PINGER_H
