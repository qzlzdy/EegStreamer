#include "ssvep.h"
#include "ui_ssvep.h"

#include <QString>

using namespace std;
using namespace ehdu;

Ssvep::Ssvep(QWidget *parent): QWidget(parent), ui(new Ui::Ssvep){
    ui->setupUi(this);
    timer = new QTimer(this);
    timerFlag = false;

    QString style = "background-color: rgb(255, 0, 255);";
    center = new QWidget(this);
    center->setStyleSheet(style);
    east = new QWidget(this);
    east->setStyleSheet(style);
    south = new QWidget(this);
    south->setStyleSheet(style);
    west = new QWidget(this);
    west->setStyleSheet(style);
    north = new QWidget(this);
    north->setStyleSheet(style);
    ui->SsvepLayout->addWidget(center, 2, 2);
    ui->SsvepLayout->addWidget(east, 2, 4);
    ui->SsvepLayout->addWidget(south, 4, 2);
    ui->SsvepLayout->addWidget(west, 2, 0);
    ui->SsvepLayout->addWidget(north, 0, 2);
    for(int i = 0; i < 5; ++i){
        ui->SsvepLayout->setRowStretch(i, 1);
        ui->SsvepLayout->setColumnStretch(i, 1);
    }

    connect(timer, &QTimer::timeout,
            this, [&](){
        if(timerFlag){
            ssvepOn();
        }
        else{
            ssvepOff();
        }
        timerFlag = !timerFlag;
    });
}

Ssvep::~Ssvep(){
    delete center;
    delete east;
    delete south;
    delete west;
    delete north;
    delete timer;
    delete ui;
}

void Ssvep::startSsvep(chrono::milliseconds cycle){
    timer->start(cycle.count());
}

void Ssvep::stopSsvep(){
    timer->stop();
}

void Ssvep::ssvepOn(){
    center->show();
    east->show();
    south->show();
    west->show();
    north->show();
}

void Ssvep::ssvepOff(){
    center->hide();
    east->hide();
    south->hide();
    west->hide();
    north->hide();
}
