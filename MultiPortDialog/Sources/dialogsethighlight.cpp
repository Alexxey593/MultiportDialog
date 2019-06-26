#include "dialogsethighlight.h"
#include "ui_dialogsethighlight.h"
#include "dialoghighlighter.h"

DialogSetHighlight::DialogSetHighlight(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::DialogSetHighlight)
    , createNewHighlight(false)
{
    ui->setupUi(this);
    setWindowTitle(tr("Highlighters"));
    highlightGetter = new DialogHighlighter(this);
    connect(highlightGetter, &DialogHighlighter::highlighterAccepted, this, &DialogSetHighlight::highlighterAccepted); //SIGNAL(highlighterAccepted()), SLOT(highlighterAccepted()));
    connect(highlightGetter, &DialogHighlighter::rejected, this, &DialogSetHighlight::highlighterDeclined); //SIGNAL(rejected()), SLOT(highlighterDeclined()));

    QFile pFile(qApp->applicationDirPath() + "/highlighters.dat");
    if (pFile.open(QIODevice::ReadOnly)) {
        QDataStream PrefFile(&pFile);
        PrefFile >> hexHighlighters;
        QList<QListWidgetItem> list;
        PrefFile >> list;
        foreach (QListWidgetItem item, list) {
            QListWidgetItem* newItem = new QListWidgetItem();
            *newItem = item;
            newItem->setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsDragEnabled);
            ui->lvHex->addItem(newItem);
        }
        pFile.close();
        enabledHighlighters.clear();
        for (int i = 0; i < ui->lvHex->count(); i++) {
            if (ui->lvHex->item(i)->checkState() == Qt::Checked)
                enabledHighlighters.append(hexHighlighters.at(i));
        }
    }
}

DialogSetHighlight::~DialogSetHighlight()
{
    delete ui;
}

QList<QPair<QByteArray, QColor> > DialogSetHighlight::getHighlighters() const
{
    return enabledHighlighters;
}

void DialogSetHighlight::on_btAddHex_clicked()
{
    createNewHighlight = true;
    highlightGetter->show();
}

void DialogSetHighlight::highlighterAccepted()
{
    QByteArray arr = highlightGetter->getHex();
    if (arr.isEmpty())
        return;

    QColor color = highlightGetter->getColor();
    if (!color.isValid())
        return;

    if (createNewHighlight) {
        createNewHighlight = false;
        hexHighlighters.append(qMakePair(arr, color));
        QPixmap pix(50, 50);
        pix.fill(color);
        QListWidgetItem* item = new QListWidgetItem(QIcon(pix), arr.toHex() + " \"" + arr + "\"");
        item->setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsDragEnabled);
        item->setCheckState(Qt::Checked);
        ui->lvHex->addItem(item);
    }
} // highlighterAccepted

void DialogSetHighlight::highlighterDeclined()
{
    createNewHighlight = false;
}

void DialogSetHighlight::on_btOK_clicked()
{
    close();
}

void DialogSetHighlight::closeEvent(QCloseEvent* e)
{
    e->ignore();
    enabledHighlighters.clear();
    for (int i = 0; i < ui->lvHex->count(); i++) {
        if (ui->lvHex->item(i)->checkState() == Qt::Checked)
            enabledHighlighters.append(hexHighlighters.at(i));
    }

    QFile pFile(qApp->applicationDirPath() + "/highlighters.dat");
    if (pFile.open(QIODevice::WriteOnly)) {
        QDataStream PrefFile(&pFile);
        PrefFile << hexHighlighters;
        QList<QListWidgetItem> list;
        for (int i = 0; i < ui->lvHex->count(); i++)
            list.append(*ui->lvHex->item(i));
        PrefFile << list;
        pFile.close();
    }

    accept();
} // closeEvent

void DialogSetHighlight::on_btRemoveHex_clicked()
{
    int row = ui->lvHex->currentRow();
    int count = ui->lvHex->count();
    if (count == 0)
        return;

    if (row < 0) {
        ui->lvHex->takeItem(count - 1);
        hexHighlighters.removeAt(count - 1);
    }
    else {
        ui->lvHex->takeItem(row);
        hexHighlighters.removeAt(row);
    }
} // on_btRemoveHex_clicked
