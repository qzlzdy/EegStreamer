#ifndef IMPEDANCECHART_H
#define IMPEDANCECHART_H

#include <QWidget>
#include <DSIAPI/dataform.h>

namespace Ui {
class impedancechart;
}

class impedancechart : public QWidget
{
    Q_OBJECT

public:
    explicit impedancechart(QWidget *parent = 0);
    ~impedancechart();

private:
    Ui::impedancechart *ui;
    int impedanceQualityValue(double v);

public slots:
    void impedanceQualitySlot(channelImpedance impData);

private slots:
    void on_resetButton_clicked();

signals:
    void resetSignals();
};

#endif // IMPEDANCECHART_H
