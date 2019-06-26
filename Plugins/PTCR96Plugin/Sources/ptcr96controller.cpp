#include "ptcr96controller.h"

void PTCR96Controller::parseResponse(QByteArray &data)
{
// emit info(data.toHex());
    if (data.length() < 5) {
        emit info(tr("Command too short."));
        return;
    }
    if (!data.startsWith(0x06) && !data.startsWith(0x15)) {
        emit info(tr("Wrong start byte."));
        return;
    }
    if (!data.endsWith(0x03)) {
        emit info(tr("Wrong end byte."));
        return;
    }
    if (data.startsWith(0x15)) {
        emit info(tr("Not Acknowledge"));
        return;
    }
    QByteArray processedData = showControllChars(data.mid(1, data.length() - 2));
// emit info(processedData.toHex());
    char crc = processedData.at(processedData.length() - 1);
    processedData.chop(1);
    char crc2 = getCRC(processedData);
    if ((crc ^ crc2) != 0) {
        QByteArray h1, h2;
        h1.append(crc);
        h2.append(crc2);
// emit info(tr("Wrong CRC"));
// emit info("crc: " + h1.toHex() + ":" + h2.toHex());
        return;
    }
    char cmd = processedData.at(1);
    if (cmd == 0x31) { // Status
        if (processedData.size() < 11)
            return;
        uchar statByte = processedData.at(10);
        bool isDest = statByte & 0x20;
        if (isDest)
            return;
        m_isMovingPan = ((statByte & 0x08) != 0) || ((statByte & 0x04) != 0);
        m_isMovingTilt = ((statByte & 0x02) != 0) || ((statByte & 0x01) != 0);
        bool _busy = statByte & 0x40;
        if (m_busy != _busy) {
            m_busy = _busy;
            emit busy(m_busy);
        }

        QByteArray arrPan = processedData.mid(2, 3);
        int iPan = 0;
        uchar *cPan = (uchar *)(&iPan);
        cPan[0] = arrPan.at(0);
        cPan[1] = arrPan.at(1);
        cPan[2] = arrPan.at(2);
        if (cPan[2] & 0x7F)
            cPan[3] = 0xFF;
        double _pan = double(iPan) / 100.0;
        if (m_pan != _pan) {
            m_pan = _pan;
            emit panChanged(m_pan);
        }
        QByteArray arrTilt = processedData.mid(5, 3);
        int iTilt = 0;
        uchar *cTilt = (uchar *)(&iTilt);
        cTilt[0] = arrTilt.at(0);
        cTilt[1] = arrTilt.at(1);
        cTilt[2] = arrTilt.at(2);
        if (cTilt[2] & 0x7F)
            cTilt[3] = 0xFF;
        double _tilt = double(iTilt) / 100.0;
        if (m_tilt != _tilt) {
            m_tilt = _tilt;
            emit tiltChanged(m_tilt);
        }

        uchar panStat = processedData.at(8);
        ErrorsTypes err = errNoError;
        if (panStat & 0x08)
            err |= errPanTO;
        if (panStat & 0x04)
            err |= errPanDE;
        uchar tiltStat = processedData.at(9);
        if (tiltStat & 0x08)
            err |= errTiltTO;
        if (tiltStat & 0x04)
            err |= errTiltDE;
        if (m_err != err) {
            m_err = err;
            emit error(err);
        }
    }
    if(cmd == 0xA1) {
        emit info(processedData.toHex());
    }
} // parseResponse

char PTCR96Controller::getCRC(QByteArray &arr)
{
    char c = 0;
    for (int i = 0; i < arr.size(); i++)
        c ^= arr.at(i);
    return c;
}

QByteArray PTCR96Controller::hideControllChars(QByteArray &arr)
{
    QByteArray res;
    for (int i = 0; i < arr.size(); i++) {
        char c = arr.at(i);
        if ((c == 0x02) || (c == 0x03) || (c == 0x06) || (c == 0x015) || (c == 0x1B)) {
            res.append(0x1B);
            c |= 0x80;
        }
        res.append(c);
    }
    return res;
} // hideControllChars

QByteArray PTCR96Controller::showControllChars(QByteArray &arr)
{
    QByteArray res;
    for (int i = 0; i < arr.size(); i++) {
        char c = arr.at(i);
        if ((i < (arr.size() - 1)) && (c == 0x1B)) {
            c = arr.at(i + 1);
            c &= 0x7F;
            i++;
        }
        res.append(c);
    }
    return res;
} // showControllChars

PTCR96Controller::PTCR96Controller(QObject *parent) :
    QObject(parent), m_pan(0.0), m_tilt(0.0), m_panSpeed(0), m_tiltSpeed(0), m_statCmd(0), m_isMovingPan(false), m_isMovingTilt(false),
    destPan(0), destTilt(0), m_slowPan(false), m_slowTilt(false), m_busy(false)
{
    statusCommand = QByteArray("\x02\x00\x31\x00\x00\x00\x00\x00\x00\x00\x31\x03", 13);
    statusData = QByteArray("\x00\x00\x00\x00\x00\x00\x00", 7);
    connect(&timer, SIGNAL(timeout()), SLOT(getStatus()));
}

