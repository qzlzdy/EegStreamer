#ifndef OPENBCI_DATAFORM_H
#define OPENBCI_DATAFORM_H

#include <utility>
#include <QString>

namespace ehdu{

using ChannelData = std::pair<double, QString>;

struct ChannelImpedance{
    ChannelImpedance(){
        Fp1.first = 0;
        Fp1.second = "Fp1";
        Fp2.first = 0;
        Fp2.second = "Fp2";
        C3.first = 0;
        C3.second = "C3";
        C4.first = 0;
        C4.second = "C4";
        P7.first = 0;
        P7.second = "P7";
        P8.first = 0;
        P8.second = "P8";
        O1.first = 0;
        O1.second = "O1";
        O2.first = 0;
        O2.second = "O2";
    }
    ChannelData Fp1, Fp2, C3, C4, P7, P8, O1, O2;
};

struct ChannelSignal{
    ChannelSignal(){
        Fp1.second = "Fp1";
        Fp2.second = "Fp2";
        C3.second = "C3";
        C4.second = "C4";
        P7.second = "P7";
        P8.second = "P8";
        O1.second = "O1";
        O2.second = "O2";
        channelNum = 8;
    }
    ChannelData Fp1, Fp2, C3, C4, P7, P8, O1, O2;
    int channelNum;
};

}

#endif // OPENBCI_DATAFORM_H
