#include "OpenBCI/AlgorithmSwitch.h"

#define THRESHOLD 0.42

using namespace std;
using namespace ehdu;

//滤波器
/* data ---  要处理的数据 */
/* Fs   ---  采样率 */
/* eegx ---  返回的数据 */
/* butter：2阶的巴特沃斯滤波器  后为上下截止频率 */
static void MiButterFilter(const array<double, 480> &data,
                           array<double, 480> &eegx){
    //5hz - 40hz
    static const array<double, 5> dv0 = {
         0.087179083712478234,
         0.000000000000000000,
        -0.174358167424956470,
         0.000000000000000000,
         0.087179083712478234
    };

    static const array<double, 5> dv1 = {
         1.000000000000000000,
        -2.871706305779434300,
         3.164559152923763100,
        -1.645569267464848300,
         0.357637958814908840
    };

    /* butter：2阶的巴特沃斯滤波器  后为上下截止频率 */
    array<double, 5> dbuffer;
    array<double, 60> y;
    for(int N = 0; N < 8; ++N){
        for(int k = 0; k < 4; ++k){
            dbuffer[k + 1] = 0.0;
        }

        for(int j = 0; j < 60; ++j){
            for(int k = 0; k < 4; ++k){
                dbuffer[k] = dbuffer[k + 1];
            }

            dbuffer[4] = 0.0;
            for(int k = 0; k < 5; ++k){
                dbuffer[k] += data[N + 8 * j] * dv0[k];
            }

            for(int k = 0; k < 4; ++k){
                dbuffer[k + 1] -= dbuffer[0] * dv1[k + 1];
            }

            y[j] = dbuffer[0];
        }

        for(int k = 0; k < 60; ++k){
            eegx[N + 8 * k] = y[k];
        }

        /*  filter：一维数字滤波器， x1是滤波前序列，B为分子，A为分母 */
    }
}

static void SsvepButterFilter(const array<double, 480> &data,
                              array<double, 480> &eegx){
  //5hz - 40hz
  static const array<double, 5> dv0 = {
       0.087179083712478234,
       0.000000000000000000,
      -0.174358167424956470,
       0.000000000000000000,
       0.087179083712478234
  };

  static const array<double, 5> dv1 = {
       1.00000000000000000,
      -2.87170630577943430,
       3.16455915292376310,
      -1.64556926746484830,
       0.35763795881490884
  };

  int offset = 0;
  array<double, 5> dbuffer;
  for(int c = 0; c < 60; ++c){
    for(int k = 0; k < 4; ++k){
      dbuffer[k + 1] = 0.0;
    }

    for(int j = 0; j < 8; ++j){
      int jp = j + offset;
      for(int k = 0; k < 4; ++k){
        dbuffer[k] = dbuffer[k + 1];
      }

      dbuffer[4] = 0.0;
      for(int k = 0; k < 5; ++k){
        dbuffer[k] += data[jp] * dv0[k];
      }

      for(int k = 0; k < 4; ++k){
        dbuffer[k + 1] -= dbuffer[0] * dv1[k + 1];
      }

      eegx[jp] = dbuffer[0];
    }

    offset += 8;
  }
  /*  filter：一维数字滤波器， x1是滤波前序列，B为分子，A为分母 */
}

CommonSpatialPattern::CommonSpatialPattern(
    const vector<double> &W1, const vector<double> &sample1,
    const vector<double> &W2, const vector<double> &sample2,
    const vector<double> &W3, const vector<double> &sample3):
W1(W1), W2(W2), W3(W3), sample1(sample1), sample2(sample2), sample3(sample3){

}

