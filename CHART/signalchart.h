#ifndef SIGNALCHART_H
#define SIGNALCHART_H

#include <mutex>
#include <QWidget>
#include "board_shim.h"
#include "CHART/qcustomplot.h"
#include "OpenBCI/Dataform.h"

QT_BEGIN_NAMESPACE
namespace Ui { class signalChart; }
QT_END_NAMESPACE

class SignalChart: public QWidget{
    Q_OBJECT
public:
    explicit SignalChart(QWidget *parent, QList<QString> paintSignal);
    ~SignalChart();
    void chartInit();
public slots:
    void chartAddData(const BrainFlowArray<double, 2> &data);
private:
    QList<QString> paintSignals;
    Ui::signalChart *ui;
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

#endif // SIGNALCHART_H
