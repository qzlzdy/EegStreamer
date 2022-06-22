#ifndef DIALOG_H
#define DIALOG_H

#include <QCombobox>
#include <QDialog>
#include <QHBoxLayout>
#include <QMediaPlayer>
#include <QPushButton>
#include <QThread>
#include <QVector>
#include <QVideoWidget>
#include <QtConcurrent/QtConcurrent>
#include "deprecated/qmediaplaylist.h"
#include "deprecated/playercontrols.h"

#include "CCA/me_CCA.h"
#include "CHART/signalchart.h"
#include "CHART/impedancechart.h"
#include "OFFLINE/saveeeg.h"
#include "OpenBCI/Cyton.h"
#include "PARADIGM/paradigm.h"
#include "USART/rehabilitativeusart.h"

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

private:
    Ui::Dialog *ui;//ui
    SignalChart *chartWidght;//chart
    impedancechart *impedanceWidght;//imp
    ehdu::Cyton *dataRec;//DSI
    DSI_FFTW *dsiFFtWDataHandle;//fftw
    saveEEG *recordWidght;//record
    rehabilitativeUsart *sendPort;//usart
    QThread *portThread;//thread
    Paradigm *parad;//fan_shi

    QPushButton *startButton;
    QPushButton *stopButton;
    QPushButton *fftwStartButton;
    QPushButton *fftwStopButton;
    QHBoxLayout *buttonLayout;
    QHBoxLayout *widghtLayout;
    QComboBox *portBox;
    QPushButton *searchButton;

    QHBoxLayout *fanshiLayout;

    //video
    QMediaPlayer *player;
    PlayerControls *controls;
    QVideoWidget *videoWidget;
    QMediaPlaylist *medialist;

    //QTimer *waitChange;

    class State_Command *fs_State;

    QPushButton* startTrainButton;
    QComboBox *trainBox;
    QHBoxLayout *newLayout;

    CSP_Train *CSPTrain;

private:
    void initSlot();//初始化
    void connetSlot();//槽定义
    void loadQssSlot(QString name);//读取QSS

public slots:
    void cytonStop();//关闭DSI
    void cytonStart();//打开DSI
    void fftwStop();//关闭FFTW
    void fftwStart();//打开FFTW
    void startVideoLeft();
    void startVideoRight();
    void printToLable(QString data);
    void toSSVEP();
    void toMI();
    void threadC1();
    void threadC2();

    void changeMode(const int &data);
};

#endif // DIALOG_H
