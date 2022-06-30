#ifndef OPENBCI_CYTON_H
#define OPENBCI_CYTON_H

#include <QList>
#include <QString>
#include <QThread>

#include "Dataform.h"
#include "board_shim.h"

namespace ehdu{

class Cyton final: public QThread{
    Q_OBJECT
public:
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
    void offlineDataSignal(ehdu::ChannelSignal data);
    void bufferDataSignal(ehdu::ChannelSignal data);
//    void fixedTimeSignal(ehdu::ChannelImpedance data);
protected:
    void run() override;
private:
    void dataStore(ChannelSignal *data, QString channelName,
                   double channelData);
    void readData();
    BoardShim *board;
    ChannelSignal chartSignal;
};

};

#endif
