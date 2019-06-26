#include "filetuloader.h"
#include <QFile>
#include <QTextStream>
#include <QLocale>

FileTULoader::FileTULoader(QObject *parent) : QObject(parent), currIndex(0), prevH(999.99), prevV(999.99)
{
    connect(&timer, SIGNAL(timeout()), SLOT(tuTick()));
}

FileTULoader::~FileTULoader()
{}

void FileTULoader::tuTick()
{
    if (currIndex >= tuList.size()) {
        emit info("Playing stopped.");
        timer.stop();
    } else {
        TUStruct tu = tuList.at(currIndex);
        emit playedTime(tu.time.toString("hh:mm:ss"));
//        emit info(QString("%1 %2 %3 %4 %5").arg(tu.time.toString("hh:mm:ss")).arg(tu.dist).arg(tu.speed).arg(tu.hAngle).arg(tu.vAngle));
        double hAngle = 999.99;
        if(qAbs(tu.hAngle - prevH) > 0.03) {
            prevH = tu.hAngle;
            hAngle = tu.hAngle;
        }
        double vAngle = 999.99;
        if(qAbs(tu.vAngle - prevV) > 0.03) {
            prevV = tu.vAngle;
            vAngle = tu.vAngle;
        }
        emit opuPos(hAngle, vAngle);
        currIndex++;
    }
} // tuTick

bool FileTULoader::openFile(QString filename)
{
    tuList.clear();
    QFile file(filename);
    if (!file.open(QFile::ReadOnly))
        return false;
    QLocale locale;
    QChar dp = locale.decimalPoint();
    QTextStream text(&file);
    forever {
        QString str = text.readLine();
        if(str.isNull())
            break;
        emit info(str);
        for (int i = 0; i < str.length(); i++)
            if (str[i] == ',')
                str[i] = dp;
        QStringList list = str.split(' ', QString::SkipEmptyParts);
        if (list.size() == 6) {
            TUStruct tu;
            tu.time = QTime::fromString(list.at(0), "hh:mm:ss");
            tu.dist = list.at(1).toDouble();
            tu.speed = list.at(2).toDouble();
            tu.hAngle = list.at(3).toDouble();
            tu.vAngle = list.at(4).toDouble() - 90.0;
            emit info(QString("%1 %2 %3 %4 %5").arg(tu.time.toString("hh:mm:ss")).arg(tu.dist).arg(tu.speed).arg(tu.hAngle).arg(tu.vAngle));
            tuList.append(tu);
        }
    }
    return true;
} // openFile

void FileTULoader::play()
{
    if (tuList.size() > 1) {
        emit info("Start playing");
        int delay = tuList.at(0).time.msecsTo(tuList.at(1).time);
        emit info(QString("Delay = %1 ms").arg(delay));
        currTime = tuList.at(0).time;
        currIndex = 0;
        prevH = 999.99;
        prevV = 999.99;
        timer.start(delay);
    } else
        emit info("Playing stopped.");
}

void FileTULoader::stop()
{
    emit info("Playing stopped.");
    timer.stop();
}
