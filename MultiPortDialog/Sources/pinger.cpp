#include "pinger.h"

Pinger::Pinger(QObject *parent) :
    QObject(parent)
{
    codec = QTextCodec::codecForName("windows-866");
    pingProc = new QProcess(this);
    connect(pingProc, SIGNAL(error(QProcess::ProcessError)), SLOT(error(QProcess::ProcessError)));
    connect(pingProc, SIGNAL(finished(int, QProcess::ExitStatus)), SLOT(finished(int, QProcess::ExitStatus)));
    connect(pingProc, SIGNAL(started()), SIGNAL(started()));
    connect(pingProc, SIGNAL(stateChanged(QProcess::ProcessState)), SLOT(stateChanged(QProcess::ProcessState)));
    connect(pingProc, SIGNAL(readyRead()), SLOT(dataReady()));
}

void Pinger::ping(const QString& ip)
{
    if (pingProc->state() != QProcess::NotRunning)
        stop();
    QStringList arguments;
    arguments << ip << "-t";
    pingProc->start("ping.exe", arguments, QIODevice::ReadOnly);
}

void Pinger::stop()
{
    pingProc->close();
}

void Pinger::error(QProcess::ProcessError error)
{
    switch (error) {
      case QProcess::FailedToStart:
          emit info("Pinger: The process failed to start. Either the invoked program is missing, or you may have insufficient permissions to invoke the program.");
          break;

      case QProcess::Crashed:
          emit info("Pinger: The process crashed some time after starting successfully.");
          break;

      case QProcess::Timedout:
          emit info("Pinger: The last waitFor...() function timed out. The state of QProcess is unchanged, and you can try calling waitFor...() again.");
          break;

      case QProcess::WriteError:
          emit info("Pinger: An error occurred when attempting to write to the process. For example, the process may not be running, or it may have closed its input channel.");
          break;

      case QProcess::ReadError:
          emit info("Pinger: An error occurred when attempting to read from the process. For example, the process may not be running.");
          break;

      case QProcess::UnknownError:
          emit info("Pinger: An unknown error occurred. This is the default return value of error().");
          break;
    } // switch
} // error

void Pinger::stateChanged(QProcess::ProcessState newState)
{
    Q_UNUSED(newState)
// switch(newState) {
// case QProcess::NotRunning:
// emit info("Pinger: The process is not running.");
// break;
// case QProcess::Starting:
// emit info("Pinger: The process is starting, but the program has not yet been invoked.");
// break;
// case QProcess::Running:
// emit info("Pinger: The process is running and is ready for reading and writing.");
// break;
// }
} // stateChanged

void Pinger::finished(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitCode)
    Q_UNUSED(exitStatus)
// switch(exitStatus) {
// case QProcess::NormalExit:
// emit info(tr("PING завершен (%1). Нормальное завершение.").arg(exitCode));
// break;
// case QProcess::CrashExit:
// emit info(tr("PING завершен (%1). Аварийное завершение.").arg(exitCode));
// break;
// default:
// break;
// }
} // finished

void Pinger::dataReady()
{
// QByteArray ba = pingProc->readAllStandardOutput();
    for ( ; ; ) {
        QByteArray ba = pingProc->readLine();
        if (ba.isEmpty())
            return;
        if (codec)
            buff.append(codec->toUnicode(ba));
        else
            buff.append(ba);
        if (buff.endsWith("\r\n")) {
            buff.chop(2);
            parseString(buff);
            buff.clear();
        }
    }
} // dataReady

void Pinger::parseString(const QString& str)
{
    if (str.length() < 10)
        return;
    if (str.endsWith(':'))
        return;
    QRegExp digits("[0-9]");
// emit info(str);
    if (!str.contains(digits)) { // Нет цифр, скорее всего "Заданный узел не доступен"
        emit pingReceived(-1);
        emit info("Объект не доступен");
    } else if (str.contains("<1")) {
        emit pingReceived(0);
        emit info("Супер соединение");
    } else if (str.contains("=")) {
        QStringList list = str.split('=');
        if (list.size() != 4) {
            emit info("Неверный формат пинга");
            return;
        }
        QString timeStr;
        foreach(QChar c, list.at(2)) {
            if (c.isDigit())
                timeStr.append(c);
            else
                break;
        }
        bool ok = false;
        int pingTime = timeStr.toInt(&ok);
        if (ok) {
            emit pingReceived(pingTime);
// emit info(QString("Ping = %1").arg(pingTime));
        } else
            emit info("Неверное число пинга");
    } else {
        emit pingReceived(-1);
        emit info("Объект не доступен");
    }
} // parseString
