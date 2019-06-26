#ifndef FORMSETHIGHLIGHT_H
#define FORMSETHIGHLIGHT_H

#include <QDialog>
#include <QCloseEvent>

class DialogHighlighter;

namespace Ui {
class FormSetHighlight;
}

class FormSetHighlight : public QDialog
{
    Q_OBJECT

public:
    explicit FormSetHighlight(QWidget *parent = 0);
    ~FormSetHighlight();

private slots:
    void on_btAddHex_clicked();
    void on_btOK_clicked();

private:
    Ui::FormSetHighlight *ui;
    DialogHighlighter *highlightGetter;

protected:
    void closeEvent(QCloseEvent *e);

signals:
    void accepted();
};

#endif // FORMSETHIGHLIGHT_H
