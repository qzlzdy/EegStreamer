#include "signalchart.h"
#include "ui_signalchart.h"

#include <QPen>

using namespace ehdu;

SignalChart::SignalChart(QWidget *parent):
QWidget(parent), ui(new Ui::SignalChart){
    ui->setupUi(this);
    ticker = QSharedPointer<QCPAxisTickerTime>(new QCPAxisTickerTime);
    ticker->setTimeFormat("%s.%z");
    initChannelChart(ui->ch1, QColor(255, 0, 0));
    initChannelChart(ui->ch2, QColor(0, 255, 0));
    initChannelChart(ui->ch3, QColor(0, 0, 255));
    initChannelChart(ui->ch4, QColor(255, 255, 0));
    initChannelChart(ui->ch5, QColor(255, 0, 255));
    initChannelChart(ui->ch6, QColor(0, 255, 255));
    initChannelChart(ui->ch7, QColor(128, 0, 0));
    initChannelChart(ui->ch8, QColor(0, 128, 0));
    startTime = 0;
}

SignalChart::~SignalChart(){
    delete ui;
}
#include <iostream>
using namespace std;
//数据动态更新槽
void SignalChart::chartAddData(const BrainFlowArray<double, 2> &data){
    if(startTime == 0){
        startTime = data.at(22, 0);
    }
    int size = data.get_size(1);
    for(int i = 0; i < size; ++i){
        double currentTime = data.at(22, i) - startTime;
        ui->ch1->graph(0)->addData(currentTime, data.at(1, i) * 0.02235);
        ui->ch2->graph(0)->addData(currentTime, data.at(2, i) * 0.02235);
        ui->ch3->graph(0)->addData(currentTime, data.at(3, i) * 0.02235);
        ui->ch4->graph(0)->addData(currentTime, data.at(4, i) * 0.02235);
        ui->ch5->graph(0)->addData(currentTime, data.at(5, i) * 0.02235);
        ui->ch6->graph(0)->addData(currentTime, data.at(6, i) * 0.02235);
        ui->ch7->graph(0)->addData(currentTime, data.at(7, i) * 0.02235);
        ui->ch8->graph(0)->addData(currentTime, data.at(8, i) * 0.02235);
    }

    double lastTime = data.at(22, size - 1) - startTime;
    double leftBound = lastTime - 10;
    if(leftBound < 0){
        lastTime = 10;
        leftBound = 0;
    }
    ui->ch1->xAxis->setRange(leftBound, lastTime);
    ui->ch2->xAxis->setRange(leftBound, lastTime);
    ui->ch3->xAxis->setRange(leftBound, lastTime);
    ui->ch4->xAxis->setRange(leftBound, lastTime);
    ui->ch5->xAxis->setRange(leftBound, lastTime);
    ui->ch6->xAxis->setRange(leftBound, lastTime);
    ui->ch7->xAxis->setRange(leftBound, lastTime);
    ui->ch8->xAxis->setRange(leftBound, lastTime);

    ui->ch1->replot();
    ui->ch2->replot();
    ui->ch3->replot();
    ui->ch4->replot();
    ui->ch5->replot();
    ui->ch6->replot();
    ui->ch7->replot();
    ui->ch8->replot();
}

void SignalChart::initChannelChart(
        QCustomPlot *customPlot, const QColor &color){
    customPlot->addGraph()->setPen(QPen(color));
    customPlot->xAxis->setTicker(ticker);
    customPlot->axisRect()->setupFullAxesBox();
    customPlot->yAxis->setRange(-187.5, 187.5);
    connect(customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)),
            customPlot->xAxis2, SLOT(setRange(QCPRange)));
    customPlot->replot();
}