//0-空闲
//1-左
//2-右
//-1-啥也不干
int CspCca::step(ChannelSignal &getSignalDataAdress){
    if(idleCount >= 0){
        rawData.push_back(getSignalDataAdress);
    }

    ++idleCount;
    if(idleCount == WINDOW_LENTH * 30){
        array<double, 480> inputData, outputData;
        int tmpCount = 0;
        for(auto &data: rawData){
            inputData[tmpCount * 8 + 0] = data.Fp1.first;
            inputData[tmpCount * 8 + 1] = data.Fp2.first;
            inputData[tmpCount * 8 + 2] = data.C3.first;
            inputData[tmpCount * 8 + 3] = data.C4.first;
            inputData[tmpCount * 8 + 4] = data.P7.first;
            inputData[tmpCount * 8 + 5] = data.P8.first;
            inputData[tmpCount * 8 + 6] = data.O1.first;
            inputData[tmpCount * 8 + 7] = data.O2.first;
            ++tmpCount;
        }
        rawData.clear();
        idleCount = 0;

        /*******************/
        /*      SSVEP      */
        /*******************/
        if(getSignalDataAdress.MI_SSVEP == 0){
            SsvepButterFilter(inputData, outputData);

            double juiceClass;
            double maxJuice;
            me_CCA(outputData.data(), 7.75, 9.75, &maxJuice, &juiceClass);// ========核心算法入口=========
            qDebug() << "juice" << maxJuice << "   " << juiceClass;
            if(maxJuice > THRESHOLD){
                //设置标志位取消600个时间点的接受，已防止误判
                idleCount = -WINDOW_LENTH * 30;
                return juiceClass;
            }
            else{
                return 0;
            }
        }
        /*******************/
        /*        MI       */
        /*******************/
        else{//getSignalDataAdress.MI_SSVEP == 1
            MiButterFilter(inputData, outputData);

            int rec = csp_rec_ovo(outputData.data(), W1, sample1,
                                  W2, sample2, W3, sample3);
            qDebug() << "juice" << "  MI" << "   " << rec;
            //左-右-空闲 —— 1 2 3
            //-1 表示本次预测作废，做空闲处理
            if(rec == -1 || rec == 3){
                if(miCount > 3){ //6s
                    idleCount = -WINDOW_LENTH * 30;
                    miCount = 0;
                    return 0;
                }
                else{
                    ++miCount;
                    return -1;
                }
            }
            else{
                idleCount = -WINDOW_LENTH * 30;
                miCount = 0;
                return rec;
            }
        }
    }
    return 0;
}

Fftw::Fftw(): windowTranslationData(2, vector<double>(FFTW_DATA_SIZE, 0)){
    motorImageryObject = new motorImagery;
}

Fftw::~Fftw(){
    delete motorImageryObject;
}

int Fftw::step(ChannelSignal &getSignalDataAdress){
    ++fftwStoreSize;
    motorImageryObject->C3ChannelIn[fftwStoreSize][0] =
        getSignalDataAdress.C3.first;
    motorImageryObject->C4ChannelIn[fftwStoreSize][0] =
        getSignalDataAdress.C4.first;

    //第一次存储，将后半部分数据保存，供滑动窗口使用
    if(fftwStoreSize >= (FFTW_DATA_SIZE / 2) && firstRecvice){
        //C3
        windowTranslationData[0][fftwStoreSize - (FFTW_DATA_SIZE / 2)] =
            motorImageryObject->C3ChannelIn[fftwStoreSize][0];
        //C4
        windowTranslationData[1][fftwStoreSize - (FFTW_DATA_SIZE / 2)] =
            motorImageryObject->C4ChannelIn[fftwStoreSize][0];
    }

    //一组数据存储完毕
    if(fftwStoreSize >= FFTW_DATA_SIZE){
        //若不是第一次存储
        if(!firstRecvice){
            //将上次的后半窗口数据赋给本次前半窗口
            for(int i = 0; i < (FFTW_DATA_SIZE / 2); ++i){
                motorImageryObject->C3ChannelIn[i][0] =
                    windowTranslationData[0][i];
                motorImageryObject->C4ChannelIn[i][0] =
                    windowTranslationData[1][i];
            }
            //将这次的后半窗口存储，用于下次窗口滑动使用
            for(int i = 0; i < (FFTW_DATA_SIZE / 2); ++i){
                windowTranslationData[0][i] =
                    motorImageryObject->C3ChannelIn[i + (FFTW_DATA_SIZE / 2)][0];
                windowTranslationData[1][i] =
                    motorImageryObject->C4ChannelIn[i + (FFTW_DATA_SIZE / 2)][0];
            }
        }
        firstRecvice = false;
        //下次接受数据就从后半部开始存储，而下次窗口前半部使用这次的后半部，从而使得窗口每次移动1/2
        fftwStoreSize = FFTW_DATA_SIZE / 2;
        //进行滤波处理
        motorImageryObject->bufferFilter();
        //开始FFT变换
        motorImageryObject->fftwTransform();
        //求解功率谱密度
        motorImageryObject->powerSpectrChooseMethod(
            C3_POWER_SPECTRA_DIRCT_METHOD
          | C4_POWER_SPECTRA_DIRCT_METHOD);
        //ERD & ERS
        int E_State = motorImageryObject->distinguish_ERD_ERS();
        if(E_State == IMAGE_LEFT){
            C3flag++;
            C4flag = 0;
            Idleflag = 0;
        }
        else if(E_State == IMAGE_RIGHT){
            C4flag++;
            C3flag = 0;
            Idleflag = 0;
        }
        else if(E_State == IMAGE_IDLE){
            C3flag = 0;
            C4flag = 0;
            Idleflag++;
        }

        if(C3flag >= 2){
            C4flag = 0;
            C3flag = 0;
            Idleflag = 0;
            qDebug() << "left";
            return 1;
        }
        if(C4flag >= 2){
            C4flag = 0;
            C3flag = 0;
            Idleflag = 0;
            qDebug() << "right";
            return 2;
        }
        if(Idleflag >= 12){//6s
            C4flag = 0;
            C3flag = 0;
            Idleflag = 0;
            qDebug() << "idel";
            return 0;
        }
        return -1;
    }
    return 0;
}

