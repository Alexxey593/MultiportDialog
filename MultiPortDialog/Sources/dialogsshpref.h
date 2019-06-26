#ifndef DIALOGSSHPREF_H
#define DIALOGSSHPREF_H

#include <QDialog>
#include "ssh/sshconnection.h"

namespace Ui {
class DialogSSHPref;
}

class DialogSSHPref : public QDialog {
    Q_OBJECT

public:
    enum ProtocolType {
        RemoteShell,
        RemoteProcess,
        Tunnel,
        SFTP
    };

    struct SSHPref {
        ProtocolType protocol;
        QString localIP;
        int localPort;
        QString processName;
        QSsh::SshConnectionParameters params;

        SSHPref()
        {
            protocol = RemoteShell;
            localIP = "127.0.0.1";
            localPort = 22;
            processName = "exec";
            params.timeout = 5;
            params.authenticationType = QSsh::SshConnectionParameters::AuthenticationTypePassword;
        }
    };

public:
    explicit DialogSSHPref(QWidget* parent = 0);
    ~DialogSSHPref();

    SSHPref getSshPref() const;
    void setSshPref(const SSHPref& pref);

private slots:
    void on_btOK_clicked();

    void on_DialogSSHPref_rejected();

    void on_btBrowsePublicKey_clicked();

private:
    Ui::DialogSSHPref* ui;
    SSHPref sshPref;
};

#endif // DIALOGSSHPREF_H
