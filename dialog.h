#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QCombobox>
#include <QThread>
#include <QVector>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QMediaPlaylist>
#include <DSIAPI/dsi_main.h>
#include <CHART/signalchart.h>
#include <CHART/impedancechart.h>
#include <OFFLINE/saveeeg.h>
#include <USART/rehabilitativeusart.h>
#include <PARADIGM/paradigm.h>
#include <DSIAPI/state_command.h>
#include <CCA/me_CCA.h>
#include <DSIAPI/alorithmswitch.h>
#include <DSIAPI/csp_train.h>
#include <QtConcurrent/QtConcurrent>

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
    signalChart *chartWidght;//chart
    impedancechart *impedanceWidght;//imp
    DSI_Main *dsiDataRec;//DSI
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
    void dsiStop();//关闭DSI
    void dsiStart();//打开DSI
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
