#ifndef DSI_MAIN_H
#define DSI_MAIN_H

#include <QObject>
#include "DSIAPI/DSI.h"
#include <QList>
#include <DSIAPI/dataform.h>
#include <QThread>
#include <QDebug>
#include <QTimer>
#include <QQueue>
#include <QMutex>
#include <QWaitCondition>
#include <vector>
#include <DSIAPI/alorithmswitch.h>
#include <QCoreApplication>

using namespace std;

class DSI_Main : public QThread
{
    Q_OBJECT
public:
    explicit DSI_Main(QObject *parent = nullptr,
                      char *port = "/dev/rfcomm1",
                      char *montage = "C3,C4",
                      int montageCount = 2,
                      char *reference = NULL,
                      int verbosity = 2,
                      int perbatch = 15,
                      double durationsec = -1,
                      double buffersec = 50.0 );
    ~DSI_Main();
    bool initLoadLib();
    int BufferingStart();
    int ImpedanceSlots();
    int Finish(DSI_Headset h);

public:
    bool readingFlag;//进程开启与关闭
    QList<QString> chooseChannlString;//获取数据的通道名称
    DSI_Headset DSIh;
    bool fftwSwitchFlag;//FFTW on or off
    bool recordSwitchFlag;//Record on or off

private:
    char *globalPort;//蓝牙串口
    char *globalMontage;//需要监测电极
    char *globalReference;//参考电极
    int globalVerbosity;//输出信息等级1-7
    int globalSamplesPerBatch;//获取的数据长度
    double globalDurationSec;//程序执行时间
    double globalBufferAheadSec;//缓冲数据时间,msec,除以1000后，以秒为单位
    int globalMontageCount;
    channelSignal chartSignalGet;//存储emit的通道数据
    QTimer *ImpTimer;//定时读取阻抗&其他参数

private:
    int StartUp(DSI_Headset * headsetOut);
    void run();
    int readDataSlots();
    QList<QString> settingChannel(QString data);
    void juiceDataStore(channelSignal *data, QString channelSignaldata, double channelSignalData);
    //void PrintImpedances( DSI_Headset h, double packetOffsetTime, void * userData);

private slots:
    void sendDataFixedTime();

public slots:
    void resetSlots();
    
signals:
    void bufferDataSignal(channelSignal);
    void fixedTimeSignal(channelImpedance);
    void offLineDataSignal(channelSignal);
};



class DSI_FFTW : public QThread
{
    Q_OBJECT

public:
    explicit DSI_FFTW(AlorithmSwitch* AS, QObject *parent = nullptr);
    ~DSI_FFTW();

public:
    bool readingFlag;//终止进程的标志
    bool uiState;
    QString useName;
    void recordInit();

private:
    void run();
    void motorImageryUseFFTW();
    AlorithmSwitch *chooseAlorithm;    
    QFile *csvFile;
    QTextStream *in;

signals:
    void C3LeftSignal();
    void C4RightSignal();
    void IdleSignal();
};

#endif // DSI_MAIN_H
