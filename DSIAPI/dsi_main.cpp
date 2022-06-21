#include "dsi_main.h"
#include "DSIAPI/dsi_debug.h"

//定义缓存区
QQueue<channelSignal* > threadShareData;
int queueSize = 300;
//定义互斥锁
QMutex *bufferMutex ;
//定义完成量
QWaitCondition *bufferIsEmpty;
QWaitCondition *bufferIsFull;



DSI_Main::DSI_Main(QObject *parent, char *port, char *montage, int montageCount, char *reference, int verbosity, int perbatch, double durationsec, double buffersec) : QThread(parent),
    globalPort(port),globalMontage(montage), globalMontageCount(montageCount), globalReference(reference), globalVerbosity(verbosity),globalSamplesPerBatch(perbatch),globalDurationSec(durationsec),globalBufferAheadSec(buffersec),readingFlag(true),
    fftwSwitchFlag(false),recordSwitchFlag(false)
{
    initLoadLib();
    chooseChannlString = settingChannel(QString(globalMontage));
}

DSI_Main::~DSI_Main()
{
    //终止线程，关闭DSI
    readingFlag = false;
    quit();
    wait();
    Finish(DSIh);
    DSIh = nullptr;
}

//初始化DSI
bool DSI_Main::initLoadLib()
{
    const char * dllname = NULL;
    int load_error = Load_DSI_API( dllname );

    if( load_error < 0 ){
        qDebug()<<"failed to load dynamic library"<<DSI_DYLIB_NAME( dllname );
        return false;
    }else if( load_error > 0 ){
        qDebug()<<"failed to import "<<load_error<<"functions from dynamic library "<<DSI_DYLIB_NAME( dllname );
        return false;
    }else{
        qDebug()<<"DSI API version "<<DSI_GetAPIVersion()<<" loaded";
    }

    if( strcmp( DSI_GetAPIVersion(), DSI_API_VERSION ) != 0 )
    {
        qDebug()<<"WARNING - mismatched versioning: program was compiled with DSI.h version "<<DSI_API_VERSION<<" but just loaded shared library version "<<DSI_GetAPIVersion()<<". You should ensure that you are using matching versions of the API files - contact Wearable Sensing if you are missing a file.";
        return false;
    }
    return true;
}

//开始缓存区接收数据
int DSI_Main::BufferingStart()
{
    int error;
    double bufferAheadSec = globalBufferAheadSec/1000.0;//缓冲数据时间,除以1000后，以秒为单位

    error = StartUp(&DSIh);
    if( error ) return error;
    //配置
    DSI_Headset_ConfigureBatch( DSIh, globalSamplesPerBatch, bufferAheadSec ); CHECK
    //开启后台获取,不能使用samplecallback注册，也不应该用dsi_headset_idle()或dsi_headset_receive() ,
    DSI_Headset_StartBackgroundAcquisition( DSIh ); CHECK
    return 0;
}

//存储所使用的通道
QList<QString> DSI_Main::settingChannel(QString data)
{
    QList<QString> L;
    for(int i=0;i<globalMontageCount;i++){
        QString inData = data.section(',',i,i);
        bool isFlag = inData.indexOf("-");
        if(isFlag)
        {
            L<<inData.section("-",0,0);
        }else{
            L<<inData;
        }
    }
    return L;
}

//DSI线程运行
void DSI_Main::run()
{
    while(readingFlag)
    {
        //这种阻塞呼叫的目的是确保在精确规定的时间间隔内采集
        DSI_Headset_WaitForBatch( DSIh );
        readDataSlots();
    }
}

//读取缓存区数据
int DSI_Main::readDataSlots()
{
    int channlN;
    //从连接开始到现在的时间小于要获取数据的时间
    //while(DSI_Headset_SecondsSinceConnection( DSIh ) < globalDurationSec)
    for(int sampleIndex = 0; sampleIndex < globalSamplesPerBatch; sampleIndex++ )
    {
        channlN = DSI_Headset_GetNumberOfChannels( DSIh );
        for(int channelIndex = 0; channelIndex < channlN; channelIndex++ )
        {
            DSI_Channel c = DSI_Headset_GetChannelByIndex( DSIh, channelIndex );CHECK
            //获取通道名称
            QString channelName = QString(DSI_Channel_GetString( c )).section("-",0,0);CHECK
            //获取通道信号
            double channelSignal = DSI_Channel_ReadBuffered( c );CHECK
            //曲线数据 & FFTW数据
            juiceDataStore(&chartSignalGet,channelName,channelSignal);
        }
        /***Chart***/
        emit bufferDataSignal(chartSignalGet);

        /***离线保存***/
        if(recordSwitchFlag==true)
        {
            emit offLineDataSignal(chartSignalGet);
        }

        /***线程之间 传递数据***/
        if(fftwSwitchFlag==true)
        {
            bufferMutex->lock();
            //若缓存区已满,交出锁,进入休眠等待
            if(threadShareData.size() == queueSize)
            {
                bufferIsFull->wait(bufferMutex);
            }
            //若线程结束,强制返回，并不进行睡眠
            if(fftwSwitchFlag==false)
            {
                bufferIsEmpty->wakeAll();
                bufferMutex->unlock();
                return true;
            }
            //向缓存区写入数据
            channelSignal *pushData = new channelSignal;
            *pushData = chartSignalGet;
            threadShareData.enqueue(pushData);
            //缓存区写入数据，唤醒读进程
            bufferIsEmpty->wakeAll();
            bufferMutex->unlock();
        }
    }
}





