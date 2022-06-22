#include "dialog.h"
#include "ui_dialog.h"

#define DSI_CHART_SHOW_NUM  21
#define DSI_REFERENCE_DATA  "A1/2+A2/2" //"Pz/21+A1/21+A2/21+Cz/21+C3/21+C4/21+Fp1/21+Fp2/21+F7/21+F3/21+Fz/21+F4/21+F8/21+T3/21+T4/21+T5/21+P3/21+P4/21+T6/21+O1/21+O2/21"
#define DSI_CHANNEL_DATA    "C3,C4,A1,A2,O1,O2,Fp1,Fp2,F7,F3,Fz,F4,F8,T3,T4,T5,P3,P4,T6,Cz,Pz"
//#define DSI_CHANNEL_DATA      "C3,Cz,C4,F3,F4,P3,Pz,P4,A1,A2,O1,O2,Fp1,Fp2,F7,Fz,F8,T3,T4,T5,T6"
#define DSI_PORT_DATA       "COM6"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    //去掉标题栏
    this->setWindowFlags(Qt::CustomizeWindowHint);
    //声明信号变量channelSignal
    qRegisterMetaType<channelSignal>("channelSignal");
    //加载QSS
    loadQssSlot("Style");
    //初始化对象
    chartWidght = nullptr;
    impedanceWidght = nullptr;
    dsiFFtWDataHandle = nullptr;
    recordWidght = nullptr;
    sendPort = nullptr;
    portThread = nullptr;
    parad = nullptr;
    CSPTrain = nullptr;

    initSlot();
    //初始化信号与槽
    connetSlot();
}

Dialog::~Dialog()
{
    portThread->quit();
    portThread->wait();
    delete portThread;
    dsiDataRec->deleteLater();
    delete chartWidght;
    delete impedanceWidght;
    delete recordWidght;
    delete sendPort;
    if(dsiFFtWDataHandle!=nullptr)
        delete dsiFFtWDataHandle;
    delete startButton;
    delete stopButton;
    delete trainBox;
    delete CSPTrain;
    delete startTrainButton;
    delete fftwStartButton;
    delete fftwStopButton;
    delete buttonLayout;
    delete widghtLayout;
    delete portBox;
    delete searchButton;
    delete controls;
    delete player;
    delete videoWidget;
    delete medialist;
    delete parad;
    delete ui;
}

