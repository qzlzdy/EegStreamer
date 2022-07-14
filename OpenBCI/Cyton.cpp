#include "OpenBCI/Cyton.h"

#include <QDebug>
#include <string>

using namespace std;
using namespace ehdu;

const vector<int> Cyton::eeg_channels = BoardShim::get_eeg_channels(
        static_cast<int>(BoardIds::CYTON_BOARD));
const vector<string> Cyton::eeg_names = BoardShim::get_eeg_names(
        static_cast<int>(BoardIds::CYTON_BOARD));

Cyton::Cyton(QObject *parent): QThread(parent){
    readingFlag = true;
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

static void dataStore(ChannelSignal *data, QString channelName,
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
    while(board->get_board_data_count() > 0){
        BrainFlowArray<double, 2> data = board->get_board_data(1);
        emit bufferDataSignal(data);
        if(recordSwitchFlag){
            emit offlineDataSignal(data);
        }
        if(fftwSwitchFlag){
            SignalBuffer::mutex.lock();
            if(SignalBuffer::sharedData.size() == SignalBuffer::QUEUE_SIZE){
                SignalBuffer::full.wait(&SignalBuffer::mutex);
            }
            if(!fftwSwitchFlag){
                SignalBuffer::empty.wakeAll();
                SignalBuffer::mutex.unlock();
                return;
            }
            ChannelSignal *pushData = new ChannelSignal;
            for(unsigned channel = 0; channel < eeg_names.size(); ++channel){
                QString channelName =
                    QString::fromStdString(eeg_names[channel]);
                double channelData = data(eeg_channels[channel], 0);
                dataStore(pushData, channelName, channelData);
            }
            SignalBuffer::sharedData.enqueue(pushData);
            SignalBuffer::empty.wakeAll();
            SignalBuffer::mutex.unlock();
        }
    }
}

const qsizetype SignalBuffer::QUEUE_SIZE = 300;
QQueue<ChannelSignal *> SignalBuffer::sharedData = QQueue<ChannelSignal *>();
QMutex SignalBuffer::mutex = QMutex();
QWaitCondition SignalBuffer::empty = QWaitCondition();
QWaitCondition SignalBuffer::full = QWaitCondition();
