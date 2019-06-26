#include "dialogpreferences.h"
#include "ui_dialogpreferences.h"
#include <QTextCodec>
#include <QSettings>

DialogPreferences::DialogPreferences(QString language, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogPreferences)
{
    ui->setupUi(this);
    QList<QByteArray> list = QTextCodec::availableCodecs();
    foreach(QByteArray codec, list) {
        ui->comboCodec->addItem(QLatin1String(codec), codec);
    }

    QSettings settings("AVStolyarov", "MultiPortDialog");
    settings.beginGroup("Pref");
    ui->cbSavePos->setChecked(settings.value("savePos", true).toBool());
    ui->sbBytesPerLine->setValue(settings.value("BPL", 16).toInt());
    QByteArray codec = settings.value("codec").toByteArray();
    ui->sbFontSize->setValue(settings.value("fontSize", 8).toInt());

    if(!list.isEmpty()) {
        if(list.contains(codec))
            ui->comboCodec->setCurrentIndex(ui->comboCodec->findText(codec, Qt::MatchExactly));
        else if(list.contains("windows-1251"))
            ui->comboCodec->setCurrentIndex(ui->comboCodec->findText("windows-1251", Qt::MatchExactly));
        if(ui->comboCodec->currentIndex() < 0 && ui->comboCodec->count() > 0)
            ui->comboCodec->setCurrentIndex(0);
    }
    else {
        // Ошибка получения списка кодеков
    }
//    QString language = settings.value("language", "en").toString();
    ui->comboLanguage->setCurrentIndex(ui->comboLanguage->findText(language));
    settings.endGroup();

    updateSettings();
}

DialogPreferences::~DialogPreferences()
{
    QSettings settings("AVStolyarov", "MultiPortDialog");
    settings.beginGroup("Pref");
    settings.setValue("savePos", currentSettings.savePos);
    settings.setValue("BPL", currentSettings.bytesPerLine);
    settings.setValue("codec", currentSettings.codec);
    settings.setValue("language", currentSettings.language);
    settings.setValue("fontSize", currentSettings.fontSize);
    settings.endGroup();
    delete ui;
}

DialogPreferences::Settings DialogPreferences::settings() const
{
    return currentSettings;
}

void DialogPreferences::updateSettings()
{
    currentSettings.savePos = ui->cbSavePos->isChecked();
    currentSettings.bytesPerLine = ui->sbBytesPerLine->value();
    QByteArray codec =  ui->comboCodec->itemData(ui->comboCodec->currentIndex()).toByteArray();
    currentSettings.codec = codec;
    if(!codec.isNull() && !codec.isEmpty())
        QTextCodec::setCodecForLocale(QTextCodec::codecForName(codec));
    currentSettings.language = ui->comboLanguage->currentText();
    currentSettings.fontSize = ui->sbFontSize->value();
}

void DialogPreferences::restoreSettings()
{
    ui->cbSavePos->setChecked(currentSettings.savePos);
    ui->sbBytesPerLine->setValue(currentSettings.bytesPerLine);
    ui->comboCodec->setCurrentIndex(ui->comboCodec->findText(currentSettings.codec));
    ui->comboLanguage->setCurrentIndex(ui->comboLanguage->findText(currentSettings.language));
    ui->sbFontSize->setValue(currentSettings.fontSize);
}

void DialogPreferences::on_btOK_clicked()
{
    updateSettings();
    accept();
}

void DialogPreferences::on_btCancel_clicked()
{
    restoreSettings();
    reject();
}
