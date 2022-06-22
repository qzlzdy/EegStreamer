#ifndef OPENBCI_DATAFORM_H
#define OPENBCI_DATAFORM_H

#include <QString>

#include <utility>

namespace ehdu{

struct ChannelSignal{
    using ChannelData = std::pair<double, QString>;
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
