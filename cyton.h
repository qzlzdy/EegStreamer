#ifndef OPENBCI_CYTON_H
#define OPENBCI_CYTON_H

#include <QThread>
#include "board_shim.h"

namespace ehdu{

class Cyton final: public QThread{
    Q_OBJECT
public:
    static const int SAMPLE_RATE;
    static const double SCALE_FACTOR;
    Cyton(QObject *parent = nullptr);
    ~Cyton();
    void startStream();
    void stopStream();
    bool readingFlag;
    bool recordFlag;
public slots:
    void reset();
signals:
    void bufferDataSignal(const BrainFlowArray<double, 2> &data);
    void recordToFile(const BrainFlowArray<double, 2> &data);
protected:
    void run() override;
private:
    void readData();
    BoardShim *board;
};

};

#endif