//初始化对象
void Dialog::initSlot()
{
    //DSI
    dsiDataRec = new DSI_Main(nullptr,DSI_PORT_DATA,DSI_CHANNEL_DATA,DSI_CHART_SHOW_NUM,DSI_REFERENCE_DATA,2,30,-1,15.0);
    if(chartWidght==nullptr || impedanceWidght==nullptr || recordWidght==nullptr)
    {
        //channelChart
        chartWidght = new signalChart(nullptr,dsiDataRec->chooseChannlString);
        chartWidght->chartInit();
        //impedanceChart
        impedanceWidght = new impedancechart;
        //saveEEG
        recordWidght = new saveEEG(this,dsiDataRec->chooseChannlString,DSI_REFERENCE_DATA);
        recordWidght->startRecordButton(false);
        recordWidght->stopRecordButton(false);
    }
    //port
    sendPort = new rehabilitativeUsart;
    portThread = new QThread;
    sendPort->moveToThread(portThread);
    portThread->start();

    //实例化控件
    startButton = new QPushButton("Start DSI");
    stopButton = new QPushButton("Stop DSI");
    fftwStartButton = new QPushButton("Start Control");
    fftwStopButton = new QPushButton("Stop Control");
    stopButton->setEnabled(false);
    fftwStopButton->setEnabled(false);
    fftwStartButton->setEnabled(false);
    buttonLayout = new QHBoxLayout;
    widghtLayout = new QHBoxLayout;
    portBox = new QComboBox;
    searchButton = new QPushButton("Search Port");

    trainBox = new QComboBox;
    trainBox->addItem("CSP and CCA.nomarl");
    trainBox->addItem("CSP and CCA.SW");
    trainBox->addItem("FFTW.SW");
    trainBox->addItem("CSP and KCCA.nomarl");
    trainBox->addItem("CSP and KCCA.SW");
    startTrainButton = new QPushButton("Start Train");
    newLayout = new QHBoxLayout;

    //布局
    buttonLayout->addWidget(recordWidght);
    buttonLayout->addWidget(startButton);
    buttonLayout->addWidget(stopButton);
    buttonLayout->setSpacing(10);
    buttonLayout->setContentsMargins(0,0,0,0);

    widghtLayout->addWidget(impedanceWidght);
    widghtLayout->addStretch();
    widghtLayout->addWidget(chartWidght);
    widghtLayout->setSpacing(10);
    widghtLayout->setContentsMargins(0,0,0,0);

    newLayout->addWidget(startTrainButton);
    startTrainButton->setEnabled(false);
    newLayout->addWidget(trainBox);
    trainBox->setEnabled(false);
    newLayout->addWidget(fftwStartButton);
    newLayout->addWidget(fftwStopButton);
    newLayout->addWidget(searchButton);
    newLayout->addWidget(portBox);
    newLayout->setSpacing(10);
    newLayout->setContentsMargins(0,0,0,0);

    //添加
    ui->verticalLayout_3->addLayout(widghtLayout);
    ui->verticalLayout_3->addLayout(buttonLayout);
    ui->verticalLayout_3->addLayout(newLayout);
    ui->verticalLayout_3->setSpacing(5);
    ui->verticalLayout_3->setContentsMargins(10,5,10,1);

    //video
    player = new QMediaPlayer;
    videoWidget = new QVideoWidget;
    medialist=new QMediaPlaylist;
    ui->vedioLayout->addWidget(videoWidget);
    player->setVideoOutput(videoWidget);
    QString vedioPath = QString("%1%2").arg(qApp->applicationDirPath().replace("/","\\")).arg(QString("\\video\\left.avi"));
    QString vedioPath2 = QString("%1%2").arg(qApp->applicationDirPath().replace("/","\\")).arg(QString("\\video\\right.avi"));
    medialist->addMedia(QUrl::fromLocalFile(vedioPath));
    medialist->addMedia(QUrl::fromLocalFile(vedioPath2));
//    player->setPlaylist(medialist);
    medialist->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
    QFont font ("Microsoft YaHei", 15, 70);
    ui->label->setFont(font);
    // Qt6 deprecated QMediaPlaylist
    controls = new PlayerControls;
    controls->setState(player->playbackState());
    controls->setMuted(controls->isMuted());
    connect(controls, &PlayerControls::play, player, &QMediaPlayer::play);
    connect(controls, &PlayerControls::pause, player, &QMediaPlayer::pause);
    connect(controls, &PlayerControls::stop, player, &QMediaPlayer::stop);
    connect(controls, &PlayerControls::next, medialist, &QMediaPlaylist::next);
    connect(controls, &PlayerControls::changeRate, player, &QMediaPlayer::setPlaybackRate);
    connect(controls, &PlayerControls::stop, videoWidget, QOverload<>::of(&QVideoWidget::update));
    connect(player, &QMediaPlayer::playbackStateChanged, controls, &PlayerControls::setState);

    //fan_shi
    parad = new Paradigm;
    fanshiLayout = new QHBoxLayout;
    fanshiLayout->addWidget(parad);
    fanshiLayout->setSpacing(10);
    fanshiLayout->setContentsMargins(0,0,0,0);
    ui->paradigm->setLayout(fanshiLayout);

    fs_State = nullptr;
    //waitChange = new QTimer;
    //QObject::connect(waitChange, &QTimer::timeout, [&](){ui->tabWidget->setCurrentIndex(0); });
    QObject::connect(parad, &Paradigm::closeSignal, [&](){ fftwStop(); });

}

