#ifndef EEGSTREAMER_H
#define EEGSTREAMER_H

#include <array>
#include <string>
#include <QChart>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QLineSeries>
#include <QList>
#include <QPointF>
#include <QTimer>
#include <QWidget>
#include <board_shim.h>
#include "cyton.h"
#include "ssvep.h"

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
    void initTimeSeries(QChart *&chart, QLineSeries *&series);
    void initPlotChart(QChart *&chart, std::array<QLineSeries *, 8> &series);
    Ui::EegStreamer *ui;
    Cyton *cyton;
    QTimer *timer;

    // Main
    std::array<QGraphicsScene *, 8> scenes;
    std::array<QChart *, 8> channels;
    std::array<QGraphicsTextItem *, 8> railed;
    std::array<QLineSeries *, 8> timeSeries;
    std::array<QList<QPointF>, 8> buffers;
    int sampleCount;

    // Ssvep
    Ssvep *ssvep;
    std::string recordFile;

    // FFT
    QChart *fftChart;
    std::array<QLineSeries *, 8> fftSeries;

    // PSD
    QChart *psdChart;
    std::array<QLineSeries *, 8> psdSeries;
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
