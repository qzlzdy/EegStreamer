#include "ssvep.h"
#include "ui_ssvep.h"

#include <QString>

using namespace std;
using namespace ehdu;

Ssvep::Ssvep(QWidget *parent): QWidget(parent), ui(new Ui::Ssvep){
    ui->setupUi(this);

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

    timer = nullptr;
    flipFlag = false;
}

Ssvep::~Ssvep(){
    delete center;
    delete east;
    delete south;
    delete west;
    delete north;
    if(timer){
        timer->flashFlag = false;
        timer->wait();
        delete timer;
    }
    delete ui;
}

void Ssvep::startSsvep(const chrono::nanoseconds &cycle){
    timer = new SsvepTimer(cycle);
    connect(timer, &SsvepTimer::timeout, this, [&](){
        if(flipFlag){
            ssvepOn();
        }
        else{
            ssvepOff();
        }
        flipFlag = !flipFlag;
    });
    timer->start();
}

void Ssvep::stopSsvep(){
    timer->flashFlag = false;
    timer->wait();
    delete timer;
    timer = nullptr;
}

void Ssvep::ssvepOn(){
    center->show();
//    east->show();
//    south->show();
//    west->show();
//    north->show();
    update();
}

void Ssvep::ssvepOff(){
    center->hide();
//    east->hide();
//    south->hide();
//    west->hide();
//    north->hide();
    update();
}
