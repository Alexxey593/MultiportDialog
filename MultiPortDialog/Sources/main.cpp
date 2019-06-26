#include <QApplication>
#include <QTextCodec>
#include "mainwindow.h"
#include <QTranslator>
#include <QLocale>
#include <QSettings>

int main(int argc, char *argv[])
{
    QApplication::setApplicationName("MultiPortDialog");
//    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication a(argc, argv);

    QTranslator translator;
    QString loc = "en";
    QSettings settings("AVStolyarov", "MultiPortDialog");
    settings.beginGroup("Pref");
    QString language = settings.value("language", "unknown").toString();
    settings.endGroup();
    if(language != "unknown" && language.size() == 2)
        loc = language;
    else if(QLocale::system().name() == "ru_RU")
        loc == "ru";
    if(loc == "ru") {
        translator.load(":/translation/MultiPortDialog_ru");
        a.installTranslator(&translator);
    }

    MainWindow w(loc);
    w.show();

    return a.exec();
}
