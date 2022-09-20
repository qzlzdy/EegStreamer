#ifndef EEGSTREAMER_H
#define EEGSTREAMER_H

#include <string>
#include <QComboBox>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QWidget>
#include "board_shim.h"
#include "Chart/impedancechart.h"
#include "Chart/signalchart.h"
#include "OpenBCI/Cyton.h"
#include "Ssvep/ssvep.h"

namespace Ui { class EegStreamer; }

namespace ehdu{

class EegStreamer: public QWidget{
    Q_OBJECT
public:
    static const QString FREQ_6_494_Hz;
    static const QString FREQ_8_065_Hz;
    static const QString FREQ_11_628_Hz;
    static const QString FREQ_15_625_Hz;
    explicit EegStreamer(QWidget *parent = nullptr);
    ~EegStreamer();
private:
    Ui::EegStreamer *ui;
    Cyton *cyton;

    QHBoxLayout *chartLayout;
    ImpedanceChart *impedanceChart;
    SignalChart *signalChart;

    QHBoxLayout *buttonsLayout;
    QComboBox *ports;
    QPushButton *refreshPorts;
    QPushButton *connectCyton;
    QPushButton *disconnectCyton;

    Ssvep *ssvep;
    QHBoxLayout *recordLayout;
    QComboBox *frequencies;
    QLineEdit *filenameEdit;
    QPushButton *startRecord;
    QPushButton *stopRecord;

    std::string recordFile;
private slots:
    void startCyton();
    void stopCyton();
    void startRecordToFile();
    void stopRecordToFile();
    void recordData(const BrainFlowArray<double, 2> &data);
};

}

#endif // EEGSTREAMER_H
