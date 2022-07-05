#ifndef CYTONFFTW_H
#define CYTONFFTW_H

#include <QFile>
#include <QMutex>
#include <QQueue>
#include <QString>
#include <QThread>
#include <QWaitCondition>

#include "OpenBCI/AlgorithmSwitch.h"

namespace ehdu{

class CytonFftw: public QThread{
    Q_OBJECT
public:
    explicit CytonFftw(AlgorithmSwitch *as, QObject *parent = nullptr);
    ~CytonFftw();
    void recordInit();
    bool readingFlag = true;//终止进程的标志
    bool uiState;
    QString useName;
signals:
    void idleSignal();
    void leftSignal();
    void rightSignal();
protected:
    void run() override;
private:
    static const int queueSize;
    void motorImageryUseFftw();
    AlgorithmSwitch *chooseAlorithm = nullptr;
    QFile *csvFile = nullptr;
    QTextStream *in = nullptr;
    QQueue<ChannelSignal *> threadShareData;
    //定义互斥锁
    QMutex *bufferMutex = nullptr;
    //定义完成量
    QWaitCondition *bufferIsEmpty = nullptr;
    QWaitCondition *bufferIsFull = nullptr;
};

}

#endif
