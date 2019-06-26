#include "PTCR96.h"
#include <QStringList>
#include <QTranslator>
#include "formopu.h"

PTCR96::PTCR96(QObject* parent)
    : QObject(parent)
    , ptcr96Started(false)
{
    opuForm = new FormOPU;
    connect(opuForm, SIGNAL(sendData(QByteArray)), SIGNAL(sendData(QByteArray)));
    connect(opuForm, SIGNAL(closed()), SLOT(mainWidgetClosed()));
}

QStringList PTCR96::DataProcessors() const
{
    return QStringList() << "PTCR96";
}

void PTCR96::startProcessor(QString procName)
{
    if (procName == "PTCR96") {
        if (!ptcr96Started) {
            ptcr96Started = true;
            opuForm->show();
        }
    }
}

void PTCR96::stopProcessor(QString procName)
{
    if (procName == "PTCR96") {
        if (ptcr96Started) {
            ptcr96Started = false;
            opuForm->hide();
        }
    }
}

bool PTCR96::processData(QString procName, QByteArray& data)
{
    if (procName == "PTCR96" && ptcr96Started) {
        opuForm->dataFromOPU(data);
        return true;
    }
    return false;
}

QIcon PTCR96::icon(QString procName)
{
    if (procName == "PTCR96") {
        return QIcon(":/im/icon.png");
    }
    else
        return QIcon();
}

QString PTCR96::friedlyName(QString procName)
{
    if (procName == "PTCR96")
        return QString(tr("PTZ control by PTCR96"));
    else
        return QString();
}

QTranslator* PTCR96::pluginTranslator(QString language)
{
    QTranslator* translator = new QTranslator(this);
    if (translator->load(":/translation/PTCR96Plugin_" + language.toLower() + ".qm"))
        return translator;
    else {
        delete translator;
        return new QTranslator(this);
    }
}

bool PTCR96::isStarted()
{
    return ptcr96Started;
}

void PTCR96::mainWidgetClosed()
{
    stopProcessor(QString("PTCR96"));
}
