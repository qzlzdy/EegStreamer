#include "impedancechart.h"
#include "ui_impedancechart.h"

impedancechart::impedancechart(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::impedancechart)
{
    ui->setupUi(this);
}

impedancechart::~impedancechart()
{
    delete ui;
}

/************************************
3个指标:impedance、noise、clipping
红：clipping both、pos、neg >100uVrms
   veryHight >10.0 M0hm
黄：not low   >1.0 M0hm
绿：low       <1.0 M0hm
baseline reset
**************************************/
int impedancechart::impedanceQualityValue(double v)
{
    if(v>0.0 && v<1.0){//green
        return 3;
    }else if(v>=1.0 && v<10.0){//yellow
        return 2;
    }else if(v>=10.0 && v<100.0){//red
        return 1;
    }else{//black
        return 0;
    }
}

void impedancechart::impedanceQualitySlot(ChannelImpedance impData){
    switch(impedanceQualityValue(impData.F3.first))
    {
        case 0: ui->F3label->setStyleSheet("background-color:black");break;
        case 1: ui->F3label->setStyleSheet("background-color:red");break;
        case 2: ui->F3label->setStyleSheet("background-color:yellow");break;
        case 3: ui->F3label->setStyleSheet("background-color:green");break;
    }
    switch(impedanceQualityValue(impData.F4.first))
    {
        case 0: ui->F4label->setStyleSheet("background-color:black");break;
        case 1: ui->F4label->setStyleSheet("background-color:red");break;
        case 2: ui->F4label->setStyleSheet("background-color:yellow");break;
        case 3: ui->F4label->setStyleSheet("background-color:green");break;
    }
    switch(impedanceQualityValue(impData.C3.first))
    {
        case 0: ui->C3label->setStyleSheet("background-color:black");break;
        case 1: ui->C3label->setStyleSheet("background-color:red");break;
        case 2: ui->C3label->setStyleSheet("background-color:yellow");break;
        case 3: ui->C3label->setStyleSheet("background-color:green");break;
    }
    switch(impedanceQualityValue(impData.C4.first))
    {
        case 0: ui->C4label->setStyleSheet("background-color:black");break;
        case 1: ui->C4label->setStyleSheet("background-color:red");break;
        case 2: ui->C4label->setStyleSheet("background-color:yellow");break;
        case 3: ui->C4label->setStyleSheet("background-color:green");break;
    }
    switch(impedanceQualityValue(impData.Cz.first))
    {
        case 0: ui->Czlabel->setStyleSheet("background-color:black");break;
        case 1: ui->Czlabel->setStyleSheet("background-color:red");break;
        case 2: ui->Czlabel->setStyleSheet("background-color:yellow");break;
        case 3: ui->Czlabel->setStyleSheet("background-color:green");break;
    }
    switch(impedanceQualityValue(impData.P3.first))
    {
        case 0: ui->P3label->setStyleSheet("background-color:black");break;
        case 1: ui->P3label->setStyleSheet("background-color:red");break;
        case 2: ui->P3label->setStyleSheet("background-color:yellow");break;
        case 3: ui->P3label->setStyleSheet("background-color:green");break;
    }
    switch(impedanceQualityValue(impData.P4.first))
    {
        case 0: ui->P4label->setStyleSheet("background-color:black");break;
        case 1: ui->P4label->setStyleSheet("background-color:red");break;
        case 2: ui->P4label->setStyleSheet("background-color:yellow");break;
        case 3: ui->P4label->setStyleSheet("background-color:green");break;
    }
    switch(impedanceQualityValue(impData.Pz.first))
    {
        case 0: ui->Pzlabel->setStyleSheet("background-color:black");break;
        case 1: ui->Pzlabel->setStyleSheet("background-color:red");break;
        case 2: ui->Pzlabel->setStyleSheet("background-color:yellow");break;
        case 3: ui->Pzlabel->setStyleSheet("background-color:green");break;
    }
}

//reset
void impedancechart::onResetButtonClicked()
{
    emit resetSignals();
}