//滑动窗口版本,算法处理的还是2s时间窗，但是1s判定一次
//懒得写了判断了，直接第一次1秒数据作废...想了想还是加上把...
int SwCspCca::step(ChannelSignal &getSignalDataAdress){
    if(idleCount >= 0){
        rawData.push_back(getSignalDataAdress);
    }

    ++idleCount;

    if(idleCount == 30){
        array<double, 480> inputData, outputData;
        int tmpCount = 0;
        if(firstRecFlag == 0){//第一次接受
            for(auto &data: rawData){
                tempInputData[tmpCount * 8 + 0] = data.Fp1.first;
                tempInputData[tmpCount * 8 + 1] = data.Fp2.first;
                tempInputData[tmpCount * 8 + 2] = data.C3.first;
                tempInputData[tmpCount * 8 + 3] = data.C4.first;
                tempInputData[tmpCount * 8 + 4] = data.P7.first;
                tempInputData[tmpCount * 8 + 5] = data.P8.first;
                tempInputData[tmpCount * 8 + 6] = data.O1.first;
                tempInputData[tmpCount * 8 + 7] = data.O2.first;
                ++tmpCount;
            }
            firstRecFlag = 1;
            rawData.clear();
            idleCount = 0;
            return -1;
        }
        else{
            for(auto &data: rawData){
                //前半部分数据
                for(int i = 0; i < 8; ++i){
                    inputData[tmpCount * 8 + i] =
                        tempInputData[tmpCount * 8 + i + 240];
                }

                //后半部分数据
                inputData[tmpCount * 8 + 0 + 240] = data.Fp1.first;
                inputData[tmpCount * 8 + 1 + 240] = data.Fp2.first;
                inputData[tmpCount * 8 + 2 + 240] = data.C3.first;
                inputData[tmpCount * 8 + 3 + 240] = data.C4.first;
                inputData[tmpCount * 8 + 4 + 240] = data.P7.first;
                inputData[tmpCount * 8 + 5 + 240] = data.P8.first;
                inputData[tmpCount * 8 + 6 + 240] = data.O1.first;
                inputData[tmpCount * 8 + 7 + 240] = data.O2.first;

                //存储后半部分数据供下次使用
                for(int i = 0; i < 8; ++i){
                    tempInputData[tmpCount * 8 + i] =
                        inputData[tmpCount * 8 + i + 240];
                }
                ++tmpCount;
            }
        }

        rawData.clear();
        idleCount = 0;

        /*******************/
        /*      SSVEP      */
        /*******************/
        if(getSignalDataAdress.MI_SSVEP == 0){
            SsvepButterFilter(inputData, outputData);

            double juiceClass;
            double maxJuice;
            me_CCA(outputData.data(), 7.75, 9.75, &maxJuice, &juiceClass);
            qDebug() << "juice" << maxJuice << "   " << juiceClass;
            if(maxJuice > THRESHOLD){
                //设置标志位取消600个时间点的接受，已防止误判
                idleCount = -WINDOW_LENTH * 30;
                //重新记录300个时间点，也就是判断之后4秒才会出下一个指令
                firstRecFlag = 0;
                return juiceClass;
            }
            else{
                return 0;
            }
        }
        /*******************/
        /*        MI       */
        /*******************/
        else{//getSignalDataAdress.MI_SSVEP == 1
            MiButterFilter(inputData, outputData);

            double rec = csp_rec_ovo(outputData.data(), W1, sample1,
                                     W2, sample2, W3, sample3);
            qDebug() << "juice  MI   " << rec;
            //左-右-空闲 —— 1 2 3
            //-1 表示本次预测作废，做空闲处理
            if(rec == -1 || rec == 3){
                if(miCount > 6){//6s表示空闲退出
                    //退出运动想象状态，先跳过600个时间点，防止误判
                    idleCount = -WINDOW_LENTH * 30;
                    //重新记录300个时间点，也就是判断之后4秒才会出下一个指令
                    firstRecFlag = 0;
                    miCount = 0;
                    return 0;
                }
                else{
                    ++miCount;
                    return -1;
                }
            }
            else{
                miCount = 0;
                return rec;
            }
        }
    }
    return 0;
}

