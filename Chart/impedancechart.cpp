#include "impedancechart.h"
#include "ui_impedancechart.h"

using namespace ehdu;

ImpedanceChart::ImpedanceChart(QWidget *parent):
QWidget(parent), ui(new Ui::ImpedanceChart){
    ui->setupUi(this);
}

ImpedanceChart::~ImpedanceChart(){
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
int ImpedanceChart::impedanceQualityValue(double v){
    if(0 < v && v < 1){//green
        return 3;
    }
    else if(1 <= v && v < 10){//yellow
        return 2;
    }
    else if(10 <= v && v < 100){//red
        return 1;
    }
    else{//black
        return 0;
    }
}

void ImpedanceChart::impedanceQualitySlot(ChannelImpedance impData){
    switch(impedanceQualityValue(impData.Fp1.first)){
    case 0:
        ui->Fp1Label->setStyleSheet("background-color:black");
        break;
    case 1:
        ui->Fp1Label->setStyleSheet("background-color:red");
        break;
    case 2:
        ui->Fp1Label->setStyleSheet("background-color:yellow");
        break;
    case 3:
        ui->Fp1Label->setStyleSheet("background-color:green");
        break;
    }
    switch(impedanceQualityValue(impData.Fp2.first)){
    case 0:
        ui->Fp2Label->setStyleSheet("background-color:black");
        break;
    case 1:
        ui->Fp2Label->setStyleSheet("background-color:red");
        break;
    case 2:
        ui->Fp2Label->setStyleSheet("background-color:yellow");
        break;
    case 3:
        ui->Fp2Label->setStyleSheet("background-color:green");
        break;
    }
    switch(impedanceQualityValue(impData.C3.first)){
    case 0:
        ui->C3Label->setStyleSheet("background-color:black");
        break;
    case 1:
        ui->C3Label->setStyleSheet("background-color:red");
        break;
    case 2:
        ui->C3Label->setStyleSheet("background-color:yellow");
        break;
    case 3:
        ui->C3Label->setStyleSheet("background-color:green");
        break;
    }
    switch(impedanceQualityValue(impData.C4.first)){
    case 0:
        ui->C4Label->setStyleSheet("background-color:black");
        break;
    case 1:
        ui->C4Label->setStyleSheet("background-color:red");
        break;
    case 2:
        ui->C4Label->setStyleSheet("background-color:yellow");
        break;
    case 3:
        ui->C4Label->setStyleSheet("background-color:green");
        break;
    }
    switch(impedanceQualityValue(impData.P7.first)){
    case 0:
        ui->P7Label->setStyleSheet("background-color:black");
        break;
    case 1:
        ui->P7Label->setStyleSheet("background-color:red");
        break;
    case 2:
        ui->P7Label->setStyleSheet("background-color:yellow");
        break;
    case 3:
        ui->P7Label->setStyleSheet("background-color:green");
        break;
    }
    switch(impedanceQualityValue(impData.P8.first)){
    case 0:
        ui->P8Label->setStyleSheet("background-color:black");
        break;
    case 1:
        ui->P8Label->setStyleSheet("background-color:red");
        break;
    case 2:
        ui->P8Label->setStyleSheet("background-color:yellow");
        break;
    case 3:
        ui->P8Label->setStyleSheet("background-color:green");
        break;
    }
    switch(impedanceQualityValue(impData.O1.first)){
    case 0:
        ui->O1Label->setStyleSheet("background-color:black");
        break;
    case 1:
        ui->O1Label->setStyleSheet("background-color:red");
        break;
    case 2:
        ui->O1Label->setStyleSheet("background-color:yellow");
        break;
    case 3:
        ui->O1Label->setStyleSheet("background-color:green");
        break;
    }
    switch(impedanceQualityValue(impData.O2.first)){
    case 0:
        ui->O2Label->setStyleSheet("background-color:black");
        break;
    case 1:
        ui->O2Label->setStyleSheet("background-color:red");
        break;
    case 2:
        ui->O2Label->setStyleSheet("background-color:yellow");
        break;
    case 3:
        ui->O2Label->setStyleSheet("background-color:green");
        break;
    }
}

//reset
void ImpedanceChart::onResetButtonClicked(){
    emit resetSignals();
}
