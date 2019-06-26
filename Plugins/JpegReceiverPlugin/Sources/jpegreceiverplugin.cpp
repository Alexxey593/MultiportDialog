#include "jpegreceiverplugin.h"
#include <QStringList>
#include <QTranslator>
#include "jpegreceiverform.h"

JpegReceiverPlugin::JpegReceiverPlugin(QObject* parent)
    : QObject(parent)
    , JpegReceiverStarted(false)
{
    jpegReceiverForm = new JpegReceiverForm;
    connect(jpegReceiverForm, &JpegReceiverForm::closed, this, &JpegReceiverPlugin::mainWidgetClosed);
}

QStringList JpegReceiverPlugin::DataProcessors() const
{
    return QStringList() << "JpegReceiver";
}

void JpegReceiverPlugin::startProcessor(QString procName)
{
    if (procName == "JpegReceiver") {
        if (!JpegReceiverStarted) {
            JpegReceiverStarted = true;
            jpegReceiverForm->show();
            jpegReceiverForm->start();
        }
    }
}

void JpegReceiverPlugin::stopProcessor(QString procName)
{
    if (procName == "JpegReceiver") {
        if (JpegReceiverStarted) {
            JpegReceiverStarted = false;
            jpegReceiverForm->hide();
            jpegReceiverForm->stop();
        }
    }
}

bool JpegReceiverPlugin::processData(QString procName, QByteArray& data)
{
    if ((procName == "JpegReceiver") && JpegReceiverStarted) {
        jpegReceiverForm->receiveData(data);
        return true;
    }
    return false;
}

QIcon JpegReceiverPlugin::icon(QString procName)
{
    if (procName == "JpegReceiver")
        return QIcon(":/JpegReceiverPlugin/im/icon.png");
    else
        return QIcon();
}

QString JpegReceiverPlugin::friedlyName(QString procName)
{
    if (procName == "JpegReceiver")
        return QString(tr("Jpeg Receiver"));
    else
        return QString();
}

QTranslator* JpegReceiverPlugin::pluginTranslator(QString language)
{
    QTranslator* translator = new QTranslator(this);
    if (translator->load(":/JpegReceiverPlugin/translation/JpegReceiverPlugin_" + language.toLower() + ".qm"))
        return translator;
    else {
        delete translator;
        return new QTranslator(this);
    }
}

bool JpegReceiverPlugin::isStarted()
{
    return JpegReceiverStarted;
}

void JpegReceiverPlugin::mainWidgetClosed()
{
    stopProcessor(QString("JpegReceiver"));
}
