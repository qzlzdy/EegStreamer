#include "eegstreamer.h"
#include "ui_eegstreamer.h"

#include <algorithm>
#include <chrono>
#include <QFile>
#include <QFont>
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

    // Main Window Initialization
    array<QGraphicsView *, 8> views = {
        ui->ch1, ui->ch2, ui->ch3, ui->ch4,
        ui->ch5, ui->ch6, ui->ch7, ui->ch8
    };
    for(int c = 0; c < 8; ++c){
        initTimeSeries(channels[c], timeSeries[c]);
        timeSeries[c]->setColor(penColors[c]);
        scenes[c] = new QGraphicsScene(this);
        scenes[c]->addItem(channels[c]);
        railed[c] = scenes[c]->addText("Railed: 0%");
        railed[c]->setDefaultTextColor(QColor(0, 0, 0));
        railed[c]->setPos(150, 20);
        views[c]->setScene(scenes[c]);
        views[c]->setRenderHint(QPainter::Antialiasing);
        views[c]->show();
        buffers[c].reserve(2500);
        for(int i = 0; i < 2500; ++i){
            buffers[c] << QPointF(4 * i, 0);
        }
    }
    sampleCount = 0;
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
    connect(timer, &QTimer::timeout, this, [&](){
        QMessageBox::information(this, "提示", "采样结束");
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
    for_each(scenes.begin(), scenes.end(), [](QGraphicsScene *&p){
        delete p;
    });

    delete timer;
    delete cyton;
    delete ui;
}

void EegStreamer::initTimeSeries(QChart *&chart, QLineSeries *&series){
    chart = new QChart;
    chart->setMinimumSize(1190, 290);
    chart->legend()->hide();
    series = new QLineSeries(this);
    series->setUseOpenGL();
    chart->addSeries(series);
    QValueAxis *xAxis = new QValueAxis(this);
    xAxis->setRange(0, 10000);
    chart->addAxis(xAxis, Qt::AlignBottom);
    series->attachAxis(xAxis);
    QValueAxis *yAxis = new QValueAxis(this);
    yAxis->setRange(-187.5, 187.5);
    chart->addAxis(yAxis, Qt::AlignLeft);
    series->attachAxis(yAxis);
}

void EegStreamer::initPlotChart(QChart *&chart, array<QLineSeries *, 8> &series){
    chart = new QChart;
    for(int c = 0; c < 8; ++c){
        series[c] = new QLineSeries(this);
        series[c]->setColor(penColors[c]);
        series[c]->setUseOpenGL();
        chart->addSeries(series[c]);
    }

    QFont font("思源宋体");
    QPen pen;
    series[0]->setColor(QColorConstants::Black);
    pen = series[0]->pen();
    pen.setWidthF(1.667);
    series[0]->setPen(pen);
    QValueAxis *xAxis = new QValueAxis(this);
    xAxis->setRange(6, 16);
    xAxis->setTickCount(6);
    xAxis->setMinorTickCount(1);
    xAxis->setLabelFormat("%d");
    xAxis->setLabelsFont(font);
    xAxis->setTitleText("频率（Hz）");
    xAxis->setTitleFont(font);
    chart->addAxis(xAxis, Qt::AlignBottom);
    pen = xAxis->linePen();
    pen.setWidthF(1.667);
    xAxis->setLinePen(pen);
    xAxis->setLinePenColor(QColorConstants::Black);
    pen = xAxis->gridLinePen();
    pen.setWidth(1);
    xAxis->setGridLinePen(pen);
    for_each(series.begin(), series.end(), [&](QLineSeries *p){
        p->attachAxis(xAxis);
    });

    QLogValueAxis *yAxis = new QLogValueAxis(this);
    yAxis->setBase(10);
    yAxis->setRange(0.01, 1);
    yAxis->setMinorTickCount(5);
    yAxis->setLabelsFont(font);
    yAxis->setTitleText("功率谱密度（&mu;V<sup>2</sup>/Hz）");
    yAxis->setTitleFont(font);
    chart->addAxis(yAxis, Qt::AlignLeft);
    pen = yAxis->linePen();
    pen.setWidthF(1.667);
    yAxis->setLinePen(pen);
    yAxis->setLinePenColor(QColorConstants::Black);
    pen = yAxis->gridLinePen();
    pen.setWidth(1);
    yAxis->setGridLinePen(pen);
    pen = yAxis->minorGridLinePen();
    pen.setWidth(1);
    yAxis->setMinorGridLinePen(pen);
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
            double value = data.at(c + 1, s - start) * 0.001;
            buffers[c][s].setY(value);
        }
        timeSeries[c]->replace(buffers[c]);
    }
    sampleCount += availableSamples;
    if(sampleCount >= 250){
        sampleCount %= 250;
        for(int c = 0; c < 8; ++c){
            array<double, 1000> head;
            transform(buffers[c].constData() + 1500, buffers[c].constData() + 2500,
                      head.begin(), [](const QPointF &p){
                return p.y();
            });
            double rail = DataFilter::get_railed_percentage(head.data(), 1000, 24);
            railed[c]->setPlainText(QString("Railed: %1%").arg(rail));
        }
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
        DataFilter::perform_bandpass(head, size, Cyton::SAMPLE_RATE, 5, 20, 4,
            static_cast<int>(FilterTypes::BUTTERWORTH), 0);

        // FFT
        int offset = 30 * Cyton::SAMPLE_RATE;
        size = 4 * Cyton::SAMPLE_RATE;
        int fft_len;
        complex<double> *fft = DataFilter::perform_fft(head + offset, size,
            static_cast<int>(WindowOperations::HANNING), &fft_len);
        QList<QPointF> buffer;
        double freq_step = 1.0 * Cyton::SAMPLE_RATE / size;
        for(int i = 0; i < fft_len; ++i){
            double freq = freq_step * i;
            double amp = abs(fft[i]);
            if(i == 0){
                amp /= size;
            }
            else{
                amp /= size >> 1;
            }
            buffer << QPointF(freq, amp);
        }
        delete[] fft;
        fftSeries[c]->replace(buffer);

        // PSD
        offset = 20 * Cyton::SAMPLE_RATE;
        size = 60 * Cyton::SAMPLE_RATE;
        fft_len = DataFilter::get_nearest_power_of_two(4 * Cyton::SAMPLE_RATE);
        int psd_len;
        pair<double *, double *> psd = DataFilter::get_psd_welch(
            head + offset, size, fft_len, fft_len / 2, Cyton::SAMPLE_RATE,
            static_cast<int>(WindowOperations::HANNING), &psd_len);
        buffer.clear();
        for(int i = 0; i < 25; ++i){
            double band = 0.5 * i + 5;
            double power;
            try{
                power = DataFilter::get_band_power(psd, psd_len, band - 0.25,
                                                   band + 0.25);
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
