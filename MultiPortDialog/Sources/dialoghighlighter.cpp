#include "dialoghighlighter.h"
#include "ui_dialoghighlighter.h"
#include <QPainter>

DialogHighlighter::DialogHighlighter(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogHighlighter)
{
    ui->setupUi(this);
    setWindowTitle(tr("Set highlighter value and color"));
    // Валидатор для Hex строки
    QRegExp rxHex("[0-9a-fA-F]+");
    QValidator *valHex = new QRegExpValidator(rxHex, this);
    ui->leHex->setValidator(valHex);
}

DialogHighlighter::~DialogHighlighter()
{
    delete ui;
}

QByteArray DialogHighlighter::getHex() const
{
    return arrHex;
}

QColor DialogHighlighter::getColor() const
{
    return ui->imColor->color();
}

void DialogHighlighter::reset()
{
    arrHex.clear();
    ui->leASCII->clear();
    ui->leHex->clear();
    ui->imColor->setColor(Qt::green);
}

void DialogHighlighter::showEvent(QShowEvent *e)
{
    reset();
    QDialog::showEvent(e);
}

void DialogHighlighter::on_buttonBox_accepted()
{
    QString str = ui->leHex->text();
    if(str.length() % 2)
        str.prepend("0");
    arrHex = QByteArray::fromHex(str.toLocal8Bit());
    emit highlighterAccepted();
}

void DialogHighlighter::on_leHex_textEdited(const QString &arg1)
{
    QString str = arg1;
    if(str.length() % 2)
        str.prepend("0");
    QByteArray arrHex2 = QByteArray::fromHex(str.toLocal8Bit());
    ui->leASCII->setText(arrHex2);
}

void DialogHighlighter::on_leASCII_textEdited(const QString &arg1)
{
    ui->leHex->setText(arg1.toLocal8Bit().toHex());
}

void DialogHighlighter::on_leHex_textChanged(const QString &arg1)
{
    if(arg1.isEmpty())
        ui->leASCII->clear();
}

void DialogHighlighter::on_leASCII_textChanged(const QString &arg1)
{
    if(arg1.isEmpty())
        ui->leHex->clear();
}
