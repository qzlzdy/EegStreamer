#include "OpenBCI/CytonFftw.h"

#include <QCoreApplication>
#include "OpenBCI/Cyton.h"

using namespace ehdu;

CytonFftw::CytonFftw(AlgorithmSwitch *as, QObject *parent): QThread(parent),
chooseAlorithm(as){

}

CytonFftw::~CytonFftw(){
    //终止线程，关闭
    readingFlag = false;
    SignalBuffer::empty.wakeAll();
    quit();
    wait();
    if(csvFile->isOpen()){
        csvFile->close();
    }
    delete csvFile;
    delete in;
    delete chooseAlorithm;
}

void CytonFftw::recordInit(){
    //判断文件是否存在
    int index = 0;
    csvFile = new QFile();
    QString csvFilePath;
    while(true){
        csvFilePath = QString("%1/train/%2_record_%3.csv").arg(
            QCoreApplication::applicationDirPath(), useName,
            QString::number(index));
        if(QFile::exists(csvFilePath)){
            ++index;
        }
        else{
            break;
        }
        if(index == 10){
            break;
        }
    }
    //创建文件并打开
    csvFile->setFileName(csvFilePath);
    if(!csvFile->open(QIODevice::WriteOnly)){
        return;
    }

    //定义输出流
    in = new QTextStream(csvFile);

    //记录表头
    *in << "Fp1,Fp2,C3,C4,P7,P8,O1,O2\n";
}

//线程运行
void CytonFftw::run(){
    while(readingFlag){
        motorImageryUseFftw();
    }
}

void CytonFftw::motorImageryUseFftw(){
    /*线程之间 传递数据*/
    SignalBuffer::mutex.lock();
    //若缓存区为空,交出锁,进入休眠等待
    if(SignalBuffer::sharedData.isEmpty()){
        SignalBuffer::empty.wait(&SignalBuffer::mutex);
    }
    //若线程结束,强制返回，并不进行睡眠
    if(readingFlag == false){
        SignalBuffer::full.wakeAll();
        SignalBuffer::mutex.unlock();
        return;
    }

    //从缓存区拿出数据,1/300时间窗，想怎么使用可根据接口自定义
    ChannelSignal *tmpSignalDataAdress = SignalBuffer::sharedData.dequeue();
    SignalBuffer::full.wakeAll();
    SignalBuffer::mutex.unlock();

    //记录数据
    *in << (*tmpSignalDataAdress).C3.first << ",";
    *in << (*tmpSignalDataAdress).C4.first << ",";
    *in << (*tmpSignalDataAdress).Fp1.first<< ",";
    *in << (*tmpSignalDataAdress).Fp2.first<< ",";
    *in << (*tmpSignalDataAdress).P7.first<< ",";
    *in << (*tmpSignalDataAdress).P8.first<< ",";
    *in << (*tmpSignalDataAdress).O1.first << ",";
    *in << (*tmpSignalDataAdress).O2.first << "\n";

    tmpSignalDataAdress->MI_SSVEP = uiState;
    //Template Method 算法调用统一接口运算
    int result = chooseAlorithm->step(*tmpSignalDataAdress);

    //决策
    if(result == 1){
        if(tmpSignalDataAdress->MI_SSVEP){
            *in << "\nMI Left\n\n";
        }
        else{
            *in << "\nSSVEP Left\n\n";
        }
        emit leftSignal();
    }
    else if(result == 2){
        if(tmpSignalDataAdress->MI_SSVEP){
            *in << "\nMI Right\n\n";
        }
        else{
            *in << "\nSSVEP Right\n\n";
        }
        emit rightSignal();
    }
    else if(result == 0){
        if(tmpSignalDataAdress->MI_SSVEP){
            *in << "\nMI Idle\n\n";
        }
        else{
            *in << "\nSSVEP Idle\n\n";
        }
        emit idleSignal();
    }
    else if(result == -1){
        qDebug() << "juice error! or juicing!";
    }
    delete tmpSignalDataAdress;
}
