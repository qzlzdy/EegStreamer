#ifndef CYTONFFTW_H
#define CYTONFFTW_H

#include <QFile>
#include <QString>
#include <QThread>
#include "OpenBCI/AlgorithmSwitch.h"

namespace ehdu{

class CytonFftw: public QThread{
    Q_OBJECT
public:
    explicit CytonFftw(AlgorithmSwitch *as, QObject *parent = nullptr);
    ~CytonFftw();
    void recordInit();
    bool readingFlag = true;//终止进程的标志
    bool uiState;
    QString useName;
signals:
    void idleSignal();
    void leftSignal();
    void rightSignal();
protected:
    void run() override;
private:
    void motorImageryUseFftw();
    AlgorithmSwitch *chooseAlorithm = nullptr;
    QFile *csvFile = nullptr;
    QTextStream *in = nullptr;
};

}

#endif
