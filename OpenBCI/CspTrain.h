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
    std::vector<double> W1;//126
    std::vector<double> W2;
    std::vector<double> W3;
    std::vector<double> sample1;//180
    std::vector<double> sample2;
    std::vector<double> sample3;
    QString filename;
    QFile *csvfile;
signals:
    void finishTrain(bool);
protected:
    void run() override;
private:
    std::array<double, 7200> dataIdleRaw;
    std::array<double, 7200> dataLeftRaw;
    std::array<double, 7200> dataRightRaw;
};

}

#endif // CSP_TRAIN_H
