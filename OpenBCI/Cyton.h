#ifndef OPENBCI_CYTON_H
#define OPENBCI_CYTON_H

#include <QList>
#include <QMutex>
#include <QQueue>
#include <QString>
#include <QThread>
#include <QWaitCondition>
#include "board_shim.h"
#include "OpenBCI/Dataform.h"

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
    bool fftwSwitchFlag;
    bool readingFlag;
    bool recordSwitchFlag;
public slots:
    void reset();
signals:
    void offlineDataSignal(const BrainFlowArray<double, 2> &data);
    void bufferDataSignal(const BrainFlowArray<double, 2> &data);
protected:
    void run() override;
private:
    void readData();
    BoardShim *board;
};

class SignalBuffer{
public:
    SignalBuffer() = delete;
    static const qsizetype QUEUE_SIZE;
    static QQueue<ChannelSignal *> sharedData;
    static QMutex mutex;
    static QWaitCondition empty;
    static QWaitCondition full;
};

};

#endif