DSI_FFTW::DSI_FFTW(AlorithmSwitch *AS, QObject *parent) : QThread(parent),chooseAlorithm(AS)
{
    //定义缓存区大小
    threadShareData.reserve(queueSize);
    readingFlag = true;
    //初始化全区动态变量
    bufferMutex = new QMutex;
    bufferIsEmpty = new QWaitCondition;
    bufferIsFull = new QWaitCondition;
    //recordInit();
}

DSI_FFTW::~DSI_FFTW()
{
    //终止线程，关闭DSI
    readingFlag = false;
    bufferIsEmpty->wakeAll();
    quit();
    wait();
    //缓存区清空
    threadShareData.clear();
    //删除动态全局变量

    if(csvFile->isOpen())
        csvFile->close();
    delete csvFile;
    delete in;
    in = nullptr;
    csvFile = nullptr;

    delete bufferMutex;
    delete bufferIsEmpty;
    delete bufferIsFull;
    delete chooseAlorithm;
    bufferMutex = nullptr;
    bufferIsEmpty = nullptr;
    bufferIsFull = nullptr;
    chooseAlorithm = nullptr;
}

void DSI_FFTW::recordInit()
{
    //判断文件是否存在
    int index = 0;
    csvFile = new QFile();
    QString csvFilePath;
    while(1)
    {
        csvFilePath = QString("%1/train/%2_record_%3.csv").arg(QCoreApplication::applicationDirPath()).arg(useName).arg(index);
        if(QFile::exists(csvFilePath)){
            ++index;
        }else{
            break;
        }
        if(index==10)
            break;
    }
    //创建文件并打开
    csvFile->setFileName(csvFilePath);
    if(!csvFile->open(QIODevice::WriteOnly))
    {
        return;
    }

    //定义输出流
    in = new QTextStream(csvFile);

    //记录表头
    *in<<"P3,C3,F3,Fz,F4,C4,P4,Cz,Pz,A1,Fp1,Fp2,T3,T5,O1,O2,F7,F8,A2,T6,T4";
    *in<<"\n";
}

//线程运行
void DSI_FFTW::run()
{
    while(readingFlag)
    {
        motorImageryUseFFTW();
    }
}

void DSI_FFTW::motorImageryUseFFTW()
{
    /*线程之间 传递数据*/
    bufferMutex->lock();
    //若缓存区为空,交出锁,进入休眠等待
    if(threadShareData.isEmpty())
    {
        bufferIsEmpty->wait(bufferMutex);
    }
    //若线程结束,强制返回，并不进行睡眠
    if(readingFlag==false)
    {
        bufferIsFull->wakeAll();
        bufferMutex->unlock();
        return;
    }

    //从缓存区拿出数据,1/300时间窗，想怎么使用可根据接口自定义
    channelSignal *tmpSignalDataAdress = threadShareData.dequeue();
    bufferIsFull->wakeAll();
    bufferMutex->unlock();

    //记录数据
    *in << (*tmpSignalDataAdress).P3.first << ",";
    *in << (*tmpSignalDataAdress).C3.first << ",";
    *in << (*tmpSignalDataAdress).F3.first << ",";
    *in << (*tmpSignalDataAdress).Fz.first << ",";
    *in << (*tmpSignalDataAdress).F4.first << ",";
    *in << (*tmpSignalDataAdress).C4.first << ",";
    *in << (*tmpSignalDataAdress).P4.first << ",";
    *in << (*tmpSignalDataAdress).Cz.first << ",";
    *in << (*tmpSignalDataAdress).Pz.first << ",";
    *in << (*tmpSignalDataAdress).A1.first << ",";
    *in << (*tmpSignalDataAdress).Fp1.first<< ",";
    *in << (*tmpSignalDataAdress).Fp2.first<< ",";
    *in << (*tmpSignalDataAdress).T3.first << ",";
    *in << (*tmpSignalDataAdress).T5.first << ",";
    *in << (*tmpSignalDataAdress).O1.first << ",";
    *in << (*tmpSignalDataAdress).O2.first << ",";
    *in << (*tmpSignalDataAdress).F7.first << ",";
    *in << (*tmpSignalDataAdress).F8.first << ",";
    *in << (*tmpSignalDataAdress).A2.first << ",";
    *in << (*tmpSignalDataAdress).T6.first << ",";
    *in << (*tmpSignalDataAdress).T4.first << "\n";

    tmpSignalDataAdress->MI_SSVEP = uiState;
    //Template Method 算法调用统一接口运算
    int result = chooseAlorithm->operationStep(*tmpSignalDataAdress);

    //决策
    if(result == 1)
    {
        if(tmpSignalDataAdress->MI_SSVEP)
            *in << "\n"<<"MI Left" << "\n"<< "\n";
        else
            *in << "\n"<<"SSVEP Left" << "\n"<< "\n";

        emit C3LeftSignal();
    }else if(result == 2){
        if(tmpSignalDataAdress->MI_SSVEP)
            *in << "\n"<<"MI Right" << "\n"<< "\n";
        else
            *in << "\n"<<"SSVEP Right" << "\n"<< "\n";

        emit C4RightSignal();
    }else if(result == 0){
        if(tmpSignalDataAdress->MI_SSVEP)
            *in << "\n"<<"MI Idle" << "\n"<< "\n";
        else
            *in << "\n"<<"SSVEP Idle" << "\n"<< "\n";

        emit IdleSignal();
    }else if(result == -1){
        qDebug()<<"juice error! or juicing!";
    }else{
        //qDebug()<<"unknow error!"<<result;
    }
    delete tmpSignalDataAdress;
}

