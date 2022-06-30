#ifndef DIALOG_H
#define DIALOG_H

#include <QComboBox>
#include <QDialog>
#include <QHBoxLayout>
#include <QMediaPlayer>
#include <QPushButton>
#include <QThread>
#include <QVector>
#include <QVideoWidget>

#include "CCA/me_CCA.h"
#include "CHART/signalchart.h"
#include "CHART/impedancechart.h"
#include "deprecated/qmediaplaylist.h"
#include "deprecated/playercontrols.h"
#include "OFFLINE/saveeeg.h"
#include "OpenBCI/CspTrain.h"
#include "OpenBCI/Cyton.h"
#include "OpenBCI/CytonFftw.h"
#include "PARADIGM/paradigm.h"
#include "USART/rehabilitativeusart.h"

class StateCommand;

QT_BEGIN_NAMESPACE
namespace Ui { class Dialog; }
QT_END_NAMESPACE

class Dialog: public QDialog{
    Q_OBJECT
public:
    explicit Dialog(QWidget *parent = nullptr);
    ~Dialog();
public slots:
    void changeMode(const int &data);
    void cytonStart();
    void cytonStop();
    void fftwStart();//打开FFTW
    void fftwStop();//关闭FFTW
    void printToLable(QString data);
    void startVideoLeft();
    void startVideoRight();
    void toMI();
    void toSSVEP();
    void threadC1();
    void threadC2();
private:
    void connetSlots();//槽定义
    void initialize();//初始化
    void loadQssSlot(QString name);//读取QSS
    Ui::Dialog *ui;//ui
    SignalChart *chartWidght;//chart
    impedancechart *impedanceWidght;//imp
    ehdu::Cyton *dataRec;
    ehdu::CytonFftw *cytonFftwDataHandler;//fftw
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

    StateCommand *fs_State;

    QPushButton* startTrainButton;
    QComboBox *trainBox;
    QHBoxLayout *newLayout;

    CspTrain *cspTrain;
};

#endif // DIALOG_H
