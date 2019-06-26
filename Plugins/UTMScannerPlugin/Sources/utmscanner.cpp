#include "utmscanner.h"
#include <QStringList>
#include <QTranslator>

#include "formscanner.h"

UTMScanner::UTMScanner(QObject* parent)
    : QObject(parent)
    , UTMScannerStarted(false)
{
    scannerForm = new FormScanner;
    connect(scannerForm, SIGNAL(sendData(QByteArray)), SIGNAL(sendData(QByteArray)));
    connect(scannerForm, SIGNAL(closed()), SLOT(mainWidgetClosed()));
}

QStringList UTMScanner::DataProcessors() const
{
    return QStringList() << "UTMScanner";
}

void UTMScanner::startProcessor(QString procName)
{
    if (procName == "UTMScanner") {
        if (!UTMScannerStarted) {
            UTMScannerStarted = true;
            scannerForm->show();
        }
    }
}

void UTMScanner::stopProcessor(QString procName)
{
    if (procName == "UTMScanner") {
        if (UTMScannerStarted) {
            UTMScannerStarted = false;
            scannerForm->hide();
        }
    }
}

bool UTMScanner::processData(QString procName, QByteArray& data)
{
    if (procName == "UTMScanner" && UTMScannerStarted) {
        scannerForm->dataFromSensor(data);
        return true;
    }
    return false;
}

QIcon UTMScanner::icon(QString procName)
{
    if (procName == "UTMScanner") {
        return QIcon(":/UTMScannerPlugin/im/icon.png");
        return QIcon();
    }
    else
        return QIcon();
}

QString UTMScanner::friedlyName(QString procName)
{
    if (procName == "UTMScanner")
        return QString(tr("Laser scanner UTM"));
    else
        return QString();
}

QTranslator* UTMScanner::pluginTranslator(QString language)
{
    QTranslator* translator = new QTranslator(this);
    if (translator->load(":/UTMScannerPlugin/translation/UTMScannerPlugin_" + language.toLower() + ".qm"))
        return translator;
    else {
        delete translator;
        return new QTranslator(this);
    }
}

bool UTMScanner::isStarted()
{
    return UTMScannerStarted;
}

void UTMScanner::mainWidgetClosed()
{
    stopProcessor(QString("UTMScanner"));
}
