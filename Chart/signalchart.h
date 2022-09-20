#ifndef CHART_SIGNALCHART_H
#define CHART_SIGNALCHART_H

#include <QWidget>
#include "board_shim.h"
#include "OpenBCI/Dataform.h"
#include "qcustomplot.h"

namespace Ui { class SignalChart; }

namespace ehdu{

class SignalChart: public QWidget{
    Q_OBJECT
public:
    explicit SignalChart(QWidget *parent, QList<QString> paintSignal);
    ~SignalChart();
public slots:
    void chartAddData(const BrainFlowArray<double, 2> &data);
private:
    QList<QString> paintSignals;
    Ui::SignalChart *ui;
    QCPGraph *Fp1;
    QCPGraph *Fp2;
    QCPGraph *C3;
    QCPGraph *C4;
    QCPGraph *P7;
    QCPGraph *P8;
    QCPGraph *O1;
    QCPGraph *O2;
    double xAxisNum;
private slots:
    void mousePress();
    void mouseWheel();
    void selectionChanged();
};

}

#endif // CHART_SIGNALCHART_H
