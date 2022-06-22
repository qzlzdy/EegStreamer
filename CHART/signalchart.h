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
    QCPGraph *F3;
    QCPGraph *F4;
    QCPGraph *C3;
    QCPGraph *Cz;
    QCPGraph *C4;
    QCPGraph *P3;
    QCPGraph *Pz;
    QCPGraph *P4;
    double xAxisNum;
};

#endif // SIGNALCHART_H
