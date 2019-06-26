#include "dialogsshpref.h"
#include "ui_dialogsshpref.h"
#include <QFileDialog>

DialogSSHPref::DialogSSHPref(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::DialogSSHPref)
{
    ui->setupUi(this);
}

DialogSSHPref::~DialogSSHPref()
{
    delete ui;
}

DialogSSHPref::SSHPref DialogSSHPref::getSshPref() const
{
    return sshPref;
}

void DialogSSHPref::setSshPref(const SSHPref& pref)
{
    sshPref = pref;
    ui->comboProtocol->setCurrentIndex(static_cast<int>(sshPref.protocol));
    ui->leProcName->setText(sshPref.processName);
    ui->leSSHLocalIP->setText(sshPref.localIP);
    ui->sbSSHLocalPort->setValue(sshPref.localPort);
    ui->comboAuth->setCurrentIndex(static_cast<int>(sshPref.params.authenticationType));
    ui->leUsername->setText(sshPref.params.userName);
    ui->lePass->setText(sshPref.params.password);
    ui->lePrivateKey->setText(sshPref.params.privateKeyFile);
}

void DialogSSHPref::on_btOK_clicked()
{
    sshPref.protocol = static_cast<ProtocolType>(ui->comboProtocol->currentIndex());
    sshPref.processName = ui->leProcName->text();
    sshPref.localIP = ui->leSSHLocalIP->text();
    sshPref.localPort = ui->sbSSHLocalPort->value();
    sshPref.params.authenticationType = static_cast<QSsh::SshConnectionParameters::AuthenticationType>(ui->comboAuth->currentIndex());
    sshPref.params.userName = ui->leUsername->text();
    sshPref.params.password = ui->lePass->text();
    sshPref.params.privateKeyFile = ui->lePrivateKey->text();
    sshPref.params.timeout = 5;
    accept();
}

void DialogSSHPref::on_DialogSSHPref_rejected()
{
    setSshPref(sshPref);
}

void DialogSSHPref::on_btBrowsePublicKey_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Get private key"), sshPref.params.privateKeyFile, tr("All Files (*.*)"));
    if (!fileName.isEmpty())
        ui->lePrivateKey->setText(fileName);
}
