#include "eegstreamer.h"
#include "ui_eegstreamer.h"

#include <algorithm>
#include <chrono>
#include <QFile>
#include <QLogValueAxis>
#include <QMessageBox>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QValueAxis>
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

    array<QChartView *, 8> views = {
        ui->ch1, ui->ch2, ui->ch3, ui->ch4,
        ui->ch5, ui->ch6, ui->ch7, ui->ch8
    };
    // Main Window Initialization
    for(int c = 0; c < 8; ++c){
        initTimeSeries(channels[c], timeSeries[c]);
        timeSeries[c]->setColor(penColors[c]);
        views[c]->setChart(channels[c]);
        views[c]->setRenderHint(QPainter::Antialiasing);
        buffers[c].reserve(2500);
        for(int i = 0; i < 2500; ++i){
            buffers[c] << QPointF(4 * i, 0);
        }
    }
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
    initPlotChart(fftChart, fftSeries);
    ui->FftPlot->setChart(fftChart);
    ui->FftPlot->setRenderHint(QPainter::Antialiasing);

    initPlotChart(psdChart, psdSeries);
    ui->PsdPlot->setChart(psdChart);
    ui->PsdPlot->setRenderHint(QPainter::Antialiasing);

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
    connect(ui->RefreshPorts, &QPushButton::clicked, this, &EegStreamer::refreshPorts);
    connect(ui->ConnectCyton, &QPushButton::clicked, this, &EegStreamer::connectCyton);
    connect(ui->DisconnectCyton, &QPushButton::clicked, this, &EegStreamer::disconnectCyton);
    connect(ui->StartRecord, &QPushButton::clicked, this, &EegStreamer::startRecord);
    connect(ui->StartRecord, &QPushButton::clicked, this, [&](){
        timer->start(151s);
    });
    connect(ui->StopRecord, &QPushButton::clicked, this, &EegStreamer::stopRecord);
    connect(ui->StopRecord, &QPushButton::clicked, ssvep, &Ssvep::stopSsvep);
    connect(ui->LoadCsv, &QPushButton::clicked, this, &EegStreamer::loadCsv);
}

EegStreamer::~EegStreamer(){
    delete psdChart;
    delete fftChart;
    delete ssvep;
    for_each(channels.begin(), channels.end(), [](QChart *&p){
        delete p;
    });

    delete timer;
    delete cyton;
    delete ui;
}

void EegStreamer::initTimeSeries(QChart *&chart, QLineSeries *&series){
    chart = new QChart;
    series = new QLineSeries;
    series->setUseOpenGL();
    chart->addSeries(series);
    QValueAxis *xAxis = new QValueAxis;
    xAxis->setRange(0, 10000);
    chart->addAxis(xAxis, Qt::AlignBottom);
    series->attachAxis(xAxis);
    QValueAxis *yAxis = new QValueAxis;
    yAxis->setRange(-187.5, 187.5);
    chart->addAxis(yAxis, Qt::AlignLeft);
    series->attachAxis(yAxis);
    chart->legend()->hide();
}

void EegStreamer::initPlotChart(QChart *&chart, array<QLineSeries *, 8> &series){
    chart = new QChart;
    for_each(series.begin(), series.end(), [&](QLineSeries *&p){
        p = new QLineSeries;
        p->setUseOpenGL();
        chart->addSeries(p);
    });
    QValueAxis *xAxis = new QValueAxis;
    xAxis->setRange(6, 20);
    xAxis->setTickCount(8);
    chart->addAxis(xAxis, Qt::AlignBottom);
    for_each(series.begin(), series.end(), [&](QLineSeries *p){
        p->attachAxis(xAxis);
    });
    QLogValueAxis *yAxis = new QLogValueAxis;
    yAxis->setBase(10);
    yAxis->setRange(0.1, 10);
    yAxis->setMinorTickCount(10);
    chart->addAxis(yAxis, Qt::AlignLeft);
    for_each(series.begin(), series.end(), [&](QLineSeries *p){
        p->attachAxis(yAxis);
    });
    chart->legend()->hide();
}

void EegStreamer::addChartData(const BrainFlowArray<double, 2> &data){
    int availableSamples = data.get_size(1);
    int start = 0;
    if(availableSamples < 2500){
        start = 2500 - availableSamples;
        for(int c = 0; c < 8; ++c){
            for(int s = 0; s < start; ++s){
                buffers[c][s].setY(buffers[c].at(s + availableSamples).y());
            }
        }
    }
    for(int c = 0; c < 8; ++c){
        for(int s = start; s < 2500; ++s){
            double value = data.at(c + 1, s - start) * Cyton::SCALE_FACTOR;
//            value *= 0.1;
            buffers[c][s].setY(value);
        }
        timeSeries[c]->replace(buffers[c]);
    }
}

void EegStreamer::connectCyton(){
    try{
        cyton->startStream();
        cyton->start();
        ui->ConnectCyton->setDisabled(true);
        ui->DisconnectCyton->setDisabled(false);
    }
    catch(const BrainFlowException &err){
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

    BrainFlowArray<double, 2> data = DataFilter::read_file(filename.toStdString());
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
        QList<QPointF> buffer;
        double freq_step = 1.0 * Cyton::SAMPLE_RATE / N;
        for(int i = 0; i < fft_len; ++i){
            double freq = freq_step * i;
            double amp = abs(fft[i]);
            if(i == 0){
                amp /= N;
            }
            else{
                amp /= N >> 1;
            }
//            amp *= Cyton::SCALE_FACTOR;
            buffer << QPointF(freq, amp);
        }
        delete[] fft;
        fftSeries[c]->replace(buffer);

        // PSD
        fft_len = DataFilter::get_nearest_power_of_two(4 * Cyton::SAMPLE_RATE);
        int psd_len;
        pair<double *, double *> psd = DataFilter::get_psd_welch(
            head, size, fft_len, fft_len / 2, Cyton::SAMPLE_RATE,
            static_cast<int>(WindowOperations::HANNING), &psd_len);
        buffer.clear();
        for(int i = 0; i < 30; ++i){
            double band = 0.5 * i + 5;
            double power;
            try{
                power = DataFilter::get_band_power(psd, psd_len, band - 0.25, band + 0.25);
            }
            catch(const BrainFlowException &err){
                power = 0;
            }
            buffer << QPointF(band, power);
        }
        delete[] psd.first;
        delete[] psd.second;
        psdSeries[c]->replace(buffer);
    }
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