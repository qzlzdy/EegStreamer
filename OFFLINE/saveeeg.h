#ifndef SAVEEEG_H
#define SAVEEEG_H

#include <QWidget>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>
#include <QDebug>
#include <DSIAPI/dataform.h>

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
private:
    Ui::saveEEG *ui;
    QFile *csvFile;
    long count;
    QList<QString> signalTemp;
    QString saveReference;
    QTextStream *in;
private slots:
    void startRecordFile();
    void stopRecordFile();
    void recordHeadSlots();
public slots:
    void recordEEGslots(channelSignal Data);
signals:
    void isTimeToRecord(bool flag);
    void stopRecordSignals();
};

#endif // SAVEEEG_H
