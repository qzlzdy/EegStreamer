#include "csp_train.h"


CSP_Train::CSP_Train(QObject *parent) : QThread(parent)
{
    _W1.resize(126);
    _W2.resize(126);
    _W3.resize(126);
    _sample1.resize(180);
    _sample2.resize(180);
    _sample3.resize(180);
    csvFile = new QFile;
}

CSP_Train::~CSP_Train()
{
    if(csvFile->isOpen())
        csvFile->close();
    delete csvFile;
}

int CSP_Train::operationStep(QString &csvFileName)
{
    //采集数据，每个类别30秒，300采样率，21通道，共18900点
    //其实可以调用另外一个信号，也就是保存数据的！！！
    //这样就在训练的时候不需要开启FFTW的那个线程，直接接受信号保存，3步3个类别数据，最后一下button的click进行训练！！！
    //直接保存三个文件，读取训练就行
    //文件格式：csvFileName_left.csv  csvFileName_right.csv  csvFileName_idle.csv

    QString csvFilePath_left = QString("%1/train/%2_left.csv").arg(QCoreApplication::applicationDirPath()).arg(csvFileName);
    QString csvFilePath_right = QString("%1/train/%2_right.csv").arg(QCoreApplication::applicationDirPath()).arg(csvFileName);
    QString csvFilePath_idle = QString("%1/train/%2_idle.csv").arg(QCoreApplication::applicationDirPath()).arg(csvFileName);
    if(!(QFile::exists(csvFilePath_left) && QFile::exists(csvFilePath_right) && QFile::exists(csvFilePath_idle)))
    {
        emit finish_train(0);
        return -1;
    }

//    vector<double> data_left_raw, data_right_raw, data_idle_raw;
//    data_right_raw.reserve(18900);
//    data_left_raw.reserve(18900);
//    data_idle_raw.reserve(18900);

    //打开文件left
    csvFile->setFileName(csvFilePath_left);
    if(!csvFile->open(QIODevice::ReadOnly | QIODevice::Text))
    {
        emit finish_train(0);
        return -1;
    }
    int i1 = 0;
    QTextStream inStream_1(csvFile);
    while(!inStream_1.atEnd())
    {
        QStringList tmp = inStream_1.readLine().split(","); //保存到List当中
        Q_FOREACH(QString str, tmp)
            data_left_raw[i1++] = str.toDouble();
    }
    csvFile->close();


    //打开文件right
    csvFile->setFileName(csvFilePath_right);
    if(!csvFile->open(QIODevice::ReadOnly | QIODevice::Text))
    {
        emit finish_train(0);
        return -1;
    }
    int i2 = 0;
    QTextStream inStream_2(csvFile);
    while(!inStream_2.atEnd())
    {
        QStringList tmp = inStream_2.readLine().split(","); //保存到List当中
        Q_FOREACH(QString str, tmp)
            data_right_raw[i2++] = str.toDouble();
    }
    csvFile->close();


    //打开文件idle
    csvFile->setFileName(csvFilePath_idle);
    if(!csvFile->open(QIODevice::ReadOnly | QIODevice::Text))
    {
        emit finish_train(0);
        return -1;
    }
    int i3 = 0;
    QTextStream inStream_3(csvFile);
    while(!inStream_3.atEnd())
    {
        QStringList tmp = inStream_3.readLine().split(","); //保存到List当中
        Q_FOREACH(QString str, tmp)
            data_idle_raw[i3++] = str.toDouble();
    }
    csvFile->close();

    //CSP训练,内部内置滤波器
    csp_train_ovo(data_left_raw, data_right_raw, data_idle_raw, _W1, _sample1, _W2, _sample2, _W3, _sample3);
    emit finish_train(1);
}

void CSP_Train::run()
{
    operationStep(fname);
}