//信号与曹连接
void Dialog::connetSlot()
{
    //chart数据更新
    QObject::connect(dsiDataRec,&DSI_Main::bufferDataSignal,chartWidght,&signalChart::chartAddData);
    //impedance数据更新
    QObject::connect(dsiDataRec,&DSI_Main::fixedTimeSignal,impedanceWidght,&impedancechart::impedanceQualitySlot);
    //reset
    QObject::connect(impedanceWidght,&impedancechart::resetSignals,dsiDataRec,&DSI_Main::resetSlots);
    //start DSI
    QObject::connect(startButton,&QPushButton::clicked,this,&Dialog::dsiStart);
    //stop DSI
    QObject::connect(stopButton,&QPushButton::clicked,this,&Dialog::dsiStop);
    //start FFTW
    QObject::connect(fftwStartButton,&QPushButton::clicked,this,&Dialog::fftwStart);
    //stop FFTW
    QObject::connect(fftwStopButton,&QPushButton::clicked,this,&Dialog::fftwStop);
    //start/stop record
    QObject::connect(recordWidght,&saveEEG::isTimeToRecord,this,[&](bool flag){
        if(flag == true)
        {
            QObject::connect(dsiDataRec,&DSI_Main::offLineDataSignal,recordWidght,&saveEEG::recordEEGslots);
        }else{
            QObject::disconnect(dsiDataRec,&DSI_Main::offLineDataSignal,recordWidght,&saveEEG::recordEEGslots);
        }
        dsiDataRec->recordSwitchFlag = flag;
    });    
    QObject::connect(searchButton,&QPushButton::clicked,this,[&](){
        //find com
        if(searchButton->text() == "Search Port"){
            searchButton->setDisabled(true);
            sendPort->findUsingPort();
        //open com
        }else if(searchButton->text() == "Connect"){
            sendPort->connectComPort(portBox->currentText());
            searchButton->setText("Disconnect");
        //close com
        }else if(searchButton->text() == "Disconnect"){
            sendPort->closePort();
            searchButton->setText("Search Port");
           }
    });
    //add com
    QObject::connect(sendPort,&rehabilitativeUsart::usingComPortSignal,this,[&](QVector<QString> data){
        foreach(QString portName, data)
        {
            portBox->clear();
            portBox->addItem(portName);
        }
        portBox->addItem("COM10");
        portBox->addItem("COM11");
        portBox->addItem("COM12");
        portBox->addItem("COM13");
        portBox->addItem("COM2");
        searchButton->setText("Connect");
        searchButton->setDisabled(false);
    });
    //delete com thread
    //QObject::connect(portThread, &QThread::finished, sendPort, &QObject::deleteLater);
    //video
    QObject::connect(ui->rightButton,SIGNAL(clicked()),this,SLOT(startVideoRight()));
    QObject::connect(ui->leftButton,SIGNAL(clicked()),this,SLOT(startVideoLeft()));

    QObject::connect(trainBox,&QComboBox::currentTextChanged,this,[&](QString data){
        if(data=="CSP and CCA.nomarl")
        {
            startTrainButton->setEnabled(true);
        }else if(data=="CSP and CCA.SW"){
            startTrainButton->setEnabled(true);
        }else if(data=="CSP and KCCA.nomarl"){
            startTrainButton->setEnabled(true);
        }else if(data=="CSP and KCCA.SW"){
            startTrainButton->setEnabled(true);
        }else if(data=="FFTW.SW"){
            startTrainButton->setEnabled(false);
        }
    });

    QObject::connect(startTrainButton,&QPushButton::clicked,this,[&]()
    {
        startTrainButton->setEnabled(false);
        if(CSPTrain!=nullptr)
            delete CSPTrain;
        CSPTrain = new CSP_Train;

        QObject::connect(CSPTrain,&CSP_Train::finish_train,this,[&](bool flag){
            startTrainButton->setEnabled(true);
            if(flag==0)
                QMessageBox::about(this,"DSI","Train File Open Failed!");
            else
                QMessageBox::about(this,"DSI","Finish Train!");
        });

        CSPTrain->fname = recordWidght->lineeditdata();
        CSPTrain->start();
    });


}

//加载QSS
void Dialog::loadQssSlot(QString name)
{
    QFile file(QString("://QSS/%1.qss").arg(name));
    file.open(QFile::ReadOnly);
    QString qss = QLatin1String(file.readAll());
    qApp->setStyleSheet(qss);
}

