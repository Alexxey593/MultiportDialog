#include "datacountvisualizer.h"
#include <QStringList>
#include "memcustomplot.h"
#include <QTranslator>

DataCountVisualizer::DataCountVisualizer(QObject* parent)
    : QObject(parent)
    , dataCount(0)
    , DataCountPlotStarted(false)
{
    dataPlot = new MemCustomPlot;
    connect(dataPlot, SIGNAL(closed()), SLOT(slotDataCountPlotClosed()));
    connect(dataPlot, SIGNAL(languageChanged()), SLOT(guiLanguageChanged()));
    dataPlot->setWindowIcon(QIcon(":/DataCountGraphPlugin/im/Graph.png"));
    dataPlot->setWindowTitle(tr("Data count plot", "График получения данных"));

    connect(&timerSecond, SIGNAL(timeout()), SLOT(slotTimerSecond()));
    timerSecond.start(1000);
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(DataCountGraphPlugin, DataCountVisualizer)
#endif // QT_VERSION < 0x050000

QStringList DataCountVisualizer::DataProcessors() const
{
    return QStringList() << "DataCountPlot";
}

void DataCountVisualizer::startProcessor(QString procName)
{
    if (procName == "DataCountPlot") {
        if (!DataCountPlotStarted) {
            DataCountPlotStarted = true;
            dataPlot->setGeometry(50, 50, 400, 400);
            dataPlot->addPoint(0.0);
            dataPlot->show();
        }
    }
}

void DataCountVisualizer::stopProcessor(QString procName)
{
    if (procName == "DataCountPlot") {
        if (DataCountPlotStarted) {
            DataCountPlotStarted = false;
            dataPlot->hide();
            dataPlot->clear();
        }
    }
}

bool DataCountVisualizer::processData(QString procName, QByteArray& data)
{
    if (procName == "DataCountPlot" && DataCountPlotStarted) {
        dataCount += data.size();
        return true;
    }
    return false;
}

QIcon DataCountVisualizer::icon(QString procName)
{
    if (procName == "DataCountPlot") {
        return QIcon(":/DataCountGraphPlugin/im/Graph.png");
    }
    else
        return QIcon();
}

QString DataCountVisualizer::friedlyName(QString procName)
{
    if (procName == "DataCountPlot")
        return QString(tr("Data count plot", "График получения данных"));
    else
        return QString();
}

QTranslator* DataCountVisualizer::pluginTranslator(QString language)
{
    QTranslator* translator = new QTranslator(this);
    if (translator->load(":/DataCountGraphPlugin/translation/DataCountGraphPlugin_" + language.toLower() + ".qm"))
        return translator;
    else {
        delete translator;
        return new QTranslator(this);
    }
}

bool DataCountVisualizer::isStarted()
{
    return DataCountPlotStarted;
}

void DataCountVisualizer::slotTimerSecond()
{
    if (DataCountPlotStarted) {
        dataPlot->addPoint(dataCount);
        dataCount = 0;
    }
}

void DataCountVisualizer::slotDataCountPlotClosed()
{
    stopProcessor(QString("DataCountPlot"));
}

void DataCountVisualizer::guiLanguageChanged()
{
    dataPlot->setWindowTitle(tr("Data count plot", "График получения данных"));
}
