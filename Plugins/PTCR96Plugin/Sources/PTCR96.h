#ifndef UTMSCANER_H
#define UTMSCANER_H

#include <QObject>
#include <QtPlugin>
#include "CPDInterfaces.h"

class FormOPU;

class PTCR96 : public QObject, public DataProcessingInterface
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID DataProcessingInterface_iid FILE "PTCR96Plugin.json")
#endif // QT_VERSION >= 0x050000
    Q_INTERFACES(DataProcessingInterface)

private:
    bool ptcr96Started;
    FormOPU *opuForm;

public:
    explicit PTCR96(QObject *parent = 0);
    QStringList DataProcessors() const;
    void startProcessor(QString procName);
    void stopProcessor(QString procName);
    bool processData(QString procName, QByteArray &data);
    QIcon icon(QString procName);
    QString friedlyName(QString procName);
    QTranslator *pluginTranslator(QString language);
    bool isStarted();

signals:
    void sendData(QByteArray data);

private slots:
    void mainWidgetClosed();

};

#endif // UTMSCANER_H