bool PTCR96Controller::isMovingPan() const
{
    return m_isMovingPan;
}

bool PTCR96Controller::isMovingTilt() const
{
    return m_isMovingTilt;
}

void PTCR96Controller::dataFromOPU(QByteArray &data)
{
    static bool packageStarted = false;
    char cReceivedChar;
    for (int i = 0; i < data.size(); i++) {
        cReceivedChar = (char)data[i];
        if ((cReceivedChar == 0x03) && packageStarted) {
            buffer += cReceivedChar;
            parseResponse(buffer);
            buffer.clear();
            packageStarted = false;
        } else {
            if ((cReceivedChar == 0x06) || (cReceivedChar == 0x15))
                packageStarted = true;
            if (packageStarted)
                buffer += cReceivedChar;
            if (buffer.size() > 300) {
                buffer.clear();
                packageStarted = false;
            }
        }
    }
} // dataFromOPU

void PTCR96Controller::getStatus()
{
    emit dataToOPU(statusCommand);
}

void PTCR96Controller::start()
{
    timer.start(200);
}

void PTCR96Controller::stop()
{
    timer.stop();
}

void PTCR96Controller::setPanSpeed(int speed)
{
    speed = qBound(-255, speed, 255);
    bool changed = false;
    bool cw = speed >= 0;
    if (m_statCmd & 0x80)
        if (!cw)
            changed = true;
    quint8 panByte = qAbs(speed);
    if (panByte != m_panSpeed) {
        m_panSpeed = panByte;
        changed = true;
    }
    if (changed) {
        stop();
        if (cw)
            m_statCmd |= 0x80;
        else
            m_statCmd &= 0x7F;
        statusData[0] = m_statCmd;
        statusData[1] = m_panSpeed;
        QByteArray command = QByteArray("\x00\x31", 2);
        command.append(statusData);
        command.append(getCRC(command));
        statusCommand = '\x02' + hideControllChars(command) + '\x03';
        getStatus();
        start();
    }
} // setPanSpeed

void PTCR96Controller::setTiltSpeed(int speed)
{
    speed = qBound(-255, speed, 255);
    bool changed = false;
    bool up = speed >= 0;
    if (m_statCmd & 0x40)
        if (!up)
            changed = true;
    quint8 tiltByte = qAbs(speed);
    if (tiltByte != m_tiltSpeed) {
        m_tiltSpeed = tiltByte;
        changed = true;
    }
    if (changed) {
        stop();
        if (up)
            m_statCmd |= 0x40;
        else
            m_statCmd &= 0xBF;
        statusData[0] = m_statCmd;
        statusData[2] = m_tiltSpeed;
        QByteArray command = QByteArray("\x00\x31", 2);
        command.append(statusData);
        command.append(getCRC(command));
        statusCommand = '\x02' + hideControllChars(command) + '\x03';
        getStatus();
        start();
    }
} // setTiltSpeed

void PTCR96Controller::setSlowSpeed(bool slow)
{
    if ((m_slowPan != slow) || (m_slowTilt != slow)) {
        stop();
        m_slowPan = slow;
        m_slowTilt = slow;
        if (slow)
            m_statCmd |= 0x18;
        else
            m_statCmd &= 0xE7;
        QByteArray command = QByteArray("\x00\x31", 2);
        command.append(statusData);
        command.append(getCRC(command));
        statusCommand = '\x02' + hideControllChars(command) + '\x03';
        getStatus();
        start();
    }
} // setSlowSpeed

void PTCR96Controller::setSlowPan(bool slow)
{
    if (m_slowPan != slow) {
        stop();
        m_slowPan = slow;
        if (m_slowPan)
            m_statCmd |= 0x10;
        else
            m_statCmd &= 0xEF;
        QByteArray command = QByteArray("\x00\x31", 2);
        command.append(statusData);
        command.append(getCRC(command));
        statusCommand = '\x02' + hideControllChars(command) + '\x03';
        getStatus();
        start();
    }
} // setSlowPan

void PTCR96Controller::setSlowTilt(bool slow)
{
    if (m_slowTilt != slow) {
        stop();
        m_slowTilt = slow;
        if (m_slowTilt)
            m_statCmd |= 0x08;
        else
            m_statCmd &= 0xF7;
        QByteArray command = QByteArray("\x00\x31", 2);
        command.append(statusData);
        command.append(getCRC(command));
        statusCommand = '\x02' + hideControllChars(command) + '\x03';
        getStatus();
        start();
    }
} // setTiltSpeed

