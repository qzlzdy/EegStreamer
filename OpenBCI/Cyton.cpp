#include "OpenBCI/Cyton.h"

#include <QtDebug>
#include <string>

using namespace std;
using namespace ehdu;

const vector<int> Cyton::eeg_channels = BoardShim::get_eeg_channels(
        static_cast<int>(BoardIds::CYTON_BOARD));
const vector<string> Cyton::eeg_names = BoardShim::get_eeg_names(
        static_cast<int>(BoardIds::CYTON_BOARD));

Cyton::Cyton(QObject *parent): QThread(parent){
    readingFlag = true;
    recordFlag = false;
    chooseChannelString.resize(eeg_names.size());
    transform(eeg_names.begin(), eeg_names.end(),
              chooseChannelString.begin(), [&](const string &name){
        return QString::fromStdString(name);
    });
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
    while(board->get_board_data_count() > 0){
        BrainFlowArray<double, 2> data = board->get_board_data(1);
        emit bufferDataSignal(data);
        if(recordFlag){
            emit recordToFile(data);
        }
    }
}
