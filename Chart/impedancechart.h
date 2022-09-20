#ifndef CHART_IMPEDANCECHART_H
#define CHART_IMPEDANCECHART_H

#include <QWidget>
#include "OpenBCI/Dataform.h"

namespace Ui { class ImpedanceChart; }

namespace ehdu{

class ImpedanceChart: public QWidget{
    Q_OBJECT
public:
    explicit ImpedanceChart(QWidget *parent = nullptr);
    ~ImpedanceChart();
public slots:
    void impedanceQualitySlot(ehdu::ChannelImpedance impData);
signals:
    void resetSignals();
private:
    int impedanceQualityValue(double v);
    Ui::ImpedanceChart *ui;
private slots:
    void onResetButtonClicked();
};

}

#endif // CHART_IMPEDANCECHART_H