/*以下为新增2020-8-21 19:41:44*/
//0-空闲
//1-左
//2-右
//-1-啥也不干
int CspKcca::step(ChannelSignal &getSignalDataAdress){
    if(idleCount >= 0){
        rawData.push_back(getSignalDataAdress);
    }

    ++idleCount;

    if(idleCount == WINDOW_LENTH * 30){
        array<double, 480> inputData, outputData;
        int tmpCount = 0;
        for(auto &data: rawData){
            inputData[tmpCount * 8 + 0] = data.Fp1.first;
            inputData[tmpCount * 8 + 1] = data.Fp2.first;
            inputData[tmpCount * 8 + 2] = data.C3.first;
            inputData[tmpCount * 8 + 3] = data.C4.first;
            inputData[tmpCount * 8 + 4] = data.P7.first;
            inputData[tmpCount * 8 + 5] = data.P8.first;
            inputData[tmpCount * 8 + 6] = data.O1.first;
            inputData[tmpCount * 8 + 7] = data.O2.first;
            ++tmpCount;
        }
        rawData.clear();
        idleCount = 0;

        /*******************/
        /*      SSVEP      */
        /*******************/
        if(!getSignalDataAdress.MI_SSVEP){
            SsvepButterFilter(inputData, outputData);

            double juiceClass;
            double maxJuice;
            me_KCCA(outputData.data(), 7.75, 9.75, &maxJuice, &juiceClass);// ========核心算法入口=========

            qDebug() << "juice" << maxJuice << "   " << juiceClass;
            if(maxJuice > THRESHOLD){
                //设置标志位取消600个时间点的接受，已防止误判
                idleCount = -WINDOW_LENTH * 30;
                return juiceClass;
            }
            else{
                return 0;
            }
        }
        /*******************/
        /*        MI       */
        /*******************/
        else{//getSignalDataAdress.MI_SSVEP == 1
            MiButterFilter(inputData, outputData);

            int rec = csp_rec_ovo(outputData.data(), W1, sample1,
                                  W2, sample2, W3, sample3);
            qDebug() << "juice  MI   " << rec;
            //左-右-空闲 —— 1 2 3
            //-1 表示本次预测作废，做空闲处理
            if(rec == -1 || rec == 3){
                if(miCount > 3){//6s
                    idleCount = -WINDOW_LENTH * 30;
                    miCount = 0;
                    return 0;
                }
                else{
                    ++miCount;
                    return -1;
                }
            }
            else{
                idleCount = -WINDOW_LENTH * 30;
                miCount = 0;
                return rec;
            }
        }
    }
    return 0;
}

