#ifndef SAVEEEG_H
#define SAVEEEG_H

#include <QDebug>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>
#include <QWidget>
#include "board_shim.h"
#include "OpenBCI/Dataform.h"

QT_BEGIN_NAMESPACE
namespace Ui { class saveEEG; }
QT_END_NAMESPACE

class saveEEG : public QWidget{
    Q_OBJECT
public:
    explicit saveEEG(QWidget *parent, QList<QString> saveSignal,
                     QString reference);
    ~saveEEG();
    void startRecordButton(bool flag);
    void stopRecordButton(bool flag);
    QString lineeditdata();
public slots:
    void recordEEGslots(const BrainFlowArray<double, 2> &Data);
signals:
    void isTimeToRecord(bool flag);
    void stopRecordSignals();
private:
    Ui::saveEEG *ui;
    std::string filename;
    long count;
    QList<QString> signalTemp;
    QString saveReference;
private slots:
    void startRecordFile();
    void stopRecordFile();
};

#endif // SAVEEEG_H
