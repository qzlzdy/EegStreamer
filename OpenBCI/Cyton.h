#ifndef OPENBCI_CYTON_H
#define OPENBCI_CYTON_H

#include <QThread>

#include "Dataform.h"

namespace ehdu{

class Cyton final: public QThread{
    Q_OBJECT
public:
    Cyton(QObject *parent = nullptr,
          const char *montage = "C3, C4",
          int montageCount = 2);
    ~Cyton();
    QList<QString> chooseChannelString() const;
signals:
    void bufferDataSignal(ehdu::ChannelSignal data);
public slots:
    void resetSlots();
private:
    void run() override;
    QList<QString> settingChannel(QString data);
    QList<QString> _chooseChannelString;
    char *globalMontage;
    int globalMontageCount;
    bool readingFlag;
};

};

#endif
