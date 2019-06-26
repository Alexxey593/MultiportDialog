#ifndef CPDINTERFACES_H
#define CPDINTERFACES_H

#include <QtPlugin>
#include <QIcon>

class QTranslator;

class DataProcessingInterface {
public:
    virtual ~DataProcessingInterface() {}
    virtual QStringList DataProcessors() const = 0;
    virtual void startProcessor(QString procName) = 0;
    virtual void stopProcessor(QString procName) = 0;
    virtual bool processData(QString procName, QByteArray &data) = 0;
    virtual QIcon icon(QString procName)
    {
        Q_UNUSED(procName);
        return QIcon();
    }
    virtual QString friedlyName(QString procName)
    {
        Q_UNUSED(procName);
        return QString();
    }
    virtual QTranslator *pluginTranslator(QString language) = 0;
    virtual bool isStarted() = 0;
};

QT_BEGIN_NAMESPACE

#define DataProcessingInterface_iid "ru.mail.Alexxey593.Programms.MultiPortDialog.DataProcessingInterface/1.2"
Q_DECLARE_INTERFACE(DataProcessingInterface, DataProcessingInterface_iid)

QT_END_NAMESPACE

#endif // CPDINTERFACES_H
