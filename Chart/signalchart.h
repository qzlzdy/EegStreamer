#ifndef CHART_SIGNALCHART_H
#define CHART_SIGNALCHART_H

#include <QColor>
#include <QSharedPointer>
#include <QWidget>
#include "board_shim.h"
#include "qcustomplot.h"

namespace Ui { class SignalChart; }

namespace ehdu{

class SignalChart: public QWidget{
    Q_OBJECT
public:
    explicit SignalChart(QWidget *parent);
    ~SignalChart();
public slots:
    void chartAddData(const BrainFlowArray<double, 2> &data);
private:
    void initChannelChart(QCustomPlot *customPlot, const QColor &color);
    Ui::SignalChart *ui;
    QSharedPointer<QCPAxisTickerTime> ticker;
    double startTime;
};

}

#endif // CHART_SIGNALCHART_H
