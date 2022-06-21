#ifndef SIGNALCHART_H
#define SIGNALCHART_H

#include <QWidget>
#include "CHART/qcustomplot.h"
#include "DSIAPI/dataform.h"
#include "mutex"

namespace Ui {
class signalChart;
}

class signalChart : public QWidget
{
    Q_OBJECT

public:
    explicit signalChart(QWidget *parent,QList<QString> paintSignal);
    ~signalChart();
    void chartInit();

    QList<QString> paintSignals;

public slots:
    void chartAddData(channelSignal data);

private slots:
        void mousePress();
        void mouseWheel();
        void selectionChanged();

private:
    Ui::signalChart *ui;
    QCPGraph *graph_Fp1;
    QCPGraph *graph_Fp2;
    QCPGraph *graph_F7;
    QCPGraph *graph_F3;
    QCPGraph *graph_Fz;
    QCPGraph *graph_F4;
    QCPGraph *graph_F8;
    QCPGraph *graph_A1;
    QCPGraph *graph_T3;
    QCPGraph *graph_C3;
    QCPGraph *graph_Cz;
    QCPGraph *graph_C4;
    QCPGraph *graph_T4;
    QCPGraph *graph_A2;
    QCPGraph *graph_T5;
    QCPGraph *graph_P3;
    QCPGraph *graph_Pz;
    QCPGraph *graph_P4;
    QCPGraph *graph_T6;
    QCPGraph *graph_O1;
    QCPGraph *graph_O2;
    QCPGraph *graph_X1;
    QCPGraph *graph_X2;
    QCPGraph *graph_X3;

    double xAxis_num;
};

#endif // SIGNALCHART_H
