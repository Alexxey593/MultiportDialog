#include "PortsListThread.h"
#include <QtSerialPort/QSerialPortInfo>

//QT_USE_NAMESPACE_SERIALPORT

PortsListThread::PortsListThread(QObject *parent) :
    QThread(parent), terminate(false)
{
}

void PortsListThread::stop()
{
    terminate = true;
}

void PortsListThread::run()
{
    terminate = false;
    while(!terminate) {
        QStringList portsList;
        foreach(const QSerialPortInfo & info, QSerialPortInfo::availablePorts()) {
            portsList << info.portName();
        }
        portsList.sort();
        if(portsList != ports) {
            ports = portsList;
            emit portList(ports);
        }
        msleep(300);
    }
}
