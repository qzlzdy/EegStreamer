#ifndef IMPEDANCECHART_H
#define IMPEDANCECHART_H

#include <QWidget>

#include "OpenBCI/Dataform.h"

using namespace ehdu;

QT_BEGIN_NAMESPACE
namespace Ui { class impedancechart; }
QT_END_NAMESPACE

class impedancechart: public QWidget{
    Q_OBJECT
public:
    explicit impedancechart(QWidget *parent = 0);
    ~impedancechart();
public slots:
    void impedanceQualitySlot(ehdu::ChannelImpedance impData);
signals:
    void resetSignals();
private:
    Ui::impedancechart *ui;
    int impedanceQualityValue(double v);
private slots:
    void onResetButtonClicked();
};

#endif // IMPEDANCECHART_H
