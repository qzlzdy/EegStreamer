#ifndef ALORITHMSWITCH_H
#define ALORITHMSWITCH_H

#include <QObject>
#include <vector>
#include <CCA/me_CCA.h>
#include <FFTW/fftw3.h>
#include <FFTW/motorimagery.h>
#include <CSP/csp_rec_ovo.h>
#include <CSP/csp_train_ovo.h>
#include <DSIAPI/dataform.h>
#include "KCCA/kcca.h"
#include <QDebug>

using namespace std;

class AlorithmSwitch
{
public:
    AlorithmSwitch();
    virtual ~AlorithmSwitch();
    virtual int operationStep(channelSignal &getSignalDataAdress) = 0;

public:
    vector<channelSignal> rawData;
};


//CSP_CCA
class CSP_CCA_AlorithmSwitch : public AlorithmSwitch
{
public:
    CSP_CCA_AlorithmSwitch(vector<double> W1, vector<double> sample1, vector<double> W2, vector<double> sample2
                           ,vector<double> W3,vector<double> sample3);

    virtual ~CSP_CCA_AlorithmSwitch() override;
    virtual int operationStep(channelSignal& getSignalDataAdress) override;

private:
    int countIdle;
    int mi_count;
    vector<double> _W1;//126
    vector<double> _W2;
    vector<double> _W3;
    vector<double> _sample1;//180
    vector<double> _sample2;
    vector<double> _sample3;
};


//SW_FFTW
class FFTW_AlorithmSwitch : public AlorithmSwitch
{
public:
    FFTW_AlorithmSwitch();
    virtual ~FFTW_AlorithmSwitch() override;
    virtual int operationStep(channelSignal& getSignalDataAdress) override;

private:
    int C3flag;
    int C4flag;
    int Idlefalsg;
    bool firstRecviceDataFlag;//是否第一次接受数据
    motorImagery *motorImageryObject;
    int fftwStoreSize;
    vector<vector<double>> windowTranslationData;
};


//SW_CSP_CCA
class SW_CSP_CCA_AlorithmSwitch : public AlorithmSwitch
{
public:
    SW_CSP_CCA_AlorithmSwitch(vector<double> W1, vector<double> sample1, vector<double> W2, vector<double> sample2
                          ,vector<double> W3,vector<double> sample3);

    virtual ~SW_CSP_CCA_AlorithmSwitch() override;
    virtual int operationStep(channelSignal& getSignalDataAdress) override;

private:
    double tempInputData[6300];
    bool firstRecFlag;
    int countIdle;
    int mi_count;
    vector<double> _W1;//126
    vector<double> _W2;
    vector<double> _W3;
    vector<double> _sample1;//180
    vector<double> _sample2;
    vector<double> _sample3;
};


/*以下为新增2020-8-21 19:37:20*/

//CSP_KCCA
class CSP_KCCA_AlorithmSwitch : public AlorithmSwitch
{
public:
    CSP_KCCA_AlorithmSwitch(vector<double> W1, vector<double> sample1, vector<double> W2, vector<double> sample2
                           ,vector<double> W3,vector<double> sample3);

    virtual ~CSP_KCCA_AlorithmSwitch() override;
    virtual int operationStep(channelSignal& getSignalDataAdress) override;

private:
    int countIdle;
    int mi_count;
    vector<double> _W1;//126
    vector<double> _W2;
    vector<double> _W3;
    vector<double> _sample1;//180
    vector<double> _sample2;
    vector<double> _sample3;
};

//SW_CSP_KCCA
class SW_CSP_KCCA_AlorithmSwitch : public AlorithmSwitch
{
public:
    SW_CSP_KCCA_AlorithmSwitch(vector<double> W1, vector<double> sample1, vector<double> W2, vector<double> sample2
                          ,vector<double> W3,vector<double> sample3);

    virtual ~SW_CSP_KCCA_AlorithmSwitch() override;
    virtual int operationStep(channelSignal& getSignalDataAdress) override;

private:
    double tempInputData[6300];
    bool firstRecFlag;
    int countIdle;
    int mi_count;
    vector<double> _W1;//126
    vector<double> _W2;
    vector<double> _W3;
    vector<double> _sample1;//180
    vector<double> _sample2;
    vector<double> _sample3;
};


#endif // ALORITHMSWITCH_H
