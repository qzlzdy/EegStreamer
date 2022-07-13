#include "OpenBCI/Cyton.h"

#include <QDebug>
#include <QMutex>
#include <QQueue>
#include <QWaitCondition>
#include <string>
#include <mutex>

using namespace std;
using namespace ehdu;

namespace{

once_flag initialized;
const size_t QUEUE_SIZE = 300;
Q_GLOBAL_STATIC(QQueue<ChannelSignal *>, threadShareData);
QMutex *bufferMutex;
QWaitCondition *bufferIsEmpty;
QWaitCondition *bufferIsFull;

vector<int> eeg_channels = BoardShim::get_eeg_channels(
    static_cast<int>(BoardIds::CYTON_BOARD));
vector<string> eeg_names = BoardShim::get_eeg_names(
    static_cast<int>(BoardIds::CYTON_BOARD));

}

static void init(){
    bufferMutex = new QMutex;
    bufferIsEmpty = new QWaitCondition;
    bufferIsFull = new QWaitCondition;
}

Cyton::Cyton(QObject *parent): QThread(parent){
    readingFlag = true;
    chooseChannelString = {"Fp1", "Fp2", "C3", "C4", "P7", "P8", "O1", "O2"};
    BrainFlowInputParams params;
    params.serial_port = string("/dev/ttyUSB0");
    board = new BoardShim(static_cast<int>(BoardIds::CYTON_BOARD),
                          params);
    call_once(initialized, init);
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
        qWarning() << err.what();
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

void Cyton::dataStore(ChannelSignal *data, QString channelName,
                      double channelData){
    if(channelName == "Fp1"){
        data->Fp1.first = channelData;
    }
    else if(channelName == "Fp2"){
        data->Fp2.first = channelData;
    }
    else if(channelName == "C3"){
        data->C3.first = channelData;
    }
    else if(channelName == "C4"){
        data->C4.first = channelData;
    }
    else if(channelName == "P7"){
        data->P7.first = channelData;
    }
    else if(channelName == "P8"){
        data->P8.first = channelData;
    }
    else if(channelName == "O1"){
        data->O1.first = channelData;
    }
    else if(channelName == "O2"){
        data->O2.first = channelData;
    }
}

void Cyton::readData(){
    BrainFlowArray<double, 2> data = board->get_board_data();
    for(int sample = 0; sample < data.get_size(1); ++sample){
        for(unsigned channel = 0; channel < eeg_names.size(); ++channel){
            QString channelName =
                QString::fromStdString(eeg_names[channel]);
            double channelData = data(eeg_channels[channel], sample);
            dataStore(&chartSignal, channelName, channelData);
        }
        emit bufferDataSignal(chartSignal);
        if(recordSwitchFlag){
            emit offlineDataSignal(chartSignal);
        }
        if(fftwSwitchFlag){
            bufferMutex->lock();
            if(threadShareData->size() == QUEUE_SIZE){
                bufferIsFull->wait(bufferMutex);
            }
            if(!fftwSwitchFlag){
                bufferIsEmpty->wakeAll();
                bufferMutex->unlock();
                return;
            }
            ChannelSignal *pushData = new ChannelSignal;
            *pushData = chartSignal;
            threadShareData->enqueue(pushData);
            bufferIsEmpty->wakeAll();
            bufferMutex->unlock();
        }
    }
}
