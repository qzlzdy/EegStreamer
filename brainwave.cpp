#include "brainwave.h"

#include <QtDebug>
#include <string>

using namespace std;
using namespace ehdu;

const int BrainWave::SAMPLE_RATE = 250;

BrainWave::BrainWave(QObject *parent): QThread(parent){
    readingFlag = true;
    recordFlag = false;
    BrainFlowInputParams params;
    params.serial_port = string("/dev/ttyUSB0");
    board = new BoardShim(static_cast<int>(BoardIds::CYTON_BOARD),
                          params);
}

BrainWave::~BrainWave(){
    readingFlag = false;
    quit();
    wait();
    if(board->is_prepared()){
        board->release_session();
    }
    delete board;
}

void BrainWave::startStream(){
    if(!board->is_prepared()){
        board->prepare_session();
    }
    board->start_stream();
}

void BrainWave::stopStream(){
    try{
        board->stop_stream();
    }
    catch(const BrainFlowException &err){
        BoardShim::log_message(static_cast<int>(LogLevels::LEVEL_ERROR),
                               err.what());
        qCritical() << err.what();
    }
}

void BrainWave::reset(){
    static_cast<void>(board->get_board_data());
}

void BrainWave::run(){
    while(readingFlag){
        readData();
    }
}

void BrainWave::readData(){
    if(board->get_board_data_count() > 0){
        BrainFlowArray<double, 2> data = board->get_board_data();
        emit bufferDataSignal(data);
        if(recordFlag){
            emit recordToFile(data);
        }
    }
}