//打开DSI
void Dialog::dsiStart()
{
    //DSI
    if(dsiDataRec==nullptr)
    {
        dsiDataRec = new DSI_Main(nullptr,DSI_PORT_DATA,DSI_CHANNEL_DATA,DSI_CHART_SHOW_NUM,DSI_REFERENCE_DATA,2,30,-1,15.0);
        QObject::connect(dsiDataRec,&DSI_Main::bufferDataSignal,chartWidght,&signalChart::chartAddData);
        QObject::connect(dsiDataRec,&DSI_Main::fixedTimeSignal,impedanceWidght,&impedancechart::impedanceQualitySlot);
        QObject::connect(impedanceWidght,&impedancechart::resetSignals,dsiDataRec,&DSI_Main::resetSlots);
    }
    if(!dsiDataRec->BufferingStart())
    {
        dsiDataRec->ImpedanceSlots();
        dsiDataRec->start();
        startButton->setEnabled(false);
        stopButton->setEnabled(true);
        fftwStartButton->setEnabled(true);
        fftwStopButton->setEnabled(false);
        recordWidght->startRecordButton(true);
        recordWidght->stopRecordButton(false);
        startTrainButton->setEnabled(true);
        if(trainBox->currentText()!="FFTW.SW")
            startTrainButton->setEnabled(true);
        trainBox->setEnabled(true);
    }else{
        QMessageBox::about(this,"DSI","Error connect to DSI!");
    }
}

//关闭DSI
void Dialog::dsiStop()
{
    startButton->setEnabled(true);
    stopButton->setEnabled(false);
    fftwStartButton->setEnabled(false);
    fftwStopButton->setEnabled(false);
    recordWidght->startRecordButton(false);
    recordWidght->stopRecordButton(false);
    startTrainButton->setEnabled(false);
    trainBox->setEnabled(false);
    if(dsiFFtWDataHandle != nullptr)
    {
        dsiFFtWDataHandle->readingFlag = false;
        dsiDataRec->fftwSwitchFlag = false;
        dsiFFtWDataHandle->deleteLater();
        dsiFFtWDataHandle = nullptr;
    }
    //终止线程，关闭DSI
    delete dsiDataRec;
    dsiDataRec = nullptr;
    //imp复原
    channelImpedance impData;
    impedanceWidght->impedanceQualitySlot(impData);
}

//打开FFTW===================算法入口===================
void Dialog::fftwStart()
{
    if(trainBox->currentText()=="CSP and CCA.nomarl")
    {
        if(CSPTrain==nullptr)
        {
            QMessageBox::about(this,"DSI","Train First!");
            return;
        }
        dsiFFtWDataHandle = new DSI_FFTW(new CSP_CCA_AlorithmSwitch(CSPTrain->_W1, CSPTrain->_sample1,
                                                                    CSPTrain->_W2, CSPTrain->_sample2,
                                                                    CSPTrain->_W3, CSPTrain->_sample3));
        dsiFFtWDataHandle->useName = recordWidght->lineeditdata();
        dsiFFtWDataHandle->recordInit();
    }else if(trainBox->currentText()=="CSP and CCA.SW"){
        if(CSPTrain==nullptr)
        {
            QMessageBox::about(this,"DSI","Train First!");
            return;
        }
        dsiFFtWDataHandle = new DSI_FFTW(new SW_CSP_CCA_AlorithmSwitch(CSPTrain->_W1, CSPTrain->_sample1,
                                                                       CSPTrain->_W2, CSPTrain->_sample2,
                                                                       CSPTrain->_W3, CSPTrain->_sample3));
        dsiFFtWDataHandle->useName = recordWidght->lineeditdata();
        dsiFFtWDataHandle->recordInit();

        /**********************以下为新增**********************/
    }else if(trainBox->currentText()=="CSP and KCCA.nomarl")
    {
        if(CSPTrain==nullptr)
        {
            QMessageBox::about(this,"DSI","Train First!");
            return;
        }
        dsiFFtWDataHandle = new DSI_FFTW(new CSP_KCCA_AlorithmSwitch(CSPTrain->_W1, CSPTrain->_sample1,
                                                                    CSPTrain->_W2, CSPTrain->_sample2,
                                                                    CSPTrain->_W3, CSPTrain->_sample3));
        dsiFFtWDataHandle->useName = recordWidght->lineeditdata();
        dsiFFtWDataHandle->recordInit();
    }else if(trainBox->currentText()=="CSP and KCCA.SW"){
        if(CSPTrain==nullptr)
        {
            QMessageBox::about(this,"DSI","Train First!");
            return;
        }
        dsiFFtWDataHandle = new DSI_FFTW(new SW_CSP_KCCA_AlorithmSwitch(CSPTrain->_W1, CSPTrain->_sample1,
                                                                       CSPTrain->_W2, CSPTrain->_sample2,
                                                                       CSPTrain->_W3, CSPTrain->_sample3));
        dsiFFtWDataHandle->useName = recordWidght->lineeditdata();
        dsiFFtWDataHandle->recordInit();
        /**********************以上为新增**********************/

    } else if(trainBox->currentText()=="FFTW.SW"){
        dsiFFtWDataHandle = new DSI_FFTW(new FFTW_AlorithmSwitch());
        dsiFFtWDataHandle->useName = recordWidght->lineeditdata();
        dsiFFtWDataHandle->recordInit();
    }

    dsiFFtWDataHandle->uiState = 0;
    dsiDataRec->fftwSwitchFlag = true;
    dsiFFtWDataHandle->start();
    fftwStartButton->setEnabled(false);
    fftwStopButton->setEnabled(true);
    startTrainButton->setEnabled(false);
    trainBox->setEnabled(false);

    QObject::connect(dsiFFtWDataHandle,&DSI_FFTW::C3LeftSignal,this,[&](){ changeMode(MI_LEFT); });
    QObject::connect(dsiFFtWDataHandle,&DSI_FFTW::C4RightSignal,this,[&](){ changeMode(MI_RIGHT); });
    QObject::connect(dsiFFtWDataHandle,&DSI_FFTW::IdleSignal,this,[&](){ changeMode(MI_IDLE); });
    ui->tabWidget->setCurrentIndex(2);
    fs_State = new Stand_Mode;
    parad->StandMode();
}