void PTCR96Controller::moveToCoords(double _pan, double _tilt)
{
    stop();
    int panCoord = _pan * 100;
    if (panCoord != 99999) {
        panCoord = qBound(-36000, panCoord, 36000);
        while (panCoord < -18000)
            panCoord += 36000;
        while (panCoord > 18000)
            panCoord -= 36000;
        destPan = panCoord;
    } else {
        if (m_isMovingPan)
            panCoord = destPan;
    }
    int tiltCoord = _tilt * 100;
    if (tiltCoord != 99999) {
        tiltCoord = qBound(-18000, tiltCoord, 18000);
        destTilt = tiltCoord;
    } else {
        if (m_isMovingTilt)
            tiltCoord = destTilt;
    }
    QByteArray command = QByteArray("\x00\x33", 2);
    uchar *cPan = (uchar *)(&panCoord);
    command.append(cPan[0]);
    command.append(cPan[1]);
    command.append(cPan[2]);
    uchar *cTilt = (uchar *)(&tiltCoord);
    command.append(cTilt[0]);
    command.append(cTilt[1]);
    command.append(cTilt[2]);
    command.append(getCRC(command));
    command = hideControllChars(command);
    command.prepend(0x02);
    command.append(0x03);
    emit dataToOPU(command);
    start();
} // moveToCoords

void PTCR96Controller::moveToDelta(double _pan, double _tilt)
{
    stop();
    int panCoord = _pan * 100;
    panCoord = qBound(-36000, panCoord, 36000);
    int tiltCoord = _tilt * 100;
    tiltCoord = qBound(-18000, tiltCoord, 18000);
    QByteArray command = QByteArray("\x00\x34", 2);
    uchar *cPan = (uchar *)(&panCoord);
    command.append(cPan[0]);
    command.append(cPan[1]);
    command.append(cPan[2]);
    uchar *cTilt = (uchar *)(&tiltCoord);
    command.append(cTilt[0]);
    command.append(cTilt[1]);
    command.append(cTilt[2]);
    command.append(getCRC(command));
    command = hideControllChars(command);
    command.prepend(0x02);
    command.append(0x03);
    emit dataToOPU(command);
    start();
} // moveToDelta

void PTCR96Controller::moveToZero()
{
    stop();
    QByteArray command = QByteArray("\x00\x35", 2);
    command.append(getCRC(command));
    command = hideControllChars(command);
    command.prepend(0x02);
    command.append(0x03);
    emit dataToOPU(command);
    start();
}

void PTCR96Controller::moveToHome()
{
    stop();
    QByteArray command = QByteArray("\x00\x36", 2);
    command.append(getCRC(command));
    command = hideControllChars(command);
    command.prepend(0x02);
    command.append(0x03);
    emit dataToOPU(command);
    start();
}

void PTCR96Controller::reset()
{
    stop();
    QByteArray command = QByteArray::fromHex(QByteArray("020031410000000000007003", 24));
    emit dataToOPU(command);
    start();
}

void PTCR96Controller::stopOpu()
{
    stop();
    m_panSpeed = 0;
    m_tiltSpeed = 0;
    m_statCmd |= 0x02;
    statusData[0] = m_statCmd;
    statusData[1] = 0;
    statusData[2] = 0;
    QByteArray command = QByteArray("\x00\x31", 2);
    command.append(statusData);
    command.append(getCRC(command));
    statusCommand = '\x02' + hideControllChars(command) + '\x03';
    getStatus();
    m_statCmd &= 0xFD;
    statusData[0] = m_statCmd;
    start();
}

void PTCR96Controller::setMinMaxPanFreq(int _minFreq, int _maxFreq)
{
    stop();
    quint16 minFreq = qBound(15, _minFreq, 30000);
    quint16 maxFreq = qBound(15, _maxFreq, 30000);
    if(maxFreq < minFreq)
        return;
    QByteArray command = QByteArray("\x00\xA1\x01", 3);
    uchar *cMinFreq = (uchar *)(&minFreq);
    command.append(cMinFreq[0]);
    command.append(cMinFreq[1]);
    uchar *cMaxFreq = (uchar *)(&maxFreq);
    command.append(cMaxFreq[0]);
    command.append(cMaxFreq[1]);
    command.append(getCRC(command));
    command = hideControllChars(command);
    command.prepend(0x02);
    command.append(0x03);
    emit dataToOPU(command);
    emit info(command.toHex());
    start();
}

void PTCR96Controller::setMinMaxTiltFreq(int _minFreq, int _maxFreq)
{
    stop();
    quint16 minFreq = qBound(15, _minFreq, 30000);
    quint16 maxFreq = qBound(15, _maxFreq, 30000);
    if(maxFreq < minFreq)
        return;
    QByteArray command = QByteArray("\x00\xA2\x00", 3);
    uchar *cMinFreq = (uchar *)(&minFreq);
    command.append(cMinFreq[0]);
    command.append(cMinFreq[1]);
    uchar *cMaxFreq = (uchar *)(&maxFreq);
    command.append(cMaxFreq[0]);
    command.append(cMaxFreq[1]);
    command.append(getCRC(command));
    command = hideControllChars(command);
    command.prepend(0x02);
    command.append(0x03);
    emit dataToOPU(command);
    emit info(command.toHex());
    start();
} // stopOpu
