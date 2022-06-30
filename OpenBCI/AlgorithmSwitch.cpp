#include "OpenBCI/AlgorithmSwitch.h"

#define THRESHOLD 0.42

using namespace std;
using namespace ehdu;

void butter_filter_MI(const real_T data[12600], real_T eegx[12600]);
void butter_filter_SSVEP(const real_T data[12600], real_T eegx[12600]);

CspCcaAlgorithmSwitch::CspCcaAlgorithmSwitch(
    vector<double> W1, vector<double> sample1, vector<double> W2,
    vector<double> sample2, vector<double> W3, vector<double> sample3):
countIdle(0), mi_count(0), _W1(begin(W1), end(W1)), _W2(begin(W2),end(W2)),
_W3(begin(W3), end(W3)), _sample1(begin(sample1), end(sample1)),
_sample2(begin(sample2), end(sample2)), _sample3(begin(sample3), end(sample3)){

}

//0-空闲
//1-左
//2-右
//-1-啥也不干
int CspCcaAlgorithmSwitch::step(ChannelSignal &getSignalDataAdress){
    if(countIdle >= 0){
        rawData.push_back(getSignalDataAdress);
    }

    ++countIdle;
    if(countIdle == WINDOW_LENTH * 300){
        double inputData[12600], outputData[12600];
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
        countIdle = 0;

        /*******************/
        /*      SSVEP      */
        /*******************/
        if(getSignalDataAdress.MI_SSVEP == 0){
            butter_filter_SSVEP(inputData, outputData);

            double juiceClass;
            double maxJuice;
            me_CCA(outputData, 7.75, 9.75, &maxJuice, &juiceClass);// ========核心算法入口=========
            qDebug() << "juice" << maxJuice << "   " << juiceClass;
            if(maxJuice > THRESHOLD){
                //设置标志位取消600个时间点的接受，已防止误判
                countIdle = -WINDOW_LENTH*300;
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
            butter_filter_MI(inputData, outputData);

            int rec = csp_rec_ovo(outputData, _W1, _sample1, _W2, _sample2, _W3, _sample3);
            qDebug() << "juice" << "  MI" << "   " << rec;
            //左-右-空闲 —— 1 2 3
            //-1 表示本次预测作废，做空闲处理
            if(rec == -1 || rec == 3){
                if(mi_count > 3){ //6s
                    countIdle = -WINDOW_LENTH * 300;
                    mi_count=0;
                    return 0;
                }
                else{
                    ++mi_count;
                    return -1;
                }
            }
            else{
                countIdle = -WINDOW_LENTH * 300;
                mi_count=0;
                return rec;
            }
        }
    }
    return 0;
}

FftwAlgorithmSwitch::FftwAlgorithmSwitch():
windowTranslationData(2, vector<double>(FFTW_DATA_SIZE, 0)){
    C3flag = 0;
    C4flag = 0;
    Idlefalsg = 0;
    fftwStoreSize = -1;
    motorImageryObject = new motorImagery;
}

FftwAlgorithmSwitch::~FftwAlgorithmSwitch(){
    delete motorImageryObject;
    motorImageryObject = nullptr;
}

int FftwAlgorithmSwitch::step(ChannelSignal &getSignalDataAdress){
    ++fftwStoreSize;
    motorImageryObject->C3ChannelIn[fftwStoreSize][0] =
        getSignalDataAdress.C3.first;
    motorImageryObject->C4ChannelIn[fftwStoreSize][0] =
        getSignalDataAdress.C4.first;

    //第一次存储，将后半部分数据保存，供滑动窗口使用
    if(fftwStoreSize >= (FFTW_DATA_SIZE / 2) && firstRecviceDataFlag == true){
        //C3
        windowTranslationData[0][fftwStoreSize-(FFTW_DATA_SIZE/2)] = motorImageryObject->C3ChannelIn[fftwStoreSize][0];
        //C4
        windowTranslationData[1][fftwStoreSize-(FFTW_DATA_SIZE/2)] = motorImageryObject->C4ChannelIn[fftwStoreSize][0];
    }

    //一组数据存储完毕
    if(fftwStoreSize >= FFTW_DATA_SIZE)
    {
        //若不是第一次存储
        if(firstRecviceDataFlag == false)
        {
            //将上次的后半窗口数据赋给本次前半窗口
            for(int i=0;i<(FFTW_DATA_SIZE/2);i++)
            {
                motorImageryObject->C3ChannelIn[i][0] = windowTranslationData[0][i];
                motorImageryObject->C4ChannelIn[i][0] = windowTranslationData[1][i];
            }
            //将这次的后半窗口存储，用于下次窗口滑动使用
            for(int i = 0; i < (FFTW_DATA_SIZE / 2); ++i){
                windowTranslationData[0][i] =
                    motorImageryObject->C3ChannelIn[i + (FFTW_DATA_SIZE / 2)][0];
                windowTranslationData[1][i] =
                    motorImageryObject->C4ChannelIn[i + (FFTW_DATA_SIZE / 2)][0];
            }
        }
        firstRecviceDataFlag = false;
        //下次接受数据就从后半部开始存储，而下次窗口前半部使用这次的后半部，从而使得窗口每次移动1/2
        fftwStoreSize = FFTW_DATA_SIZE / 2;
        //进行滤波处理
        motorImageryObject->bufferFilter();
        //开始FFT变换
        motorImageryObject->fftwTransform();
        //求解功率谱密度
        motorImageryObject->powerSpectrChooseMethod(C3_POWER_SPECTRA_DIRCT_METHOD | C4_POWER_SPECTRA_DIRCT_METHOD);
        //ERD & ERS
        int E_State = motorImageryObject->distinguish_ERD_ERS();
        if(E_State == IMAGE_LEFT){
            C3flag++;
            C4flag = 0;
            Idlefalsg = 0;
        }
        else if(E_State == IMAGE_RIGHT){
            C4flag++;
            C3flag = 0;
            Idlefalsg = 0;
        }
        else if(E_State == IMAGE_IDLE){
            C3flag = 0;
            C4flag = 0;
            Idlefalsg++;
        }

        if(C3flag >= 2){
            C4flag = 0;
            C3flag = 0;
            Idlefalsg = 0;
            qDebug() << "left";
            return 1;
        }
        if(C4flag >= 2){
            C4flag = 0;
            C3flag = 0;
            Idlefalsg = 0;
            qDebug() << "right";
            return 2;
        }
        if(Idlefalsg >= 12){//6s
            C4flag = 0;
            C3flag = 0;
            Idlefalsg = 0;
            qDebug() << "idel";
            return 0;
        }
        return -1;
    }
    return 0;
}

SwCspCcaAlgorithmSwitch::SwCspCcaAlgorithmSwitch(vector<double> W1,
    vector<double> sample1, vector<double> W2, vector<double> sample2,
    vector<double> W3, vector<double> sample3):
firstRecFlag(0), countIdle(0), mi_count(0), _W1(begin(W1), end(W1)),
_W2(begin(W2), end(W2)), _W3(begin(W3), end(W3)),
_sample1(begin(sample1), end(sample1)), _sample2(begin(sample2), end(sample2)),
_sample3(begin(sample3), end(sample3)){

}

//滑动窗口版本,算法处理的还是2s时间窗，但是1s判定一次
//懒得写了判断了，直接第一次1秒数据作废...想了想还是加上把...
int SwCspCcaAlgorithmSwitch::step(ChannelSignal &getSignalDataAdress){
    if(countIdle >= 0){
        rawData.push_back(getSignalDataAdress);
    }

    ++countIdle;

    if(countIdle == 300){
        double inputData[12600],outputData[12600];
        int tmpCount = 0;
        if(firstRecFlag==0){//第一次接受
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
            countIdle = 0;
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
        countIdle = 0;

        /*******************/
        /*      SSVEP      */
        /*******************/
        if(getSignalDataAdress.MI_SSVEP == 0){
            butter_filter_SSVEP(inputData, outputData);

            double juiceClass;
            double maxJuice;
            me_CCA(outputData, 7.75, 9.75, &maxJuice, &juiceClass);
            qDebug() << "juice" << maxJuice << "   " << juiceClass;
            if(maxJuice > THRESHOLD){
                //设置标志位取消600个时间点的接受，已防止误判
                countIdle = -WINDOW_LENTH * 300;
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
            butter_filter_MI(inputData, outputData);

            double rec = csp_rec_ovo(outputData, _W1, _sample1, _W2, _sample2, _W3, _sample3);
            qDebug() << "juice" << "  MI" << "   " << rec;
            //左-右-空闲 —— 1 2 3
            //-1 表示本次预测作废，做空闲处理
            if(rec == -1 || rec == 3){
                if(mi_count > 6){//6s表示空闲退出
                    //退出运动想象状态，先跳过600个时间点，防止误判
                    countIdle = -WINDOW_LENTH * 300;
                    //重新记录300个时间点，也就是判断之后4秒才会出下一个指令
                    firstRecFlag = 0;
                    mi_count = 0;
                    return 0;
                }
                else{
                    ++mi_count;
                    return -1;
                }
            }
            else{
                mi_count = 0;
                return rec;
            }
        }
    }
    return 0;
}

//滤波器
void butter_filter_MI(const double data[12600], double eegx[12600]){
    int N;
    double dbuffer[5];
    int k;
    double y[600];
    int j;

    //5hz - 40hz
    static const double dv0[5] = { 0.087179083712478234, 0.0, -0.17435816742495647,
        0.0, 0.087179083712478234 };

    static const double dv1[5] = { 1.0, -2.8717063057794343, 3.1645591529237631,
        -1.6455692674648483, 0.35763795881490884 };

    /* butter：2阶的巴特沃斯滤波器  后为上下截止频率 */
    for (N = 0; N < 21; N++) {
        for (k = 0; k < 4; k++) {
            dbuffer[k + 1] = 0.0;
        }

        for (j = 0; j < 600; j++) {
            for (k = 0; k < 4; k++) {
                dbuffer[k] = dbuffer[k + 1];
            }

            dbuffer[4] = 0.0;
            for (k = 0; k < 5; k++) {
                dbuffer[k] += data[N + 21 * j] * dv0[k];
            }

            for (k = 0; k < 4; k++) {
                dbuffer[k + 1] -= dbuffer[0] * dv1[k + 1];
            }

            y[j] = dbuffer[0];
        }

        for (k = 0; k < 600; k++) {
            eegx[N + 21 * k] = y[k];
        }

        /*  filter：一维数字滤波器， x1是滤波前序列，B为分子，A为分母 */
    }
}

void butter_filter_SSVEP(const real_T data[12600], real_T eegx[12600]){
  int32_T offset;
  int32_T c;
  real_T dbuffer[5];
  int32_T k;
  int32_T j;
  int32_T jp;

  //5hz - 40hz
  static const double dv0[5] = { 0.087179083712478234, 0.0, -0.17435816742495647,
      0.0, 0.087179083712478234 };

  static const double dv1[5] = { 1.0, -2.8717063057794343, 3.1645591529237631,
      -1.6455692674648483, 0.35763795881490884 };


  /* data ---  要处理的数据 */
  /* Fs   ---  采样率 */
  /* eegx ---  返回的数据 */
  /* butter：2阶的巴特沃斯滤波器  后为上下截止频率 */
  offset = 0;
  for (c = 0; c < 600; c++) {
    for (k = 0; k < 4; k++) {
      dbuffer[k + 1] = 0.0;
    }

    for (j = 0; j < 21; j++) {
      jp = j + offset;
      for (k = 0; k < 4; k++) {
        dbuffer[k] = dbuffer[k + 1];
      }

      dbuffer[4] = 0.0;
      for (k = 0; k < 5; k++) {
        dbuffer[k] += data[jp] * dv0[k];
      }

      for (k = 0; k < 4; k++) {
        dbuffer[k + 1] -= dbuffer[0] * dv1[k + 1];
      }

      eegx[jp] = dbuffer[0];
    }

    offset += 21;
  }
  /*  filter：一维数字滤波器， x1是滤波前序列，B为分子，A为分母 */
}

/*以下为新增2020-8-21 19:41:44*/
CspKccaAlgorithmSwitch::CspKccaAlgorithmSwitch(vector<double> W1,
    vector<double> sample1, vector<double> W2, vector<double> sample2,
    vector<double> W3, vector<double> sample3):
countIdle(0), mi_count(0), _W1(begin(W1), end(W1)), _W2(begin(W2), end(W2)),
_W3(begin(W3), end(W3)), _sample1(begin(sample1), end(sample1)),
_sample2(begin(sample2), end(sample2)), _sample3(begin(sample3), end(sample3)){

}

//0-空闲
//1-左
//2-右
//-1-啥也不干
int CspKccaAlgorithmSwitch::step(ChannelSignal &getSignalDataAdress){
    if(countIdle >= 0){
        rawData.push_back(getSignalDataAdress);
    }

    ++countIdle;

    if(countIdle == WINDOW_LENTH * 300){
        double inputData[12600], outputData[12600];
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
        countIdle = 0;

        /*******************/
        /*      SSVEP      */
        /*******************/
        if(!getSignalDataAdress.MI_SSVEP){
            butter_filter_SSVEP(inputData, outputData);

            double juiceClass;
            double maxJuice;
            me_KCCA(outputData, 7.75, 9.75, &maxJuice, &juiceClass);// ========核心算法入口=========

            qDebug() << "juice" << maxJuice << "   " << juiceClass;
            if(maxJuice > THRESHOLD){
                //设置标志位取消600个时间点的接受，已防止误判
                countIdle = -WINDOW_LENTH * 300;
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
            butter_filter_MI(inputData, outputData);

            int rec = csp_rec_ovo(outputData, _W1, _sample1, _W2, _sample2, _W3, _sample3);
            qDebug() << "juice  MI   " << rec;
            //左-右-空闲 —— 1 2 3
            //-1 表示本次预测作废，做空闲处理
            if(rec == -1 || rec == 3){
                if(mi_count > 3){//6s
                    countIdle = -WINDOW_LENTH * 300;
                    mi_count=0;
                    return 0;
                }
                else{
                    ++mi_count;
                    return -1;
                }
            }
            else{
                countIdle = -WINDOW_LENTH * 300;
                mi_count=0;
                return rec;
            }
        }
    }
    return 0;
}

SwCspKccaAlgorithmSwitch::SwCspKccaAlgorithmSwitch(vector<double> W1,
    vector<double> sample1, vector<double> W2, vector<double> sample2,
    vector<double> W3, vector<double> sample3):
firstRecFlag(0), countIdle(0), mi_count(0), _W1(begin(W1), end(W1)),
_W2(begin(W2), end(W2)), _W3(begin(W3), end(W3)),
_sample1(begin(sample1), end(sample1)), _sample2(begin(sample2), end(sample2)),
_sample3(begin(sample3), end(sample3)){

}

//滑动窗口版本,算法处理的还是2s时间窗，但是1s判定一次
//懒得写了判断了，直接第一次1秒数据作废...想了想还是加上把...
int SwCspKccaAlgorithmSwitch::step(ChannelSignal &getSignalDataAdress){
    if(countIdle >= 0){
        rawData.push_back(getSignalDataAdress);
    }

    ++countIdle;

    if(countIdle == 300){
        double inputData[12600],outputData[12600];
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
            countIdle = 0;
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
        countIdle = 0;

        /*******************/
        /*      SSVEP      */
        /*******************/
        if(getSignalDataAdress.MI_SSVEP == 0)
        {
            butter_filter_SSVEP(inputData, outputData);

            double juiceClass;
            double maxJuice;
            me_KCCA(outputData, 7.75, 9.75, &maxJuice, &juiceClass);
            qDebug()<<"juice"<<maxJuice<<"   "<<juiceClass;
            if(maxJuice > THRESHOLD)
            {
                //设置标志位取消600个时间点的接受，已防止误判
                countIdle = -WINDOW_LENTH*300;
                //重新记录300个时间点，也就是判断之后4秒才会出下一个指令
                firstRecFlag = 0;
                return juiceClass;
            }else{
                return 0;
            }
        }
        /*******************/
        /*        MI       */
        /*******************/
        else//getSignalDataAdress.MI_SSVEP == 1
        {
            butter_filter_MI(inputData, outputData);

            double rec = csp_rec_ovo(outputData, _W1, _sample1, _W2, _sample2, _W3, _sample3);
            qDebug()<<"juice"<<"  MI"<<"   "<<rec;
            //左-右-空闲 —— 1 2 3
            //-1 表示本次预测作废，做空闲处理
            if(rec==-1 || rec==3)
            {
                if(mi_count > 6)//6s表示空闲退出
                {
                    //退出运动想象状态，先跳过600个时间点，防止误判
                    countIdle = -WINDOW_LENTH * 300;
                    //重新记录300个时间点，也就是判断之后4秒才会出下一个指令
                    firstRecFlag = 0;
                    mi_count = 0;
                    return 0;
                }else{
                    ++mi_count;
                    return -1;
                }
            }else{
                mi_count = 0;
                return rec;
            }
        }
    }
    return 0;
}
