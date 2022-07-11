#include "paradigm.h"
#include "ui_paradigm.h"

Paradigm::Paradigm(QWidget *parent): QWidget(parent), ui(new Ui::Paradigm){
    ui->setupUi(this);
    init();
    connect(ui->closeButton, &QToolButton::clicked,
            this, [&](){emit closeSignal();});
}

#include <iostream>
using namespace std;

Paradigm::~Paradigm(){
    if(process1 != nullptr){
        process1->close();
        delete process1;
    }

    if(process2 != nullptr){
        process2->close();
        delete process2;
    }
    delete ui;
}

void Paradigm::StandMode(){
    ui->closeButton->show();
    ui->leftButton->setText("Stairs Mode");
    ui->rightButton->setText("Walk/Sit Mode");
    ui->stateLabel->setText("Stand Mode");
}

void Paradigm::ModeOne(){
    ui->closeButton->hide();
    ui->leftButton->setText("Down Mode");
    ui->rightButton->setText("Up Mode");
    ui->stateLabel->setText("Mode One");
}

void Paradigm::DownMode(){
    ui->leftButton->setText("None");
    ui->rightButton->setText("Stand Mode");
    ui->stateLabel->setText("Down Mode");
}

void Paradigm::UpMode(){
    ui->leftButton->setText("None");
    ui->rightButton->setText("Stand Mode");
    ui->stateLabel->setText("Up Mode");
}

void Paradigm::ModeTwo(){
    ui->closeButton->hide();
    ui->leftButton->setText("Walk Mode");
    ui->rightButton->setText("Sit Mode");
    ui->stateLabel->setText("Mode Two");
}

void Paradigm::WalkMode(){
    ui->leftButton->setText("MI Mode");
    ui->rightButton->setText("Stand Mode");
    ui->stateLabel->setText("Walk Mode");
}

void Paradigm::SitMode(){
    ui->leftButton->setText("None");
    ui->rightButton->setText("Stand Mode");
    ui->stateLabel->setText("Sit Mode");
}

void Paradigm::MI_Mode(){
    ui->leftButton->setText("L-Foot Move");
    ui->rightButton->setText("R-Foot Move");
    ui->stateLabel->setText("MI Mode");
}

void Paradigm::init(){
    process1 = new QProcess;
    process1->start("Stimulus-10hz.exe");

    process2 = new QProcess;
    process2->start("Stimulus-12hz.exe");
}
