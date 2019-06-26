#ifndef DIALOGHIGHLIGHTER_H
#define DIALOGHIGHLIGHTER_H

#include <QDialog>
#include <QShowEvent>

namespace Ui {
class DialogHighlighter;
}

class DialogHighlighter : public QDialog {
    Q_OBJECT

public:
    explicit DialogHighlighter(QWidget *parent = 0);
    ~DialogHighlighter();
    QByteArray getHex() const;
    QColor getColor() const;

private:
    Ui::DialogHighlighter *ui;
    QByteArray arrHex;

public slots:
    void reset();

protected:
    void showEvent(QShowEvent *e);

private slots:
    void on_buttonBox_accepted();
    void on_leHex_textEdited(const QString &arg1);

    void on_leASCII_textEdited(const QString &arg1);


    void on_leHex_textChanged(const QString &arg1);

    void on_leASCII_textChanged(const QString &arg1);

signals:
    void highlighterAccepted();
};
#endif // DIALOGHIGHLIGHTER_H
