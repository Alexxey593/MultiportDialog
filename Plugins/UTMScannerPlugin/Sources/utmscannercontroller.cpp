#include "utmscannercontroller.h"

void UTMScannerController::parseResponse(QByteArray &data)
{
    QList<QByteArray> commands = data.split('\n');
    if(commands.count() < 2)
    {
        emit info(tr("Wrong command format: fields num %1", "Неверный формат команды: количество полей %1").arg(commands.count()));
        return;
    }
    else
    {
        QByteArray commandCode = commands.at(0).left(2);
        CommandType type = commandToCommandType(commandCode);
        if(type != ctMeasureLongCont && type != ctMeasureShortCont)
            emit info(tr("Command: \"%1\" (%2)").arg(commandTypeToString(type)).arg(QString(commandCode)));
        if(commands.at(1).size() != 3)
        {
            emit info(tr("Wrong status format:  %1", "Неверный формат статуса:  %1").arg(QString(commands.at(1))));
            return;
        }
        else
        {
            QByteArray status = commands.at(1).left(2);
            if(type != ctMeasureLongCont && type != ctMeasureShortCont)
                emit info(tr("  Status: \"%1\" (%2)").arg(statusToString(status)).arg(QString(status)));
        }

        if(type == ctMeasureLong || type == ctMeasureShort || type == ctMeasureLongCont || type == ctMeasureShortCont)
        {
            if(commands.count() < 3)
            {
                emit info(tr("  Wrong fields number for this command"));
            }
            else
            {
                QByteArray time = commands.at(2);
                QByteArray scanData;
                for(int i = 3; i < commands.count(); i++)
                {
                    QByteArray dataBytes = commands.at(i);
                    dataBytes.chop(1);
                    scanData.append(dataBytes);
                }
                CEDType ced = ced3;
                if(type == ctMeasureShort || type == ctMeasureShortCont)
                    ced = ced2;
                emit scanLine(decode6BitCode(scanData, ced));
            }
        }
        else // Просто разбираем данные
        {
            int dataCount = 0;
            for(int i = 2; i < commands.count(); i++)
            {
                //            char CC = commands.at(i).right(1).at(0);
                QByteArray dataBytes = commands.at(i);
                dataBytes.chop(1);
                emit info(tr("  Data %1: %2").arg(++dataCount).arg(QString(dataBytes)));
            }
        }
    }
}

QList<qint32> UTMScannerController::decode6BitCode(QByteArray code, CEDType ced)
{
    for(int i = 0; i < code.size(); i++)
    {
        char c = code[i];
        c -= 0x30;
        code[i] = c;
    }
    int bytesPerNum = static_cast<int>(ced);
    QList<qint32> values;
    if(code.size() % bytesPerNum)
    {
        emit info(tr("Codesize %1 not multiply by %2").arg(code.size()).arg(bytesPerNum));
        return values;
    }
    for(int i = 0; i <= code.size() - bytesPerNum; i += bytesPerNum)
    {
        qint32 codedVal = 0;
        for(int j = 0; j < bytesPerNum; j++)
        {
            codedVal += qint32(code.at(i+j)) << (bytesPerNum - 1 - j)*6;
        }
        //        qint32 val = 0;
        //        for(int j = 0; j < bytesPerNum; j++)
        //        {
        //            qint32 bits = (codedVal & (0x3f << j)) << 2*j;
        //            val += bits;
        //        }
        values.append(codedVal);
    }
    return values;
}

UTMScannerController::UTMScannerController(QObject *parent) :
    QObject(parent)
{
}

void UTMScannerController::sendCommandToSensor(UTMScannerController::CommandType type, QByteArray parameters, QByteArray userString)
{
    QByteArray command;
    switch (type) {
    case ctInfoVersion:
        command = "VV";
        break;
    case ctInfoParameters:
        command = "PP";
        break;
    case ctInfoState:
        command = "II";
        break;
    case ctToMeasurementState:
        command = "BM";
        break;
    case ctToStandbyState:
        command = "QT";
        break;
    case ctReset:
        command = "RS";
        break;
    case ctPartialReset:
        command = "RT";
        break;
    case ctReboot:
        command = "RB";
        break;
    case ctMeasureLong:
        command = "GD";
        break;
    case ctMeasureShort:
        command = "GS";
        break;
    case ctMeasureLongCont:
        command = "MD";
        break;
    case ctMeasureShortCont:
        command = "MS";
        break;

    default:
        break;
    }
    command += parameters;
    if(!userString.isEmpty())
    {
        if(userString.length() > 16)
            userString.truncate(16);
        command += ';';
        command += userString;
    }
    command += "\n";
    emit dataToSensor(command);
}

