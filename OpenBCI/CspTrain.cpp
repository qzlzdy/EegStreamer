#include "OpenBCI/CspTrain.h"

using namespace ehdu;

CspTrain::CspTrain(QObject *parent): QThread(parent){
    _W1.resize(126);
    _W2.resize(126);
    _W3.resize(126);
    _sample1.resize(180);
    _sample2.resize(180);
    _sample3.resize(180);
    csvfile = new QFile;
}

CspTrain::~CspTrain(){
    if(csvfile->isOpen()){
        csvfile->close();
    }
    delete csvfile;
}

void CspTrain::step(QString &csvFilename){
    //采集数据，每个类别30秒，300采样率，21通道，共18900点
    //其实可以调用另外一个信号，也就是保存数据的！！！
    //这样就在训练的时候不需要开启FFTW的那个线程，直接接受信号保存，3步3个类别数据，最后一下button的click进行训练！！！
    //直接保存三个文件，读取训练就行
    //文件格式：csvFileName_left.csv  csvFileName_right.csv  csvFileName_idle.csv

    QString csvFilePath_left = QString("%1/train/%2_left.csv").arg(
        QCoreApplication::applicationDirPath(), csvFilename);
    QString csvFilePath_right = QString("%1/train/%2_right.csv").arg(
        QCoreApplication::applicationDirPath(), csvFilename);
    QString csvFilePath_idle = QString("%1/train/%2_idle.csv").arg(
        QCoreApplication::applicationDirPath(), csvFilename);
    if(!(QFile::exists(csvFilePath_left) && QFile::exists(csvFilePath_right)
       && QFile::exists(csvFilePath_idle))){
        emit finishTrain(false);
        return;
    }

    //打开文件left
    csvfile->setFileName(csvFilePath_left);
    if(!csvfile->open(QIODevice::ReadOnly | QIODevice::Text)){
        emit finishTrain(false);
        return;
    }
    int i1 = 0;
    QTextStream inStream_1(csvfile);
    while(!inStream_1.atEnd()){
        QStringList tmp = inStream_1.readLine().split(","); //保存到List当中
        for(QString &str: tmp){
            dataLeftRaw[i1++] = str.toDouble();
        }
    }
    csvfile->close();

    //打开文件right
    csvfile->setFileName(csvFilePath_right);
    if(!csvfile->open(QIODevice::ReadOnly | QIODevice::Text)){
        emit finishTrain(false);
        return;
    }
    int i2 = 0;
    QTextStream inStream_2(csvfile);
    while(!inStream_2.atEnd()){
        QStringList tmp = inStream_2.readLine().split(","); //保存到List当中
        for(QString &str: tmp){
            dataRightRaw[i2++] = str.toDouble();
        }
    }
    csvfile->close();

    //打开文件idle
    csvfile->setFileName(csvFilePath_idle);
    if(!csvfile->open(QIODevice::ReadOnly | QIODevice::Text)){
        emit finishTrain(false);
        return;
    }
    int i3 = 0;
    QTextStream inStream_3(csvfile);
    while(!inStream_3.atEnd()){
        QStringList tmp = inStream_3.readLine().split(","); //保存到List当中
        for(QString &str: tmp){
            dataIdleRaw[i3++] = str.toDouble();
        }
    }
    csvfile->close();

    //CSP训练,内部内置滤波器
    csp_train_ovo(dataLeftRaw, dataRightRaw, dataIdleRaw, _W1, _sample1, _W2, _sample2, _W3, _sample3);
    emit finishTrain(true);
}

void CspTrain::run(){
    step(filename);
}
