#ifndef SIGNALCHART_H
#define SIGNALCHART_H

#include <QWidget>

#include <mutex>

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
    void chartAddData(ehdu::ChannelSignal data);
private slots:
    void mousePress();
    void mouseWheel();
    void selectionChanged();
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
};

#endif // SIGNALCHART_H
