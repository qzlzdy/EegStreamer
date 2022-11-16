#ifndef EEGSTREAMER_H
#define EEGSTREAMER_H

#include <array>
#include <string>
#include <QTimer>
#include <QSharedPointer>
#include <QWidget>
#include <board_shim.h>
#include "ThirdParty/qcustomplot/qcustomplot.h"
#include "OpenBCI/cyton.h"
#include "Ssvep/ssvep.h"

namespace Ui { class EegStreamer; }

namespace ehdu{

class EegStreamer: public QWidget{
    Q_OBJECT
public:
    static const QString FREQ_8Hz;
    static const QString FREQ_10Hz;
    static const QString FREQ_12Hz;
    static const QString FREQ_14Hz;
    explicit EegStreamer(QWidget *parent = nullptr);
    ~EegStreamer();
private:
    Ui::EegStreamer *ui;
    Cyton *cyton;
    QTimer *timer;

    std::array<QCustomPlot *, 8> channels;
    QSharedPointer<QCPAxisTickerTime> ticker;
    double startTime;

    Ssvep *ssvep;
    std::string recordFile;
private slots:
    void addChartData(const BrainFlowArray<double, 2> &data);
    void connectCyton();
    void disconnectCyton();
    void loadCsv();
    void recordData(const BrainFlowArray<double, 2> &data);
    void refreshPorts();
    void startRecord();
    void stopRecord();
};

}

#endif // EEGSTREAMER_H
