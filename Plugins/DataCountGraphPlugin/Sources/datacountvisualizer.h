#ifndef DATACOUNTVISUALIZER_H
#define DATACOUNTVISUALIZER_H

#include <QObject>
#include <QtPlugin>
#include "CPDInterfaces.h"
#include <QTimer>

class MemCustomPlot;

class DataCountVisualizer : public QObject, public DataProcessingInterface
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID DataProcessingInterface_iid FILE "DataCountGraphPlugin.json")
#endif // QT_VERSION >= 0x050000
    Q_INTERFACES(DataProcessingInterface)

public:
    DataCountVisualizer(QObject *parent = 0);
    QStringList DataProcessors() const;
    void startProcessor(QString procName);
    void stopProcessor(QString procName);
    bool processData(QString procName, QByteArray &data);
    QIcon icon(QString procName);
    QString friedlyName(QString procName);
    QTranslator *pluginTranslator(QString language);
    bool isStarted();

private:
    double dataCount;
    QTimer timerSecond;
    bool DataCountPlotStarted;
    MemCustomPlot* dataPlot;

private slots:
    void slotTimerSecond();
    void slotDataCountPlotClosed();
    void guiLanguageChanged();
};

#endif // DATACOUNTVISUALIZER_H
