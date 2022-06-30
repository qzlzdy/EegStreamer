#ifndef ALGORITHMSWITCH_H
#define ALGORITHMSWITCH_H

#include <vector>
#include <QDebug>
#include <QObject>

#include "CCA/me_CCA.h"
#include "CSP/csp_rec_ovo.h"
#include "CSP/csp_train_ovo.h"
#include "FFTW/fftw3.h"
#include "FFTW/motorimagery.h"
#include "OpenBCI/Dataform.h"
#include "KCCA/kcca.h"

namespace ehdu{

class AlgorithmSwitch{
public:
    AlgorithmSwitch() = default;
    virtual ~AlgorithmSwitch() = default;
    virtual int step(ChannelSignal &getSignalDataAdress) = 0;
public:
    std::vector<ChannelSignal> rawData;
};

//CSP_CCA
class CspCcaAlgorithmSwitch: public AlgorithmSwitch{
public:
    CspCcaAlgorithmSwitch(std::vector<double> W1, std::vector<double> sample1,
                         std::vector<double> W2, std::vector<double> sample2,
                         std::vector<double> W3, std::vector<double> sample3);
    virtual ~CspCcaAlgorithmSwitch() override = default;
    virtual int step(ChannelSignal &getSignalDataAdress) override;
private:
    int countIdle;
    int mi_count;
    std::vector<double> _W1;//126
    std::vector<double> _W2;
    std::vector<double> _W3;
    std::vector<double> _sample1;//180
    std::vector<double> _sample2;
    std::vector<double> _sample3;
};


//SW_FFTW
class FftwAlgorithmSwitch: public AlgorithmSwitch{
public:
    FftwAlgorithmSwitch();
    virtual ~FftwAlgorithmSwitch() override;
    virtual int step(ChannelSignal &getSignalDataAdress) override;
private:
    int C3flag;
    int C4flag;
    int Idlefalsg;
    bool firstRecviceDataFlag;//是否第一次接受数据
    motorImagery *motorImageryObject;
    int fftwStoreSize;
    std::vector<std::vector<double>> windowTranslationData;
};


//SW_CSP_CCA
class SwCspCcaAlgorithmSwitch: public AlgorithmSwitch{
public:
    SwCspCcaAlgorithmSwitch(std::vector<double> W1, std::vector<double> sample1,
                           std::vector<double> W2, std::vector<double> sample2,
                           std::vector<double> W3, std::vector<double> sample3);
    virtual ~SwCspCcaAlgorithmSwitch() override = default;
    virtual int step(ChannelSignal &getSignalDataAdress) override;
private:
    double tempInputData[6300];
    bool firstRecFlag;
    int countIdle;
    int mi_count;
    std::vector<double> _W1;//126
    std::vector<double> _W2;
    std::vector<double> _W3;
    std::vector<double> _sample1;//180
    std::vector<double> _sample2;
    std::vector<double> _sample3;
};


/*以下为新增2020-8-21 19:37:20*/

//CSP_KCCA
class CspKccaAlgorithmSwitch: public AlgorithmSwitch{
public:
    CspKccaAlgorithmSwitch(std::vector<double> W1, std::vector<double> sample1,
                          std::vector<double> W2, std::vector<double> sample2,
                          std::vector<double> W3, std::vector<double> sample3);
    virtual ~CspKccaAlgorithmSwitch() override = default;
    virtual int step(ChannelSignal &getSignalDataAdress) override;
private:
    int countIdle;
    int mi_count;
    std::vector<double> _W1;//126
    std::vector<double> _W2;
    std::vector<double> _W3;
    std::vector<double> _sample1;//180
    std::vector<double> _sample2;
    std::vector<double> _sample3;
};

//SW_CSP_KCCA
class SwCspKccaAlgorithmSwitch: public AlgorithmSwitch{
public:
    SwCspKccaAlgorithmSwitch(std::vector<double> W1, std::vector<double> sample1,
                            std::vector<double> W2, std::vector<double> sample2,
                            std::vector<double> W3, std::vector<double> sample3);
    virtual ~SwCspKccaAlgorithmSwitch() override = default;
    virtual int step(ChannelSignal &getSignalDataAdress) override;
private:
    double tempInputData[6300];
    bool firstRecFlag;
    int countIdle;
    int mi_count;
    std::vector<double> _W1;//126
    std::vector<double> _W2;
    std::vector<double> _W3;
    std::vector<double> _sample1;//180
    std::vector<double> _sample2;
    std::vector<double> _sample3;
};

}

#endif // ALORITHMSWITCH_H
