#include "eegstreamer.h"
#include "ui_eegstreamer.h"

#include <chrono>
#include <QFile>
#include <QMessageBox>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QVector>
#include <data_filter.h>

using namespace std;
using namespace ehdu;

const QString EegStreamer::FREQ_8Hz = "8.0 Hz";
const QString EegStreamer::FREQ_10Hz = "10.0 Hz";
const QString EegStreamer::FREQ_12Hz = "12.0 Hz";
const QString EegStreamer::FREQ_14Hz = "14.0 Hz";

namespace {

array<QColor, 8> penColors = {
    QColor(255, 0, 0),
    QColor(0, 255, 0),
    QColor(0, 0, 255),
    QColor(255, 255, 0),
    QColor(255, 0, 255),
    QColor(0, 255, 255),
    QColor(127, 0, 0),
    QColor(0, 127, 0)
};

}

EegStreamer::EegStreamer(QWidget *parent):
QWidget(parent), ui(new Ui::EegStreamer){
    ui->setupUi(this);
    cyton = new Cyton(this);
    timer = new QTimer(this);

    // Main Window Initialization
    channels = {ui->ch1, ui->ch2, ui->ch3, ui->ch4,
                ui->ch5, ui->ch6, ui->ch7, ui->ch8};
    ticker = QSharedPointer<QCPAxisTickerTime>(new QCPAxisTickerTime);
    ticker->setTimeFormat("%s.%z");
    for(int c = 0; c < 8; ++c){
        channels[c]->xAxis->setLabel("time (s)");
        channels[c]->xAxis->setRange(0, 10);
        channels[c]->yAxis->setLabel("amp (uV)");
        channels[c]->yAxis->setRange(-187.5, 187.5);
        channels[c]->addGraph()->setPen(QPen(penColors[c]));
        channels[c]->xAxis->setTicker(ticker);
        channels[c]->axisRect()->setupFullAxesBox();
        connect(channels[c]->xAxis, SIGNAL(rangeChanged(QCPRange)),
                channels[c]->xAxis2, SLOT(setRange(QCPRange)));
        channels[c]->replot();
    }
    startTime = 0;
    ui->DisconnectCyton->setDisabled(true);

    // SSVEP Initialization
    ssvep = new Ssvep(this);
    ui->Freqs->addItem(FREQ_8Hz);
    ui->Freqs->addItem(FREQ_10Hz);
    ui->Freqs->addItem(FREQ_12Hz);
    ui->Freqs->addItem(FREQ_14Hz);
    ui->SsvepLayout->insertWidget(0, ssvep);
    ui->StopRecord->setDisabled(true);

    // FFT & PSD Initialization
    ui->FftPlot->xAxis->setLabel("freq (Hz)");
    ui->FftPlot->xAxis->setRange(5, 20);
    ui->FftPlot->yAxis->setLabel("amp");
    ui->FftPlot->yAxis->setRange(0, 1);
    ui->PsdPlot->xAxis->setLabel("freq (Hz)");
    ui->PsdPlot->xAxis->setRange(5, 20);
    ui->PsdPlot->yAxis->setLabel("power");
    ui->PsdPlot->yAxis->setRange(0, 1);
    ui->RangeSel->addItem("1");
    ui->RangeSel->addItem("10");

    // connect signals & slots
    connect(cyton, &Cyton::bufferDataSignal, this, &EegStreamer::addChartData);
    connect(cyton, &Cyton::recordToFile, this, &EegStreamer::recordData);
    connect(timer, &QTimer::timeout, this, &EegStreamer::stopRecord);
    connect(timer, &QTimer::timeout, ssvep, &Ssvep::stopSsvep);
    connect(timer, &QTimer::timeout, this, [](){
        QMessageBox msgBox;
        msgBox.setText("采样结束");
        msgBox.exec();
    });
    connect(ui->RefreshPorts, &QPushButton::clicked,
            this, &EegStreamer::refreshPorts);
    connect(ui->ConnectCyton, &QPushButton::clicked,
            this, &EegStreamer::connectCyton);
    connect(ui->DisconnectCyton, &QPushButton::clicked,
            this, &EegStreamer::disconnectCyton);
    connect(ui->StartRecord, &QPushButton::clicked,
            this, &EegStreamer::startRecord);
    connect(ui->StartRecord, &QPushButton::clicked, this, [&](){
        timer->start(151s);
    });
    connect(ui->StopRecord, &QPushButton::clicked,
            this, &EegStreamer::stopRecord);
    connect(ui->StopRecord, &QPushButton::clicked, ssvep, &Ssvep::stopSsvep);
    connect(ui->LoadCsv, &QPushButton::clicked, this, &EegStreamer::loadCsv);
}

