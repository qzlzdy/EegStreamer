#include "rehabilitativeusart.h"

rehabilitativeUsart::rehabilitativeUsart()
{
    comPort = new QSerialPort;
}

rehabilitativeUsart::~rehabilitativeUsart()
{
    comPort->deleteLater();
}

void rehabilitativeUsart::findUsingPort()
{
    //查找可用的串口
    QVector<QString> usingComPortNow;
    foreach (const QSerialPortInfo &info,QSerialPortInfo::availablePorts())
    {
        QSerialPort serial;
        serial.setPort(info);
        if(serial.open(QIODevice::ReadWrite))
        {
            usingComPortNow << serial.portName();
        }
    }
    emit usingComPortSignal(usingComPortNow);
}

void rehabilitativeUsart::connectComPort(QString com)
{
    //com init
    comPort->setPortName(com);
    if(comPort->open(QIODevice::ReadWrite)) //以读写方式打开串口
    {
        comPort->setBaudRate(QSerialPort::Baud9600); //波特率设置，设置为115200
        comPort->setDataBits(QSerialPort::Data8); //数据位设置，设置为8位数据位
        comPort->setParity(QSerialPort::OddParity); //奇偶校验设置，设置为无校验
        comPort->setStopBits(QSerialPort::OneStop); //停止位设置，设置为1位停止位
        comPort->setFlowControl(QSerialPort::NoFlowControl); //数据流控制设置，设置为无数据流控制
        qDebug()<<"open "<<com<<" successful!";
    }
}

void rehabilitativeUsart::closePort()
{
    comPort->clear();
    comPort->close();
}
//屈——0xbe
void rehabilitativeUsart::flexSend()
{
    QString str;
    str.append(QChar(0xbe));
    comPort->write(str.toLatin1());
    Delay_MSec_Suspend(100);
    comPort->write(str.toLatin1());
    Delay_MSec_Suspend(100);
    comPort->write(str.toLatin1());
    Delay_MSec_Suspend(100);
    comPort->write(str.toLatin1());
}
//伸——0x5f
void rehabilitativeUsart::extenSend()
{
    QString str;
    str.append(QChar(0x5f));
    comPort->write(str.toLatin1());
    Delay_MSec_Suspend(100);
    comPort->write(str.toLatin1());
    Delay_MSec_Suspend(100);
    comPort->write(str.toLatin1());
    Delay_MSec_Suspend(100);
    comPort->write(str.toLatin1());
}

void rehabilitativeUsart::sendToBend(const int& data)
{
    QString str;
    str.append(QChar(0xff));
    str.append(QChar(0x01));
    str.append(QChar(data));
    str.append(QChar(0xff^0x01^data));
    comPort->write(str.toLatin1());
}

void rehabilitativeUsart::Delay_MSec_Suspend(int msec)
{
    QTime _Timer = QTime::currentTime();
    QTime _NowTimer;
    do{
        _NowTimer=QTime::currentTime();
    }while (_Timer.msecsTo(_NowTimer)<=msec);
}
