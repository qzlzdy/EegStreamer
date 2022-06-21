#ifndef CSP_TRAIN_H
#define CSP_TRAIN_H

#include <vector>
#include <CSP/csp_train_ovo.h>
#include <DSIAPI/dataform.h>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QThread>
#include <QString>
#include <QCoreApplication>
#include <QMessageBox>

using namespace std;

//CSP_CCA Train
class CSP_Train: public QThread
{
    Q_OBJECT

public:
    explicit CSP_Train(QObject *parent = nullptr);
    ~CSP_Train();
    int operationStep(QString &csvFileName);

public:
    vector<double> _W1;//126
    vector<double> _W2;
    vector<double> _W3;
    vector<double> _sample1;//180
    vector<double> _sample2;
    vector<double> _sample3;
    QString fname;
    QFile *csvFile;

private:
    void run();
    double data_right_raw[189000];
    double data_left_raw[189000];
    double data_idle_raw[189000];

signals:
    void finish_train(bool);
};


#endif // CSP_TRAIN_H