//滑动窗口版本,算法处理的还是2s时间窗，但是1s判定一次
//懒得写了判断了，直接第一次1秒数据作废...想了想还是加上把...
int SwCspKcca::step(ChannelSignal &getSignalDataAdress){
    if(idleCount >= 0){
        rawData.push_back(getSignalDataAdress);
    }

    ++idleCount;

    if(idleCount == 30){
        array<double, 480> inputData,outputData;
        int tmpCount = 0;
        if(!firstRecFlag){//第一次接受
            for(auto &data: rawData){
                tempInputData[tmpCount * 8 + 0] = data.Fp1.first;
                tempInputData[tmpCount * 8 + 1] = data.Fp2.first;
                tempInputData[tmpCount * 8 + 2] = data.C3.first;
                tempInputData[tmpCount * 8 + 3] = data.C4.first;
                tempInputData[tmpCount * 8 + 4] = data.P7.first;
                tempInputData[tmpCount * 8 + 5] = data.P8.first;
                tempInputData[tmpCount * 8 + 6] = data.O1.first;
                tempInputData[tmpCount * 8 + 7] = data.O2.first;
                ++tmpCount;
            }
            firstRecFlag = 1;
            rawData.clear();
            idleCount = 0;
            return -1;
        }
        else{
            for(auto &data: rawData){
                //前半部分数据
                for(int i = 0; i < 8; ++i){
                    inputData[tmpCount * 8 + i] =
                        tempInputData[tmpCount * 8 + i + 240];
                }

                //后半部分数据
                inputData[tmpCount * 8 + 0 + 240] = data.Fp1.first;
                inputData[tmpCount * 8 + 1 + 240] = data.Fp2.first;
                inputData[tmpCount * 8 + 2 + 240] = data.C3.first;
                inputData[tmpCount * 8 + 3 + 240] = data.C4.first;
                inputData[tmpCount * 8 + 4 + 240] = data.P7.first;
                inputData[tmpCount * 8 + 5 + 240] = data.P8.first;
                inputData[tmpCount * 8 + 6 + 240] = data.O1.first;
                inputData[tmpCount * 8 + 7 + 240] = data.O2.first;

                //存储后半部分数据供下次使用
                for(int i = 0; i < 8; ++i){
                    tempInputData[tmpCount * 8 + i] =
                        inputData[tmpCount * 8 + i + 240];
                }
                ++tmpCount;
            }
        }

        rawData.clear();
        idleCount = 0;

        /*******************/
        /*      SSVEP      */
        /*******************/
        if(getSignalDataAdress.MI_SSVEP == 0){
            SsvepButterFilter(inputData, outputData);

            double juiceClass;
            double maxJuice;
            me_KCCA(outputData.data(), 7.75, 9.75, &maxJuice, &juiceClass);
            qDebug() << "juice" << maxJuice << "   " << juiceClass;
            if(maxJuice > THRESHOLD){
                //设置标志位取消600个时间点的接受，已防止误判
                idleCount = -WINDOW_LENTH * 30;
                //重新记录300个时间点，也就是判断之后4秒才会出下一个指令
                firstRecFlag = 0;
                return juiceClass;
            }
            else{
                return 0;
            }
        }
        /*******************/
        /*        MI       */
        /*******************/
        else{//getSignalDataAdress.MI_SSVEP == 1
            MiButterFilter(inputData, outputData);

            double rec = csp_rec_ovo(outputData.data(), W1, sample1,
                                     W2, sample2, W3, sample3);
            qDebug() << "juice  MI   " << rec;
            //左-右-空闲 —— 1 2 3
            //-1 表示本次预测作废，做空闲处理
            if(rec == -1 || rec == 3){
                if(miCount > 6){//6s表示空闲退出
                    //退出运动想象状态，先跳过600个时间点，防止误判
                    idleCount = -WINDOW_LENTH * 30;
                    //重新记录300个时间点，也就是判断之后4秒才会出下一个指令
                    firstRecFlag = 0;
                    miCount = 0;
                    return 0;
                }
                else{
                    ++miCount;
                    return -1;
                }
            }
            else{
                miCount = 0;
                return rec;
            }
        }
    }
    return 0;
}
