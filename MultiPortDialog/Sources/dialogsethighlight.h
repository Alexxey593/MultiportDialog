#ifndef DIALOGSETHIGHLIGHT_H
#define DIALOGSETHIGHLIGHT_H

#include <QDialog>
#include <QCloseEvent>

class DialogHighlighter;

namespace Ui {
class DialogSetHighlight;
}

class DialogSetHighlight : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSetHighlight(QWidget *parent = 0);
    ~DialogSetHighlight();
    QList<QPair<QByteArray, QColor> > getHighlighters() const;

private slots:
    void on_btAddHex_clicked();

private:
    Ui::DialogSetHighlight *ui;
    DialogHighlighter *highlightGetter;
    bool createNewHighlight;
    QList<QPair<QByteArray, QColor> > hexHighlighters;
    QList<QPair<QByteArray, QColor> > enabledHighlighters;

private slots:
    void highlighterAccepted();
    void highlighterDeclined();
    void on_btOK_clicked();

    void on_btRemoveHex_clicked();

protected:
    void closeEvent(QCloseEvent *e);
};

#endif // DIALOGSETHIGHLIGHT_H
