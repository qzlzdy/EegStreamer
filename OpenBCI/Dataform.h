#ifndef OPENBCI_DATAFORM_H
#define OPENBCI_DATAFORM_H

#define C3_POWER_SPECTRA_DIRCT_METHOD 1
#define C4_POWER_SPECTRA_DIRCT_METHOD 2
#define FFTW_DATA_SIZE 300
#define IMAGE_LEFT  0
#define IMAGE_RIGHT 1
#define IMAGE_IDLE 2
#define IMAGE_JUICE 2.5
#define MAX_FREQUENCY 13
#define MIN_FREQUENCY 8

#include <QString>

#include <utility>

namespace ehdu{

using ChannelData = std::pair<double, QString>;

struct ChannelImpedance{
    ChannelImpedance(){
        F3.first = 0;
        F3.second = "F3";
        F4.first = 0;
        F4.second = "F4";
        C3.first = 0;
        C3.second = "C3";
        Cz.first = 0;
        Cz.second = "Cz";
        C4.first = 0;
        C4.second = "C4";
        P3.first = 0;
        P3.second = "P3";
        Pz.first = 0;
        Pz.second = "Pz";
        P4.first = 0;
        P4.second = "P4";
    }
    ChannelData F3, F4, C3, Cz, C4, P3, Pz, P4;
};

struct ChannelSignal{
    ChannelSignal(){
        F3.second = "F3";
        F4.second = "F4";
        C3.second = "C3";
        Cz.second = "Cz";
        C4.second = "C4";
        P3.second = "P3";
        Pz.second = "Pz";
        P4.second = "P4";
        channelNum = 8;
    }
    ChannelData F3, F4, C3, Cz, C4, P3, Pz, P4;
    int channelNum;
};

};

#endif