//关闭FFTW
void Dialog::fftwStop()
{
    if(fs_State == nullptr)
        return;

    delete fs_State;
    fs_State = nullptr;

    dsiFFtWDataHandle->readingFlag = false;
    dsiDataRec->fftwSwitchFlag = false;
    dsiFFtWDataHandle->deleteLater();
    dsiFFtWDataHandle = nullptr;
    fftwStartButton->setEnabled(true);
    fftwStopButton->setEnabled(false);
    if(trainBox->currentText()!="FFTW.SW")
        startTrainButton->setEnabled(true);
    trainBox->setEnabled(true);

    ui->tabWidget->setCurrentIndex(0);
}

//打开right视频
void Dialog::startVideoRight()
{
    medialist->setCurrentIndex(1);
    videoWidget->show();
    player->play();
}

//打开left视频
void Dialog::startVideoLeft()
{
    medialist->setCurrentIndex(0);
    videoWidget->show();
    player->play();
}

void Dialog::printToLable(QString data)
{
    ui->label->setText(data);
}

void Dialog::toSSVEP()
{
    ui->tabWidget->setCurrentIndex(2);
}

void Dialog::toMI()
{
    ui->label->clear();
    ui->tabWidget->setCurrentIndex(1);
    //waitChange->start(3000);
}

//范式状态切换
/****
 * 拟写一个线程：在线程中将缓存队列中存储的状态取出进行状态机判断
 * 判断过后主要工作：1.串口发送指令 2.进行状态机切换 3.范式切换 4.刺激器开关
 * 1.串口要加一个函数，接受0x数字进行显示的发送
 * 2.关于刺激器的开关
 * 3.范式切换主要难点在MI部分
 * tip.EEG的部分还要注意Idle状态的编写
 * ******/
//要有个按钮开关——范式（这两个按钮在STAND界面）
//初始化——Stand状态——开SSVEP
void Dialog::changeMode(const int& data)
{
    if(fs_State != nullptr)
    {
        State_Command *tmp = fs_State->Operation(data, sendPort, parad, this);
        if(tmp != nullptr)
        {
            delete fs_State;
            fs_State = tmp;
        }

        dsiFFtWDataHandle->uiState = fs_State->nowState;
        //dsiFFtWDataHandle->uiState = 1;

    }
}

//QTimer不能由另外的thread启动或者关闭，所以加下面两个函数
void Dialog::threadC1()
{
    parad->MI_Mode();
}

void Dialog::threadC2()
{
    parad->WalkMode();
}
