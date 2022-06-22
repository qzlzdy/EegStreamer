#include "OpenBCI/Cyton.h"

using namespace ehdu;

Cyton::Cyton(QObject *parent, const char *montage, int montageCount):
QThread(parent){
    globalMontage = const_cast<char *>(montage);
    globalMontageCount = montageCount;
    _chooseChannelString = settingChannel(QString(globalMontage));
    readingFlag = true;
}

Cyton::~Cyton(){
    readingFlag = false;
    quit();
    wait();
}

QList<QString> Cyton::chooseChannelString() const{
    return _chooseChannelString;
}

void Cyton::run(){
    while(readingFlag){

    }
}

QList<QString> Cyton::settingChannel(QString data){
    QList<QString> L;
    for(int i = 0; i < globalMontageCount; i++){
        QString inData = data.section(',', i, i);
        bool isFlag = inData.indexOf("-");
        if(isFlag){
            L << inData.section("-", 0, 0);
        }
        else{
            L << inData;
        }
    }
    return L;
}
