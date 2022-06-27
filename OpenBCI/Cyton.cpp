#include "OpenBCI/Cyton.h"

using namespace ehdu;

Cyton::Cyton(QObject *parent): QThread(parent){

}

Cyton::~Cyton(){

}

int Cyton::BufferingStart(){
    return 0;
}

int Cyton::ImpedanceSlots(){
    return 0;
}

void Cyton::resetSlots(){

}
