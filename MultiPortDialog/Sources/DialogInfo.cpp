#include "DialogInfo.h"
#include "ui_DialogInfo.h"

DialogInfo::DialogInfo(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogInfo)
{
    ui->setupUi(this);
}

DialogInfo::~DialogInfo()
{
    delete ui;
}

bool DialogInfo::setInfo(QStringList &info)
{
    if(info.size() != 6)
        return false;
    ui->lbPort->setText(tr("Port: %1", "Порт: %1").arg(info.at(0)));
    ui->lbDescription->setText(tr("Description: %1", "Описание: %1").arg(info.at(1)));
    ui->lbManufacturer->setText(tr("Manufacturer: %1", "Производитель: %1").arg(info.at(2)));
    ui->lbLocation->setText(tr("Path: %1", "Расположение: %1").arg(info.at(3)));
    ui->lbVid->setText(tr("Vendor ID: %1", "Vendor ID: %1").arg(info.at(4)));
    ui->lbPid->setText(tr("Product ID: %1", "Product ID: %1").arg(info.at(5)));
    return true;
}
