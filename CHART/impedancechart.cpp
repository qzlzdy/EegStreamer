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

void impedancechart::impedanceQualitySlot(channelImpedance impData)
{
    switch(impedanceQualityValue(impData.A1.first))
    {
        case 0: ui->A1label->setStyleSheet("background-color:black");break;
        case 1: ui->A1label->setStyleSheet("background-color:red");break;
        case 2: ui->A1label->setStyleSheet("background-color:yellow");break;
        case 3: ui->A1label->setStyleSheet("background-color:green");break;
    }

    switch(impedanceQualityValue(impData.A2.first))
    {
        case 0: ui->A2label->setStyleSheet("background-color:black");break;
        case 1: ui->A2label->setStyleSheet("background-color:red");break;
        case 2: ui->A2label->setStyleSheet("background-color:yellow");break;
        case 3: ui->A2label->setStyleSheet("background-color:green");break;
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
    switch(impedanceQualityValue(impData.F7.first))
    {
        case 0: ui->F7label->setStyleSheet("background-color:black");break;
        case 1: ui->F7label->setStyleSheet("background-color:red");break;
        case 2: ui->F7label->setStyleSheet("background-color:yellow");break;
        case 3: ui->F7label->setStyleSheet("background-color:green");break;
    }
    switch(impedanceQualityValue(impData.F8.first))
    {
        case 0: ui->F8label->setStyleSheet("background-color:black");break;
        case 1: ui->F8label->setStyleSheet("background-color:red");break;
        case 2: ui->F8label->setStyleSheet("background-color:yellow");break;
        case 3: ui->F8label->setStyleSheet("background-color:green");break;
    }
    switch(impedanceQualityValue(impData.Fp1.first))
    {
        case 0: ui->Fp1label->setStyleSheet("background-color:black");break;
        case 1: ui->Fp1label->setStyleSheet("background-color:red");break;
        case 2: ui->Fp1label->setStyleSheet("background-color:yellow");break;
        case 3: ui->Fp1label->setStyleSheet("background-color:green");break;
    }
    switch(impedanceQualityValue(impData.Fp2.first))
    {
        case 0: ui->Fp2label->setStyleSheet("background-color:black");break;
        case 1: ui->Fp2label->setStyleSheet("background-color:red");break;
        case 2: ui->Fp2label->setStyleSheet("background-color:yellow");break;
        case 3: ui->Fp2label->setStyleSheet("background-color:green");break;
    }
    switch(impedanceQualityValue(impData.Fz.first))
    {
        case 0: ui->Fzlabel->setStyleSheet("background-color:black");break;
        case 1: ui->Fzlabel->setStyleSheet("background-color:red");break;
        case 2: ui->Fzlabel->setStyleSheet("background-color:yellow");break;
        case 3: ui->Fzlabel->setStyleSheet("background-color:green");break;
    }
    switch(impedanceQualityValue(impData.O1.first))
    {
        case 0: ui->O1label->setStyleSheet("background-color:black");break;
        case 1: ui->O1label->setStyleSheet("background-color:red");break;
        case 2: ui->O1label->setStyleSheet("background-color:yellow");break;
        case 3: ui->O1label->setStyleSheet("background-color:green");break;
    }
    switch(impedanceQualityValue(impData.O2.first))
    {
        case 0: ui->O2label->setStyleSheet("background-color:black");break;
        case 1: ui->O2label->setStyleSheet("background-color:red");break;
        case 2: ui->O2label->setStyleSheet("background-color:yellow");break;
        case 3: ui->O2label->setStyleSheet("background-color:green");break;
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
    switch(impedanceQualityValue(impData.T3.first))
    {
        case 0: ui->T3label->setStyleSheet("background-color:black");break;
        case 1: ui->T3label->setStyleSheet("background-color:red");break;
        case 2: ui->T3label->setStyleSheet("background-color:yellow");break;
        case 3: ui->T3label->setStyleSheet("background-color:green");break;
    }
    switch(impedanceQualityValue(impData.T4.first))
    {
        case 0: ui->T4label->setStyleSheet("background-color:black");break;
        case 1: ui->T4label->setStyleSheet("background-color:red");break;
        case 2: ui->T4label->setStyleSheet("background-color:yellow");break;
        case 3: ui->T4label->setStyleSheet("background-color:green");break;
    }
    switch(impedanceQualityValue(impData.T5.first))
    {
        case 0: ui->T5label->setStyleSheet("background-color:black");break;
        case 1: ui->T5label->setStyleSheet("background-color:red");break;
        case 2: ui->T5label->setStyleSheet("background-color:yellow");break;
        case 3: ui->T5label->setStyleSheet("background-color:green");break;
    }
    switch(impedanceQualityValue(impData.T6.first))
    {
        case 0: ui->T6label->setStyleSheet("background-color:black");break;
        case 1: ui->T6label->setStyleSheet("background-color:red");break;
        case 2: ui->T6label->setStyleSheet("background-color:yellow");break;
        case 3: ui->T6label->setStyleSheet("background-color:green");break;
    }
    switch(impedanceQualityValue(impData.X1.first))
    {
        case 0: ui->X1label->setStyleSheet("background-color:black");break;
        case 1: ui->X1label->setStyleSheet("background-color:red");break;
        case 2: ui->X1label->setStyleSheet("background-color:yellow");break;
        case 3: ui->X1label->setStyleSheet("background-color:green");break;
    }
    switch(impedanceQualityValue(impData.X2.first))
    {
        case 0: ui->X2label->setStyleSheet("background-color:black");break;
        case 1: ui->X2label->setStyleSheet("background-color:red");break;
        case 2: ui->X2label->setStyleSheet("background-color:yellow");break;
        case 3: ui->X2label->setStyleSheet("background-color:green");break;
    }
    switch(impedanceQualityValue(impData.X3.first))
    {
        case 0: ui->X3label->setStyleSheet("background-color:black");break;
        case 1: ui->X3label->setStyleSheet("background-color:red");break;
        case 2: ui->X3label->setStyleSheet("background-color:yellow");break;
        case 3: ui->X3label->setStyleSheet("background-color:green");break;
    }
}

//reset
void impedancechart::on_resetButton_clicked()
{
    emit resetSignals();
}
