#include "motorimagery.h"

#include <cmath>

#include "OpenBCI/Dataform.h"

const double motorImagery::dv0[5] = { 0.022564564495471402, 0.0, -0.045129128990942805, 0.0, 0.022564564495471402 };
const double motorImagery::dv1[5] = { 1.0, -3.4700191951417114, 4.5770105173722424, -2.72866765422065, 0.62266033998953441 };

motorImagery::motorImagery(QObject *parent) : QObject(parent)
{
    initFFTWPolicy();
}

motorImagery::~motorImagery()
{
    //销毁C4策略
    fftw_destroy_plan(C4Policy);
    //销毁C3策略
    fftw_destroy_plan(C3Policy);

    //释放分配二维数组内存
    fftw_free(C4ChannelIn);
    fftw_free(C4ChannelOut);
    fftw_free(C3ChannelIn);
    fftw_free(C3channelOut);

    //释放存储功率谱估计幅值double数组
    delete []C3powerSpectrData;
    delete []C4powerSpectrData;
}

//初始化FFTW
void motorImagery::initFFTWPolicy()
{
    //FFTW init
    C3ChannelIn = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * FFTW_DATA_SIZE);
    C3channelOut= (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * FFTW_DATA_SIZE);
    C4ChannelIn = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * FFTW_DATA_SIZE);
    C4ChannelOut= (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * FFTW_DATA_SIZE);

    //存储功率谱估计幅值double数组
    C3powerSpectrData = new double[FFTW_DATA_SIZE];
    C4powerSpectrData = new double[FFTW_DATA_SIZE];

    //初始化虚部位0.0
    for(int i=0;i<FFTW_DATA_SIZE;i++)
    {
        C3ChannelIn[i][1] = 0.0;
        C4ChannelIn[i][1] = 0.0;
        C3powerSpectrData[i] = 0.0;
        C4powerSpectrData[i] = 0.0;
    }

    //返回C3变换策略
    C3Policy = fftw_plan_dft_1d(FFTW_DATA_SIZE ,C3ChannelIn,C3channelOut, FFTW_FORWARD, FFTW_ESTIMATE);
    //返回C4变换策略
    C4Policy = fftw_plan_dft_1d(FFTW_DATA_SIZE ,C4ChannelIn,C4ChannelOut, FFTW_FORWARD, FFTW_ESTIMATE);    
}

//读取存储C3，C4数据,执行策略，进行变换
void motorImagery::fftwTransform()
{
    //执行C3变换
    fftw_execute(C3Policy);
    //执行C4变换
    fftw_execute(C4Policy);
}

//选择方法进行功率谱估计
void motorImagery::powerSpectrChooseMethod(int method)
{
    if(method & C3_POWER_SPECTRA_DIRCT_METHOD){//C3直接法
        for(int i=0;i<FFTW_DATA_SIZE;i++)
        {
            C3powerSpectrData[i] = CalculatePowerSpectraByDirctMethod(FFTW_DATA_SIZE,C3channelOut[i][0],C3channelOut[i][1]);
        }
    }
    if(method & C4_POWER_SPECTRA_DIRCT_METHOD){//C4直接法
        for(int i=0;i<FFTW_DATA_SIZE;i++)
        {
            C4powerSpectrData[i] = CalculatePowerSpectraByDirctMethod(FFTW_DATA_SIZE,C4ChannelOut[i][0],C4ChannelOut[i][1]);
        }
    }
}

/*********************************************
    计算功率谱密度
    直接法--实部平方加上虚部平方开根号除以采样点个数
 *********************************************/
double motorImagery::CalculatePowerSpectraByDirctMethod( int , double real, double imag)
{
    double result = 0.0;
//    result = (real/len) * (real/len) + (imag/len) * (imag/len);
//    result = sqrt(result);
    result = real*real + imag*imag;
    result = sqrt(result);
    return result;
}

/*********************************************
    计算功率谱密度
    AR回归模型法(参数估计)
 *********************************************/



// 利用ERD/ERS现象进行判别  8-13Hz
//如果C3数值高，说明左(left)运动想象，如果C4数值高，说明右(right)运动想象
int motorImagery::distinguish_ERD_ERS()
{
    double C3Ave = averageAlphaWave(MIN_FREQUENCY,MAX_FREQUENCY,C3powerSpectrData);
    double C4Ave = averageAlphaWave(MIN_FREQUENCY,MAX_FREQUENCY,C4powerSpectrData);
    qDebug()<<"C3:"<<C3Ave<<" C4:"<<C4Ave;
    if(C3Ave >= IMAGE_JUICE*C4Ave)
        return IMAGE_LEFT;
    else if(C4Ave >= IMAGE_JUICE*C3Ave)
        return IMAGE_RIGHT;
    return IMAGE_IDLE;
}

//计算功率铺特定频段平均值
double motorImagery::averageAlphaWave(int minFre,int maxFre, double *powerSpectraData)
{
    double sum = 0.0;
    for(int i=minFre; i<=maxFre; i++)
    {
        sum += powerSpectraData[i];
    }
    return (sum/(maxFre - minFre + 1));
}

//二阶巴特沃兹滤波器
void motorImagery::bufferFilter()
{
    double dbuffer[5];
    int k;
    int j;

    // butter：2阶的巴特沃斯滤波器  后为上下截止频率
    // filter：一维数字滤波器， x1是滤波前序列，B为分子，A为分母
    //1
    for (k = 0; k < 4; k++)
    {
      dbuffer[k + 1] = 0.0;
    }

    for (j = 0; j < 300; j++)
    {
        for (k = 0; k < 4; k++)
        {
            dbuffer[k] = dbuffer[k + 1];
        }

        dbuffer[4] = 0.0;
        for (k = 0; k < 5; k++)
        {
            dbuffer[k] += C3ChannelIn[j][1] * dv0[k];
        }

        for (k = 0; k < 4; k++)
        {
            dbuffer[k + 1] -= dbuffer[0] * dv1[k + 1];
        }

        C3ChannelIn[j][1] = dbuffer[0];
    }
    //2
    for (k = 0; k < 4; k++)
    {
      dbuffer[k + 1] = 0.0;
    }

    for (j = 0; j < 300; j++)
    {
        for (k = 0; k < 4; k++)
        {
            dbuffer[k] = dbuffer[k + 1];
        }

        dbuffer[4] = 0.0;
        for (k = 0; k < 5; k++)
        {
            dbuffer[k] += C4ChannelIn[j][1] * dv0[k];
        }

        for (k = 0; k < 4; k++)
        {
            dbuffer[k + 1] -= dbuffer[0] * dv1[k + 1];
        }

        C4ChannelIn[j][1] = dbuffer[0];
    }
}
