#ifndef ALGORITHMSWITCH_H
#define ALGORITHMSWITCH_H

#include <array>
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
    std::vector<ChannelSignal> rawData;
};

class SlidingWindow{
public:
    SlidingWindow() = default;
    virtual ~SlidingWindow() = default;
protected:
    bool firstRecFlag = false;
    std::array<double, 240> tempInputData;
};

class CommonSpatialPattern{
public:
    CommonSpatialPattern(
        const std::vector<double> &W1, const std::vector<double> &sample1,
        const std::vector<double> &W2, const std::vector<double> &sample2,
        const std::vector<double> &W3, const std::vector<double> &sample3);
    virtual ~CommonSpatialPattern() = default;
protected:
    int idleCount = 0;
    int miCount = 0;
    std::vector<double> W1;//126
    std::vector<double> W2;
    std::vector<double> W3;
    std::vector<double> sample1;//180
    std::vector<double> sample2;
    std::vector<double> sample3;
};

//CSP_CCA
class CspCca: public AlgorithmSwitch, public CommonSpatialPattern{
public:
    using CommonSpatialPattern::CommonSpatialPattern;
    virtual ~CspCca() override = default;
    virtual int step(ChannelSignal &getSignalDataAdress) override;
};


//SW_FFTW
class Fftw: public AlgorithmSwitch{
public:
    Fftw();
    virtual ~Fftw() override;
    virtual int step(ChannelSignal &getSignalDataAdress) override;
private:
    int fftwStoreSize = -1;
    bool firstRecvice = false;//是否第一次接受数据
    int C3flag = 0;
    int C4flag = 0;
    int Idleflag = 0;
    motorImagery *motorImageryObject = nullptr;
    std::vector<std::vector<double>> windowTranslationData;
};


//SW_CSP_CCA
class SwCspCca: public AlgorithmSwitch, public SlidingWindow,
public CommonSpatialPattern{
public:
    using CommonSpatialPattern::CommonSpatialPattern;
    virtual ~SwCspCca() override = default;
    virtual int step(ChannelSignal &getSignalDataAdress) override;
};


/*以下为新增2020-8-21 19:37:20*/

//CSP_KCCA
class CspKcca: public AlgorithmSwitch, public CommonSpatialPattern{
public:
    using CommonSpatialPattern::CommonSpatialPattern;
    virtual ~CspKcca() override = default;
    virtual int step(ChannelSignal &getSignalDataAdress) override;
};

//SW_CSP_KCCA
class SwCspKcca: public AlgorithmSwitch, public SlidingWindow,
public CommonSpatialPattern{
public:
    using CommonSpatialPattern::CommonSpatialPattern;
    virtual ~SwCspKcca() override = default;
    virtual int step(ChannelSignal &getSignalDataAdress) override;
};

}

#endif // ALORITHMSWITCH_H