//判断存储SIGNAL数据
void DSI_Main::juiceDataStore(channelSignal *data, QString channelSignaldata, double channelSignalData)
{
    if(channelSignaldata == "Fp1")
        data->Fp1.first = channelSignalData;
    else if(channelSignaldata == "Fp2")
        data->Fp2.first = channelSignalData;
    else if(channelSignaldata == "F7")
        data->F7.first = channelSignalData;
    else if(channelSignaldata == "F3")
        data->F3.first = channelSignalData;
    else if(channelSignaldata == "Fz")
        data->Fz.first = channelSignalData;
    else if(channelSignaldata == "F4")
        data->F4.first = channelSignalData;
    else if(channelSignaldata == "F8")
        data->F8.first = channelSignalData;
    else if(channelSignaldata == "A1")
        data->A1.first = channelSignalData;
    else if(channelSignaldata == "T3")
        data->T3.first = channelSignalData;
    else if(channelSignaldata == "C3"){
        data->C3.first = channelSignalData;
    }else if(channelSignaldata == "Cz")
        data->Cz.first = channelSignalData;
    else if(channelSignaldata == "C4"){
        data->C4.first = channelSignalData;
    }else if(channelSignaldata == "T4")
        data->T4.first = channelSignalData;
    else if(channelSignaldata == "A2")
        data->A2.first = channelSignalData;
    else if(channelSignaldata == "T5")
        data->T5.first = channelSignalData;
    else if(channelSignaldata == "P3")
        data->P3.first = channelSignalData;
    else if(channelSignaldata == "Pz")
        data->Pz.first = channelSignalData;
    else if(channelSignaldata == "P4")
        data->P4.first = channelSignalData;
    else if(channelSignaldata == "T6")
        data->T6.first = channelSignalData;
    else if(channelSignaldata == "O1")
        data->O1.first = channelSignalData;
    else if(channelSignaldata == "O2")
        data->O2.first = channelSignalData;
    else if(channelSignaldata == "X1")
        data->X1.first = channelSignalData;
    else if(channelSignaldata == "X2")
        data->X2.first = channelSignalData;
    else if(channelSignaldata == "X3")
        data->X3.first = channelSignalData;
}

//连接设备
int DSI_Main::StartUp(DSI_Headset * headsetOut)
{
    DSI_Headset h;

    if( headsetOut ) *headsetOut = NULL;
    h = DSI_Headset_New( NULL ); CHECK
    DSI_Headset_SetMessageCallback( h, Message ); CHECK
    DSI_Headset_SetVerbosity( h, globalVerbosity ); CHECK
    //连接
    DSI_Headset_Connect( h, globalPort ); CHECK
    //选择通道
    DSI_Headset_ChooseChannels( h, globalMontage, globalReference, 0 ); CHECK
    //打印出DSI的参数信息
    fprintf( stderr, "%s\n", DSI_Headset_GetInfoString( h ) ); CHECK
    if( headsetOut ) *headsetOut = h;
    return 0;
}

