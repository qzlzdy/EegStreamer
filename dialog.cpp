#include "dialog.h"
#include "ui_dialog.h"

#include <QMetaType>

#include "OpenBCI/StateCommand.h"

using namespace ehdu;

Dialog::Dialog(QWidget *parent): QDialog(parent), ui(new Ui::Dialog){
    ui->setupUi(this);
    //声明信号变量channelSignal
    qRegisterMetaType<ChannelSignal>("ChannelSignal");
    //加载QSS
    loadQssSlot("Style");
    //初始化对象
    chartWidget = nullptr;
    impedanceWidget = nullptr;
    cytonFftwDataHandler = nullptr;
    recordWidget = nullptr;
    sendPort = nullptr;
    portThread = nullptr;
    parad = nullptr;
    cspTrain = nullptr;

    initialize();
    //初始化信号与槽
    connetSlots();
}

Dialog::~Dialog(){
    portThread->quit();
    portThread->wait();
    delete portThread;
    delete dataRecv;
    delete chartWidget;
    delete impedanceWidget;
    delete recordWidget;
    delete sendPort;
    if(cytonFftwDataHandler != nullptr){
        delete cytonFftwDataHandler;
    }
    delete startButton;
    delete stopButton;
    delete trainBox;
    delete cspTrain;
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

/**
 * @brief 信号与槽连接
 */
void Dialog::connetSlots(){
    //chart数据更新
    connect(dataRecv, &Cyton::bufferDataSignal,
            chartWidget, &SignalChart::chartAddData);
    //impedance数据更新
//    connect(dataRecv, &Cyton::fixedTimeSignal,
//            impedanceWidght, &impedancechart::impedanceQualitySlot);
    //reset
    connect(impedanceWidget, &impedancechart::resetSignals,
            dataRecv, &Cyton::reset);
    //start
    connect(startButton, &QPushButton::clicked, this, &Dialog::cytonStart);
    //stop
    connect(stopButton, &QPushButton::clicked, this, &Dialog::cytonStop);
    //start FFTW
    connect(fftwStartButton, &QPushButton::clicked, this, &Dialog::fftwStart);
    //stop FFTW
    connect(fftwStopButton, &QPushButton::clicked, this, &Dialog::fftwStop);
    //start/stop record
    connect(recordWidget, &saveEEG::isTimeToRecord, this, [&](bool flag){
        if(flag){
            connect(dataRecv, &Cyton::offlineDataSignal,
                    recordWidget, &saveEEG::recordEEGslots);
        }
        else{
            disconnect(dataRecv, &Cyton::offlineDataSignal,
                       recordWidget, &saveEEG::recordEEGslots);
        }
        dataRecv->recordSwitchFlag = flag;
    });
    searchButton->setDisabled(true);
//    connect(searchButton, &QPushButton::clicked, this, [&](){
//        //find com
//        if(searchButton->text() == "Search Port"){
//            searchButton->setDisabled(true);
//            sendPort->findUsingPort();
//        //open com
//        }
//        else if(searchButton->text() == "Connect"){
//            sendPort->connectComPort(portBox->currentText());
//            searchButton->setText("Disconnect");
//        //close com
//        }
//        else if(searchButton->text() == "Disconnect"){
//            sendPort->closePort();
//            searchButton->setText("Search Port");
//        }
//    });
    //add com
    connect(sendPort, &rehabilitativeUsart::usingComPortSignal,
            this, [&](QVector<QString> data){
        for(QString &portName: data){
            portBox->clear();
            portBox->addItem(portName);
        }
        searchButton->setText("Connect");
        searchButton->setDisabled(false);
    });
    //video
    connect(ui->rightButton, SIGNAL(clicked()), this, SLOT(startVideoRight()));
    connect(ui->leftButton, SIGNAL(clicked()), this, SLOT(startVideoLeft()));

    connect(trainBox, &QComboBox::currentTextChanged, this, [&](QString data){
        if(data=="CSP and CCA.nomarl"){
            startTrainButton->setEnabled(true);
        }
        else if(data=="CSP and CCA.SW"){
            startTrainButton->setEnabled(true);
        }
        else if(data=="CSP and KCCA.nomarl"){
            startTrainButton->setEnabled(true);
        }
        else if(data=="CSP and KCCA.SW"){
            startTrainButton->setEnabled(true);
        }
        else if(data=="FFTW.SW"){
            startTrainButton->setEnabled(false);
        }
    });

    connect(startTrainButton, &QPushButton::clicked, this, [&](){
        startTrainButton->setEnabled(false);
        if(cspTrain != nullptr){
            delete cspTrain;
        }
        cspTrain = new CspTrain;

        connect(cspTrain, &CspTrain::finishTrain,
                this, [&](bool flag){
            startTrainButton->setEnabled(true);
            if(flag == 0){
                QMessageBox::about(this, "Cyton", "Train File Open Failed!");
            }
            else{
                QMessageBox::about(this, "Cyton", "Finish Train!");
            }
        });

        cspTrain->filename = recordWidget->lineeditdata();
        cspTrain->start();
    });
}

/**
 * @brief 初始化对象
 */
void Dialog::initialize(){
    dataRecv = new Cyton(nullptr);
    if(chartWidget == nullptr || impedanceWidget == nullptr
                              || recordWidget == nullptr){
        //channelChart
        chartWidget = new SignalChart(nullptr, dataRecv->chooseChannelString);
        chartWidget->chartInit();
        //impedanceChart
        impedanceWidget = new impedancechart;
        //saveEEG
        recordWidget = new saveEEG(this, dataRecv->chooseChannelString,
                                   "" /*FIXME DSI_REFERENCE_DATA*/);
        recordWidget->startRecordButton(false);
        recordWidget->stopRecordButton(false);
    }
    //port
    sendPort = new rehabilitativeUsart;
    portThread = new QThread;
    sendPort->moveToThread(portThread);
    portThread->start();

    //实例化控件
    startButton = new QPushButton("Start Cyton");
    stopButton = new QPushButton("Stop Cyton");
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
    buttonLayout->addWidget(recordWidget);
    buttonLayout->addWidget(startButton);
    buttonLayout->addWidget(stopButton);
    buttonLayout->setSpacing(10);
    buttonLayout->setContentsMargins(0,0,0,0);

    widghtLayout->addWidget(impedanceWidget);
    widghtLayout->addStretch();
    widghtLayout->addWidget(chartWidget);
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
    QString vedioPath = QString("%1%2").arg(qApp->applicationDirPath(),
                                            QString("/video/left.avi"));
    QString vedioPath2 = QString("%1%2").arg(qApp->applicationDirPath(),
                                             QString("/video/right.avi"));
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
    connect(controls, &PlayerControls::changeRate,
            player, &QMediaPlayer::setPlaybackRate);
    connect(controls, &PlayerControls::stop,
            videoWidget, QOverload<>::of(&QVideoWidget::update));
    connect(player, &QMediaPlayer::playbackStateChanged,
            controls, &PlayerControls::setState);

    //fan_shi
    parad = new Paradigm;
    fanshiLayout = new QHBoxLayout;
    fanshiLayout->addWidget(parad);
    fanshiLayout->setSpacing(10);
    fanshiLayout->setContentsMargins(0,0,0,0);
    ui->Paradigm->setLayout(fanshiLayout);

    fs_State = nullptr;
    connect(parad, &Paradigm::closeSignal, this, [&](){fftwStop();});
}

/**
 * @brief 加载QSS
 */
void Dialog::loadQssSlot(QString name){
    QFile file(QString("://QSS/%1.qss").arg(name));
    file.open(QFile::ReadOnly);
    QString qss = QLatin1String(file.readAll());
    qApp->setStyleSheet(qss);
}

void Dialog::cytonStart(){
    if(dataRecv == nullptr){
        dataRecv = new Cyton(nullptr);
        connect(dataRecv, &Cyton::bufferDataSignal,
                chartWidget, &SignalChart::chartAddData);
//        connect(dataRecv, &Cyton::fixedTimeSignal,
//                impedanceWidght, &impedancechart::impedanceQualitySlot);
        connect(impedanceWidget, &impedancechart::resetSignals,
                dataRecv, &Cyton::reset);
    }
    try{
        dataRecv->startStream();
        dataRecv->start();
        startButton->setEnabled(false);
        stopButton->setEnabled(true);
        fftwStartButton->setEnabled(true);
        fftwStopButton->setEnabled(false);
        recordWidget->startRecordButton(true);
        recordWidget->stopRecordButton(false);
        startTrainButton->setEnabled(true);
        if(trainBox->currentText()!="FFTW.SW"){
            startTrainButton->setEnabled(true);
        }
        trainBox->setEnabled(true);
    }
    catch(const BrainFlowException &err){
        BoardShim::log_message(static_cast<int>(LogLevels::LEVEL_ERROR),
                               err.what());
        QMessageBox::about(this, "Cyton", err.what());
    }
}

void Dialog::cytonStop(){
    startButton->setEnabled(true);
    stopButton->setEnabled(false);
    fftwStartButton->setEnabled(false);
    fftwStopButton->setEnabled(false);
    recordWidget->startRecordButton(false);
    recordWidget->stopRecordButton(false);
    startTrainButton->setEnabled(false);
    trainBox->setEnabled(false);
    if(cytonFftwDataHandler != nullptr){
        cytonFftwDataHandler->readingFlag = false;
        dataRecv->fftwSwitchFlag = false;
        cytonFftwDataHandler->deleteLater();
        cytonFftwDataHandler = nullptr;
    }
    //终止线程，关闭
    dataRecv->stopStream();
    //imp复原
    ChannelImpedance impData;
    impedanceWidget->impedanceQualitySlot(impData);
}

/**
 * @brief 打开FFTW
 *
 * ===================算法入口===================
 */
void Dialog::fftwStart(){
    if(trainBox->currentText() == "CSP and CCA.nomarl"){
        if(cspTrain == nullptr){
            QMessageBox::about(this, "Cyton", "Train First!");
            return;
        }
        cytonFftwDataHandler = new CytonFftw(new CspCca(
            cspTrain->W1, cspTrain->sample1, cspTrain->W2,
            cspTrain->sample2, cspTrain->W3, cspTrain->sample3));
        cytonFftwDataHandler->useName = recordWidget->lineeditdata();
        cytonFftwDataHandler->recordInit();
    }
    else if(trainBox->currentText() == "CSP and CCA.SW"){
        if(cspTrain == nullptr){
            QMessageBox::about(this, "Cyton", "Train First!");
            return;
        }
        cytonFftwDataHandler = new CytonFftw(new SwCspCca(
            cspTrain->W1, cspTrain->sample1, cspTrain->W2,
            cspTrain->sample2, cspTrain->W3, cspTrain->sample3));
        cytonFftwDataHandler->useName = recordWidget->lineeditdata();
        cytonFftwDataHandler->recordInit();

        /**********************以下为新增**********************/
    }
    else if(trainBox->currentText()=="CSP and KCCA.nomarl"){
        if(cspTrain == nullptr){
            QMessageBox::about(this, "Cyton", "Train First!");
            return;
        }
        cytonFftwDataHandler = new CytonFftw(new CspKcca(
            cspTrain->W1, cspTrain->sample1, cspTrain->W2,
            cspTrain->sample2, cspTrain->W3, cspTrain->sample3));
        cytonFftwDataHandler->useName = recordWidget->lineeditdata();
        cytonFftwDataHandler->recordInit();
    }
    else if(trainBox->currentText()=="CSP and KCCA.SW"){
        if(cspTrain == nullptr){
            QMessageBox::about(this, "Cyton", "Train First!");
            return;
        }
        cytonFftwDataHandler = new CytonFftw(new SwCspKcca(
            cspTrain->W1, cspTrain->sample1, cspTrain->W2,
            cspTrain->sample2, cspTrain->W3, cspTrain->sample3));
        cytonFftwDataHandler->useName = recordWidget->lineeditdata();
        cytonFftwDataHandler->recordInit();
        /**********************以上为新增**********************/

    }
    else if(trainBox->currentText() == "FFTW.SW"){
        cytonFftwDataHandler = new CytonFftw(new Fftw());
        cytonFftwDataHandler->useName = recordWidget->lineeditdata();
        cytonFftwDataHandler->recordInit();
    }

    cytonFftwDataHandler->uiState = 0;
    dataRecv->fftwSwitchFlag = true;
    cytonFftwDataHandler->start();
    fftwStartButton->setEnabled(false);
    fftwStopButton->setEnabled(true);
    startTrainButton->setEnabled(false);
    trainBox->setEnabled(false);

    connect(cytonFftwDataHandler, &CytonFftw::leftSignal,
            this, [&](){changeMode(MI_LEFT);});
    connect(cytonFftwDataHandler, &CytonFftw::rightSignal,
            this, [&](){changeMode(MI_RIGHT);});
    connect(cytonFftwDataHandler, &CytonFftw::idleSignal,
            this, [&](){changeMode(MI_IDLE);});
    ui->tabWidget->setCurrentIndex(2);
    fs_State = new StandMode;
    parad->StandMode();
}

/**
 * @brief 关闭FFTW
 */
void Dialog::fftwStop(){
    if(fs_State == nullptr){
        return;
    }

    delete fs_State;
    fs_State = nullptr;

    cytonFftwDataHandler->readingFlag = false;
    dataRecv->fftwSwitchFlag = false;
    cytonFftwDataHandler->deleteLater();
    cytonFftwDataHandler = nullptr;
    fftwStartButton->setEnabled(true);
    fftwStopButton->setEnabled(false);
    if(trainBox->currentText()!= "FFTW.SW"){
        startTrainButton->setEnabled(true);
    }
    trainBox->setEnabled(true);

    ui->tabWidget->setCurrentIndex(0);
}

/**
 * @brief 打开right视频
 */
void Dialog::startVideoRight(){
    medialist->setCurrentIndex(1);
    videoWidget->show();
    player->play();
}

/**
 * @brief 打开left视频
 */
void Dialog::startVideoLeft(){
    medialist->setCurrentIndex(0);
    videoWidget->show();
    player->play();
}

void Dialog::printToLable(QString data){
    ui->label->setText(data);
}

void Dialog::toSSVEP(){
    ui->tabWidget->setCurrentIndex(2);
}

void Dialog::toMI(){
    ui->label->clear();
    ui->tabWidget->setCurrentIndex(1);
}

/**
 * @brief 范式状态切换
 *
 * 拟写一个线程：在线程中将缓存队列中存储的状态取出进行状态机判断
 * 判断过后主要工作：1.串口发送指令 2.进行状态机切换 3.范式切换 4.刺激器开关
 * 1.串口要加一个函数，接受0x数字进行显示的发送
 * 2.关于刺激器的开关
 * 3.范式切换主要难点在MI部分
 * tip.EEG的部分还要注意Idle状态的编写
 *
 * 要有个按钮开关——范式(这两个按钮在STAND界面)
 * 初始化——Stand状态——开SSVEP
 */
void Dialog::changeMode(const int &data){
    if(fs_State != nullptr){
        StateCommand *tmp = fs_State->Operation(data, sendPort, parad, this);
        if(tmp != nullptr){
            delete fs_State;
            fs_State = tmp;
        }

        cytonFftwDataHandler->uiState = fs_State->nowState;
    }
}

//QTimer不能由另外的thread启动或者关闭，所以加下面两个函数
void Dialog::threadC1(){
    parad->MI_Mode();
}

void Dialog::threadC2(){
    parad->WalkMode();
}
