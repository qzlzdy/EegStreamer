#include "OpenBCI/cyton.h"

#include <QtDebug>
#include <string>

using namespace std;
using namespace ehdu;

Cyton::Cyton(QObject *parent): QThread(parent){
    readingFlag = true;
    recordFlag = false;
    BrainFlowInputParams params;
    params.serial_port = string("/dev/ttyUSB0");
    board = new BoardShim(static_cast<int>(BoardIds::CYTON_BOARD),
                          params);
}

Cyton::~Cyton(){
    readingFlag = false;
    quit();
    wait();
    if(board->is_prepared()){
        board->release_session();
    }
    delete board;
}

void Cyton::startStream(){
    if(!board->is_prepared()){
        board->prepare_session();
    }
    board->start_stream();
}

void Cyton::stopStream(){
    try{
        board->stop_stream();
    }
    catch(const BrainFlowException &err){
        BoardShim::log_message(static_cast<int>(LogLevels::LEVEL_ERROR),
                               err.what());
        qCritical() << err.what();
    }
}

void Cyton::reset(){
    static_cast<void>(board->get_board_data());
}

void Cyton::run(){
    while(readingFlag){
        readData();
    }
}

void Cyton::readData(){
    if(board->get_board_data_count() > 0){
        BrainFlowArray<double, 2> data = board->get_board_data();
        emit bufferDataSignal(data);
        if(recordFlag){
            emit recordToFile(data);
        }
    }
}
