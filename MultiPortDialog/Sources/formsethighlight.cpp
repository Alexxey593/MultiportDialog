#include "formsethighlight.h"
#include "ui_formsethighlight.h"
#include "dialoghighlighter.h"

FormSetHighlight::FormSetHighlight(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FormSetHighlight)
{
    ui->setupUi(this);
    highlightGetter = new DialogHighlighter(this);
}

FormSetHighlight::~FormSetHighlight()
{
    delete ui;
}

void FormSetHighlight::on_btAddHex_clicked()
{
    highlightGetter->show();
}

void FormSetHighlight::on_btOK_clicked()
{
    close();
}

void FormSetHighlight::closeEvent(QCloseEvent *e)
{
    emit accepted();
    QDialog::closeEvent(e);
}
