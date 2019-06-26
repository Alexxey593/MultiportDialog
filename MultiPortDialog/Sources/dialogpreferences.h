#ifndef DIALOGPREFERENCES_H
#define DIALOGPREFERENCES_H

#include <QDialog>

namespace Ui {
class DialogPreferences;
}

class DialogPreferences : public QDialog {
    Q_OBJECT

public:
    struct Settings {
        bool       savePos;
        int        bytesPerLine;
        QByteArray codec;
        QString    language;
        int        fontSize;

        Settings() : savePos(false), bytesPerLine(16), language("en"), fontSize(8) {}
    };

public:
    explicit DialogPreferences(QString language = "ru", QWidget *parent = 0);
    ~DialogPreferences();
    Settings settings() const;

private slots:
    void on_btOK_clicked();
    void on_btCancel_clicked();

private:
    Ui::DialogPreferences *ui;
    Settings currentSettings;

private:
    void updateSettings();
    void restoreSettings();
};
#endif // DIALOGPREFERENCES_H