EegStreamer::~EegStreamer(){
    delete ssvep;

    delete timer;
    delete cyton;
    delete ui;
}

void EegStreamer::addChartData(const BrainFlowArray<double, 2> &data){
    if(startTime == 0){
        startTime = data.at(22, 0);
    }
    int size = data.get_size(1);
    double lastTime = data.at(22, size - 1) - startTime;
    double leftBound = lastTime - 10;
    if(leftBound < 0){
        lastTime = 10;
        leftBound = 0;
    }
    for(int c = 0; c < 8; ++c){
        for(int i = 0; i < size; ++i){
            double currentTime = data.at(22, i) - startTime;
            double value = data.at(c + 1, i) * Cyton::SCALE_FACTOR;
            channels[c]->graph(0)->addData(currentTime, value);
        }
        channels[c]->xAxis->setRange(leftBound, lastTime);
        channels[c]->replot();
    }
}

void EegStreamer::connectCyton(){
    try{
        for(int c = 0; c < 8; ++c){
            channels[c]->clearGraphs();
        }
        cyton->startStream();
        cyton->start();
        ui->ConnectCyton->setDisabled(true);
        ui->DisconnectCyton->setDisabled(false);
    }
    catch(const BrainFlowException &err){
        BoardShim::log_message(static_cast<int>(LogLevels::LEVEL_ERROR),
                               err.what());
        QMessageBox::critical(this, "Streaming Error", err.what());
    }
}

void EegStreamer::disconnectCyton(){
    cyton->stopStream();
    ui->ConnectCyton->setDisabled(false);
    ui->DisconnectCyton->setDisabled(true);
}

