#ifndef CSPTRAIN_H
#define CSPTRAIN_H

#include <vector>

#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QMessageBox>
#include <QString>
#include <QTextStream>
#include <QThread>

#include "CSP/csp_train_ovo.h"
#include "OpenBCI/Dataform.h"

namespace ehdu{

//CSP_CCA Train
class CspTrain: public QThread{
    Q_OBJECT
public:
    explicit CspTrain(QObject *parent = nullptr);
    ~CspTrain();
    void step(QString &csvFilename);
    std::vector<double> _W1;//126
    std::vector<double> _W2;
    std::vector<double> _W3;
    std::vector<double> _sample1;//180
    std::vector<double> _sample2;
    std::vector<double> _sample3;
    QString filename;
    QFile *csvfile;
signals:
    void finishTrain(bool);
protected:
    void run() override;
private:
    double dataIdleRaw[189000];
    double dataLeftRaw[189000];
    double dataRightRaw[189000];
};

}

#endif // CSP_TRAIN_H
