#ifndef OPENBCI_CYTON_H
#define OPENBCI_CYTON_H

#include <QList>
#include <QString>
#include <QThread>
#include "board_shim.h"

namespace ehdu{

class Cyton final: public QThread{
    Q_OBJECT
public:
    static const std::vector<int> eeg_channels;
    static const std::vector<std::string> eeg_names;
    Cyton(QObject *parent = nullptr);
    ~Cyton();
    void startStream();
    void stopStream();
    QList<QString> chooseChannelString;
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