void EegStreamer::loadCsv(){
    if(ui->LoadFilename->text().isEmpty()){
        QMessageBox::critical(this, "Load Error", "文件名不可为空");
        return;
    }

    QString filename = QString("records/%1").arg(ui->LoadFilename->text());
    if(!QFile::exists(filename)){
        QMessageBox::critical(this, "Load Error", "文件不存在");
        return;
    }

    BrainFlowArray<double, 2> data =
        DataFilter::read_file(filename.toStdString());
    ui->FftPlot->clearGraphs();
    ui->PsdPlot->clearPlottables();
    QString selectedRange = ui->RangeSel->currentText();
    if(selectedRange == "10"){
        ui->FftPlot->yAxis->setRange(0, 10);
        ui->PsdPlot->yAxis->setRange(0, 10);
    }
    else if(selectedRange == "1"){
        ui->FftPlot->yAxis->setRange(0, 1);
        ui->PsdPlot->yAxis->setRange(0, 1);
    }
    bool useLog = ui->UseLog->isChecked();
    for(int c = 0; c < 8; ++c){
        double *head = data.get_address(c + 1) + 30 * Cyton::SAMPLE_RATE;
        int size = 120 * Cyton::SAMPLE_RATE;
        DataFilter::remove_environmental_noise(head, size, Cyton::SAMPLE_RATE,
            static_cast<int>(NoiseTypes::FIFTY));
        DataFilter::perform_bandpass(head, size, Cyton::SAMPLE_RATE, 3, 40, 4,
            static_cast<int>(FilterTypes::BUTTERWORTH), 0);

        // FFT
        int fft_len;
        int N = DataFilter::get_nearest_power_of_two(size) << 1;
        double *padding = new double[N];
        for(int j = 0; j < size; ++j){
            padding[j] = head[j];
        }
        for(int j = size; j < N; ++j){
            padding[j] = 0;
        }
        complex<double> *fft = DataFilter::perform_fft(padding, N,
            static_cast<int>(WindowOperations::HANNING), &fft_len);
        delete[] padding;
        QVector<double> freq(fft_len), amp(fft_len);
        double freq_step = 1.0 * Cyton::SAMPLE_RATE / N;
        for(int i = 0; i < fft_len; ++i){
            freq[i] = freq_step * i;
            amp[i] = abs(fft[i]);
            if(i == 0){
                amp[i] /= N;
            }
            else{
                amp[i] /= N >> 1;
            }
            amp[i] *= Cyton::SCALE_FACTOR;
            if(useLog){
                amp[i] = 10 * log10(amp[i]);
            }
        }
        delete[] fft;
        ui->FftPlot->addGraph()->setData(freq, amp);

        // PSD
        fft_len = DataFilter::get_nearest_power_of_two(4 * Cyton::SAMPLE_RATE);
        int psd_len;
        pair<double *, double *> psd = DataFilter::get_psd_welch(
            head, size, fft_len, fft_len / 2, Cyton::SAMPLE_RATE,
            static_cast<int>(WindowOperations::HANNING), &psd_len);
        QVector<double> band(30), power(30);
        for(int i = 0; i < 30; ++i){
            double f = 0.5 * i + 5;
            band[i] = f;
            try{
                power[i] = DataFilter::get_band_power(
                    psd, psd_len, f - 0.25, f + 0.25);
            }
            catch(const BrainFlowException &err){
                power[i] = 0;
            }
            if(useLog){
                power[i] = 10 * log10(power[i]);
            }
        }
        delete[] psd.first;
        delete[] psd.second;
        QCPBars *bars = new QCPBars(ui->PsdPlot->xAxis, ui->PsdPlot->yAxis);
        bars->setWidth(0.2);
        bars->setData(band, power);
        bars->setPen(QPen(penColors[c]));
    }
    ui->FftPlot->replot();
    ui->PsdPlot->replot();
}

void EegStreamer::recordData(const BrainFlowArray<double, 2> &data){
    DataFilter::write_file(data, recordFile, "a");
}

void EegStreamer::refreshPorts(){
    ui->Ports->clear();
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts()){
        QSerialPort serial;
        serial.setPort(info);
        if(serial.open(QIODevice::ReadWrite)){
            ui->Ports->addItem(serial.portName());
            serial.close();
        }
    }
}

void EegStreamer::startRecord(){
    if(ui->RecordFilename->text().isEmpty()){
        QMessageBox::critical(this, "Record Error", "文件名不可为空");
        return;
    }

    QString filename = QString("records/%1").arg(ui->RecordFilename->text());
    if(QFile::exists(filename)){
        QMessageBox::critical(this, "Record Error", "文件已存在");
        return;
    }
    recordFile = filename.toStdString();

    QString freq = ui->Freqs->currentText();
    chrono::nanoseconds cycle = 1s;
    if(freq == FREQ_8Hz){
        cycle = 62500000ns;
    }
    else if(freq == FREQ_10Hz){
        cycle = 50000000ns;
    }
    else if(freq == FREQ_12Hz){
        cycle = 41666667ns;
    }
    else if(freq == FREQ_14Hz){
        cycle = 35714286ns;
    }
    ssvep->startSsvep(cycle);
    cyton->recordFlag = true;
    ui->StartRecord->setDisabled(true);
    ui->StopRecord->setDisabled(false);
}

void EegStreamer::stopRecord(){
    cyton->recordFlag = false;
    ui->StartRecord->setDisabled(false);
    ui->StopRecord->setDisabled(true);
}
