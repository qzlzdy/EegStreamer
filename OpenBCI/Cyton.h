#ifndef OPENBCI_CYTON_H
#define OPENBCI_CYTON_H

#include <QList>
#include <QString>
#include <QThread>

#include "Dataform.h"

namespace ehdu{

class Cyton final: public QThread{
    Q_OBJECT
public:
    Cyton(QObject *parent = nullptr);
    ~Cyton();
    // FIXME
    int BufferingStart();
    int ImpedanceSlots();
    QList<QString> chooseChannelString;
    bool fftwSwitchFlag;
    bool recordSwitchFlag;
public slots:
    void resetSlots();
signals:
    void offLineDataSignal(ChannelSignal);
    void bufferDataSignal(ChannelSignal);
};

};

#endif
