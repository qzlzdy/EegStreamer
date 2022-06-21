#ifndef REHABILITATIVEUSART_H
#define REHABILITATIVEUSART_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QVector>
#include <QDebug>
#include <QTime>

class rehabilitativeUsart : public QObject
{
    Q_OBJECT

public:
    rehabilitativeUsart();
    ~rehabilitativeUsart();

public slots:
    //屈——0xbe 伸——0x5f
    void flexSend();
    void extenSend();
    void findUsingPort();
    void connectComPort(QString com);
    void closePort();
    void sendToBend(const int &);


private:
    QSerialPort *comPort;
    void Delay_MSec_Suspend(int msec);

signals:
    void usingComPortSignal(QVector<QString> );
};

#endif // REHABILITATIVEUSART_H
