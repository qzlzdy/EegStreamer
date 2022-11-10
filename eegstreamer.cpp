#include "eegstreamer.h"
#include "ui_eegstreamer.h"

#include <chrono>
#include <QFile>
#include <QMessageBox>
#include <QSerialPort>
#include <QSerialPortInfo>
#include "data_filter.h"

using namespace std;
using namespace ehdu;

const QString EegStreamer::FREQ_8Hz = "8.0 Hz";
const QString EegStreamer::FREQ_11Hz = "11.0 Hz";
const QString EegStreamer::FREQ_13Hz = "13.0 Hz";
const QString EegStreamer::FREQ_15Hz = "15.0 Hz";

EegStreamer::EegStreamer(QWidget *parent):
QWidget(parent), ui(new Ui::EegStreamer){
    ui->setupUi(this);
    cyton = new Cyton(this);

    signalChart = new SignalChart(this);
    ui->MainLayout->addWidget(signalChart);

    ports = new QComboBox(this);
    refreshPorts = new QPushButton("refresh ports", this);
    connectCyton = new QPushButton("connect Cyton", this);
    disconnectCyton = new QPushButton("disconnect Cyton", this);
    disconnectCyton->setDisabled(true);
    buttonsLayout = new QHBoxLayout(this);
    buttonsLayout->setSpacing(10);
    buttonsLayout->setContentsMargins(0, 0, 0, 0);
    buttonsLayout->addWidget(ports);
    buttonsLayout->addWidget(refreshPorts);
    buttonsLayout->addWidget(connectCyton);
    buttonsLayout->addWidget(disconnectCyton);
    ui->MainLayout->addLayout(buttonsLayout);

    ssvep = new Ssvep(this);
    frequencies = new QComboBox(this);
    frequencies->addItem(FREQ_8Hz);
    frequencies->addItem(FREQ_11Hz);
    frequencies->addItem(FREQ_13Hz);
    frequencies->addItem(FREQ_15Hz);
    filenameEdit = new QLineEdit(this);
    startRecord = new QPushButton("start record", this);
    stopRecord = new QPushButton("stop record", this);
    stopRecord->setDisabled(true);
    recordLayout = new QHBoxLayout(this);
    recordLayout->addWidget(frequencies);
    recordLayout->addWidget(filenameEdit);
    recordLayout->addWidget(startRecord);
    recordLayout->addWidget(stopRecord);
    ui->SsvepLayout->addWidget(ssvep);
    ui->SsvepLayout->addLayout(recordLayout);

    timer = new QTimer;

    connect(cyton, &Cyton::bufferDataSignal,
            signalChart, &SignalChart::chartAddData);
    connect(refreshPorts, &QPushButton::clicked, this, [&](){
        ports->clear();
        foreach(const QSerialPortInfo &info,
                QSerialPortInfo::availablePorts()){
            QSerialPort serial;
            serial.setPort(info);
            if(serial.open(QIODevice::ReadWrite)){
                ports->addItem(serial.portName());
                serial.close();
            }
        }
    });
    connect(connectCyton, &QPushButton::clicked,
            this, &EegStreamer::startCyton);
    connect(disconnectCyton, &QPushButton::clicked,
            this, &EegStreamer::stopCyton);
    connect(startRecord, &QPushButton::clicked,
            this, &EegStreamer::startRecordToFile);
    connect(startRecord, &QPushButton::clicked, this, [&](){
        timer->start(151s);
    });
    connect(cyton, &Cyton::recordToFile, this, &EegStreamer::recordData);
    connect(stopRecord, &QPushButton::clicked,
            this, &EegStreamer::stopRecordToFile);
    connect(stopRecord, &QPushButton::clicked, ssvep, &Ssvep::stopSsvep);
    connect(timer, &QTimer::timeout, this, &EegStreamer::stopRecordToFile);
    connect(timer, &QTimer::timeout, ssvep, &Ssvep::stopSsvep);
    connect(timer, &QTimer::timeout, this, [](){
        QMessageBox msgBox;
        msgBox.setText("采样结束");
        msgBox.exec();
    });
}

EegStreamer::~EegStreamer(){
    delete signalChart;

    delete ports;
    delete refreshPorts;
    delete connectCyton;
    delete disconnectCyton;
    delete buttonsLayout;

    delete ssvep;
    delete frequencies;
    delete startRecord;
    delete stopRecord;
    delete recordLayout;

    delete timer;
    delete cyton;
    delete ui;
}

void EegStreamer::startCyton(){
    try{
        cyton->startStream();
        cyton->start();
        connectCyton->setDisabled(true);
        disconnectCyton->setDisabled(false);
    }
    catch(const BrainFlowException &err){
        BoardShim::log_message(static_cast<int>(LogLevels::LEVEL_ERROR),
                               err.what());
        QMessageBox::critical(this, "Streaming Error", err.what());
    }
}

void EegStreamer::stopCyton(){
    cyton->stopStream();
    connectCyton->setDisabled(false);
    disconnectCyton->setDisabled(true);
}

void EegStreamer::startRecordToFile(){
    if(filenameEdit->text().isEmpty()){
        QMessageBox::critical(this, "Record Error", "Filename is empty!");
        return;
    }
    // 判断文件是否存在
    QString filename = QString("record/%1").arg(filenameEdit->text());
    if(QFile::exists(filename)){
        QMessageBox::critical(this, "Record Error", "File already exists!");
        return;
    }
    recordFile = filename.toStdString();
    // 告诉线程可以开始记录了
    QString freq = frequencies->currentText();
    chrono::nanoseconds cycle = 1s;
    if(freq == FREQ_8Hz){
        cycle = 62500000ns;
    }
    else if(freq == FREQ_11Hz){
        cycle = 45454545ns;
    }
    else if(freq == FREQ_13Hz){
        cycle = 38461538ns;
    }
    else if(freq == FREQ_15Hz){
        cycle = 33333333ns;
    }
    ssvep->startSsvep(cycle);
    cyton->recordFlag = true;
    startRecord->setDisabled(true);
    stopRecord->setDisabled(false);
}

void EegStreamer::stopRecordToFile(){
    cyton->recordFlag = false;
    startRecord->setDisabled(false);
    stopRecord->setDisabled(true);
}

void EegStreamer::recordData(const BrainFlowArray<double, 2> &data){
    DataFilter::write_file(data, recordFile, "a");
}