QString UTMScannerController::commandTypeToString(UTMScannerController::CommandType type)
{
    switch (type) {
    case ctWrongFormat:
        return tr("Wrong command format");
    case ctInfoVersion:
        return tr("Version");
    case ctInfoParameters:
        return tr("Sensor parameters", "Параметры сенсора");
    case ctInfoState:
        return tr("Sensor state", "Состояние сенсора");
    case ctToMeasurementState:
        return tr("Transition to measurement state");
    case ctToStandbyState:
        return tr("Transition to standby state");
    case ctReset:
        return tr("Reset");
    case ctPartialReset:
        return tr("Partial reset");
    case ctReboot:
        return tr("Reboot");
    case ctMeasureLong:
        return tr("Distance measurement (long)");
    case ctMeasureShort:
        return tr("Distance measurement (short)");
    case ctMeasureLongCont:
        return tr("Continuous distance measurement (long)");
    case ctMeasureShortCont:
        return tr("Continuous distance measurement (short)");
    default:
        return tr("Unknown command", "Неизвестная команда");
    }
}

UTMScannerController::CommandType UTMScannerController::commandToCommandType(QByteArray commandCode)
{
    if(commandCode.size() != 2)
        return ctWrongFormat;
    if(commandCode == "VV")
        return ctInfoVersion;
    else if(commandCode == "PP")
        return ctInfoParameters;
    else if(commandCode == "II")
        return ctInfoState;
    else if(commandCode == "BM")
        return ctToMeasurementState;
    else if(commandCode == "QT")
        return ctToStandbyState;
    else if(commandCode == "RS")
        return ctReset;
    else if(commandCode == "RT")
        return ctPartialReset;
    else if(commandCode == "RB")
        return ctReboot;
    else if(commandCode == "GD")
        return ctMeasureLong;
    else if(commandCode == "GS")
        return ctMeasureShort;
    else if(commandCode == "MD")
        return ctMeasureLongCont;
    else if(commandCode == "MS")
        return ctMeasureShortCont;
    else
        return ctUnknown;
}

QString UTMScannerController::statusToString(QByteArray status)
{
    if(status.size() != 2)
        return tr("Unknown status", "Неизвестный статус");
    if(status == "00")
        return tr("Command accepted", "Команда принята");
    else if(status == "0L")
        return tr("Sensor is in abnormal condition state", "Сенсор в нерабочем состоянии");
    else if(status == "0M")
        return tr("Sensor is in the unstable state", "Сенсор в нестабильном состоянии");
    else if(status == "0E")
        return tr("Unknown command", "Неизвестная команда");
    else if(status == "0F")
        return tr("Command is not supported in the current sensor", "Команда не поддерживается этой моделью сенсора");
    else if(status == "10")
        return tr("Command cannot be received in the current sensor state", "В текущем состояниии сенсор не принимает команды данного типа");
    else if(status == "0G")
        return tr("User defined string is too long", "Пользовательская строчка слишком длинная");
    else if(status == "0H")
        return tr("User defined string has a problem", "Ошибка пользовательской строки");
    else if(status == "0C")
        return tr("The length of request message is shorter than expected", "Сообщение запроса слишком короткое");
    else if(status == "0D")
        return tr("The length of request message is longer than expected", "Сообщение запроса слишком длинное");
    else if(status == "01" || status == "02" || status == "03" || status == "04" || status == "05" || status == "06" || status == "07")
        return tr("One of the parameters specified in the request message has a problem", "Один из параметров сообщения запроса не верен");
    else if(status == "99")
        return tr("Continuous scan is OK");
    else
        return tr("Unknown status", "Неизвестный статус");
}

void UTMScannerController::dataFromSensor(QByteArray &data)
{
    data.prepend(buffer);
    buffer.clear();
    int packagesnum = data.count("\n\n");
    if(packagesnum == 0)
    {
        buffer.append(data);
        return;
    }
    else if(packagesnum > 1)
    {
        int count = data.indexOf("\n\n");
        QByteArray response = data.left(count);
        parseResponse(response);
        data = data.right(data.length() - count - 2);
        dataFromSensor(data);
    }
    else if(packagesnum == 1)
    {
        if(data.endsWith("\n\n"))
        {
            data.chop(2);
            parseResponse(data);
        }
        else
        {
            int count = data.indexOf("\n\n");
            QByteArray response = data.left(count);
            parseResponse(response);
            data = data.right(data.length() - count - 2);
            buffer.append(data);
        }
    }
}