//断开设备
int DSI_Main::Finish( DSI_Headset h )
{
    DSI_Headset_SetSampleCallback( h, NULL, NULL ); CHECK
    // This stops our application from responding to received samples.
    DSI_Headset_StopDataAcquisition( h ); CHECK
    // This send a command to the headset to tell it to stop sendingsamples.
    DSI_Headset_Idle( h, 1.0 ); CHECK
    // This allows more than enough time to receive any samples that were
    // sent before the stop command is carried out, along with the alarm
    // signal that the headset sends out when it stops
    DSI_Headset_Delete( h ); CHECK
    // This is the only really necessary step. Disconnects from the serial port, frees memory, etc.
    return 0;
}

//判断存储IMP数据
void juiceImpedanceStore(channelImpedance *data, QString channelSignaldata, double channelSignalData)
{
    if(channelSignaldata == "Fp1")
        data->Fp1.first = channelSignalData;
    else if(channelSignaldata == "Fp2")
        data->Fp2.first = channelSignalData;
    else if(channelSignaldata == "F7")
        data->F7.first = channelSignalData;
    else if(channelSignaldata == "F3")
        data->F3.first = channelSignalData;
    else if(channelSignaldata == "Fz")
        data->Fz.first = channelSignalData;
    else if(channelSignaldata == "F4")
        data->F4.first = channelSignalData;
    else if(channelSignaldata == "F8")
        data->F8.first = channelSignalData;
    else if(channelSignaldata == "A1")
        data->A1.first = channelSignalData;
    else if(channelSignaldata == "T3")
        data->T3.first = channelSignalData;
    else if(channelSignaldata == "C3")
        data->C3.first = channelSignalData;
    else if(channelSignaldata == "Cz")
        data->Cz.first = channelSignalData;
    else if(channelSignaldata == "C4")
        data->C4.first = channelSignalData;
    else if(channelSignaldata == "T4")
        data->T4.first = channelSignalData;
    else if(channelSignaldata == "A2")
        data->A2.first = channelSignalData;
    else if(channelSignaldata == "T5")
        data->T5.first = channelSignalData;
    else if(channelSignaldata == "P3")
        data->P3.first = channelSignalData;
    else if(channelSignaldata == "Pz")
        data->Pz.first = channelSignalData;
    else if(channelSignaldata == "P4")
        data->P4.first = channelSignalData;
    else if(channelSignaldata == "T6")
        data->T6.first = channelSignalData;
    else if(channelSignaldata == "O1")
        data->O1.first = channelSignalData;
    else if(channelSignaldata == "O2")
        data->O2.first = channelSignalData;
    else if(channelSignaldata == "X1")
        data->X1.first = channelSignalData;
    else if(channelSignaldata == "X2")
        data->X2.first = channelSignalData;
    else if(channelSignaldata == "X3")
        data->X3.first = channelSignalData;
}

channelImpedance *ImpData = new channelImpedance;
//存储通道电阻值
void PrintImpedances( DSI_Headset h, double packetOffsetTime, void * userData )
{
    unsigned int numberOfSources = DSI_Headset_GetNumberOfSources( h );

    for(unsigned int sourceIndex = 0; sourceIndex < numberOfSources; sourceIndex++ )
    {
        DSI_Source s = DSI_Headset_GetSourceByIndex( h, sourceIndex );
        if( DSI_Source_IsReferentialEEG( s ) && ! DSI_Source_IsFactoryReference( s ) )
        {
            QString sourceName = DSI_Source_GetName( s );
            double sourceImp = DSI_Source_GetImpedanceEEG( s );
            juiceImpedanceStore(ImpData,sourceName,sourceImp);
            //qDebug()<<sourceName<<sourceImp;
        }
    }
}

//配置电阻测量
int DSI_Main::ImpedanceSlots()
{
    //阻抗驱动注入110Hz与130Hz，允许测量阻抗
    DSI_Headset_StartImpedanceDriver( DSIh ); CHECK
    //数据开始从脑电帽发出
    DSI_Headset_StartDataAcquisition( DSIh ); CHECK
    //直到阻抗稳定一秒之后在打印,空转一秒
    DSI_Headset_Idle( DSIh, 1.0 ); CHECK
    //定时调用PrintImpedances
    DSI_Headset_SetSampleCallback( DSIh, PrintImpedances, NULL ); CHECK
    //初始化成功，开启定时器
    ImpTimer = new QTimer;
    ImpTimer->start(1000);
    QObject::connect(ImpTimer,&QTimer::timeout,this,&DSI_Main::sendDataFixedTime);
    return 0;
}

//定时器返回数据：阻抗、
void DSI_Main::sendDataFixedTime()
{
    //Impedance
    channelImpedance ImpFixedTimeData = *ImpData;
    emit fixedTimeSignal(ImpFixedTimeData);
}

//复位阻抗按键
void DSI_Main::resetSlots()
{
    DSI_Headset_StartAnalogReset(DSIh);
}

