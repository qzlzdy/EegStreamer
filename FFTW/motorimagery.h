#ifndef MOTORIMAGERY_H
#define MOTORIMAGERY_H

#include <QDebug>
#include <QObject>

#include "FFTW/fftw3.h"

class motorImagery: public QObject{
    Q_OBJECT
public:
    explicit motorImagery(QObject *parent = nullptr);
    ~motorImagery();
    void bufferFilter();
    int distinguish_ERD_ERS();
    void fftwTransform();
    void powerSpectrChooseMethod(int method);
    //申请动态内存，构造二维数组
    fftw_complex *C3ChannelIn;
    fftw_complex *C3channelOut;
    fftw_complex *C4ChannelIn;
    fftw_complex *C4ChannelOut;
    //声明变换策略
    fftw_plan C3Policy;
    fftw_plan C4Policy;
    //存储功率谱估计数据
    double *C3powerSpectrData;
    double *C4powerSpectrData;
private:
    static const double dv0[5];
    static const double dv1[5];
    double averageAlphaWave(int minFre,int maxFre, double *powerSpectraData);
    double CalculatePowerSpectraByDirctMethod( int len, double real, double imag );
    void initFFTWPolicy();
};

#endif // MOTORIMAGERY_H
