#ifndef DATAFORM_H
#define DATAFORM_H

#include <QPair>
#include <iostream>
#include <QString>

//typedef QPair<double, QString> cData;
typedef std::pair<double, QString> cData;


#define FFTW_DATA_SIZE 300
#define C3_POWER_SPECTRA_DIRCT_METHOD 1
#define C4_POWER_SPECTRA_DIRCT_METHOD 2
#define C3_POWER_SPECTRA_AR_METHOD 4
#define C4_POWER_SPECTRA_AR_METHOD 8
#define MAX_FREQUENCY 13
#define MIN_FREQUENCY 8
#define IMAGE_LEFT  0
#define IMAGE_RIGHT 1
#define IMAGE_IDLE 2
#define IMAGE_JUICE 2.5

#define EEG_STATE int
#define MI_LEFT 0
#define MI_RIGHT 1
#define MI_IDLE 2
#define SSVEP_LEFT 3
#define SSVEP_RIGHT 4
#define SSVEP_IDLE 5

#define PERSON_STATE int
#define STAND_MODE 1
#define MODE_ONE 2
#define MODE_TWO 3
#define DOWN_MODE 4
#define UP_MODE 5
#define WALK_MODE 6
#define SIT_MODE 7
#define MI_MODE 8
#define MI_MODE_Left 9
#define MI_MODE_Right 10

#define WINDOW_LENTH 2

struct channelSignal{
    cData Fp1,Fp2,F7,F3,Fz,F4,F8,A1,T3,C3,Cz,C4,T4,A2,T5,P3,Pz,P4,T6,O1,O2,X1,X2,X3;
    int channelNum;
    bool MI_SSVEP;
    channelSignal()
    {
        Fp1.second = "Fp1";
        Fp2.second = "Fp2";
        F7.second = "F7";
        F3.second = "F3";
        Fz.second = "Fz";
        F4.second = "F4";
        F8.second = "F8";
        A1.second = "A1";
        T3.second = "T3";
        C3.second = "C3";
        Cz.second = "Cz";
        C4.second = "C4";
        T4.second = "T4";
        A2.second = "A2";
        T5.second = "T5";
        P3.second = "P3";
        Pz.second = "Pz";
        P4.second = "P4";
        T6.second = "T6";
        O1.second = "O1";
        O2.second = "O2";
        X1.second = "X1";
        X2.second = "X2";
        X3.second = "X3";
        channelNum = 24;
    }
};

struct channelImpedance{
    cData Fp1,Fp2,F7,F3,Fz,F4,F8,A1,T3,C3,Cz,C4,T4,A2,T5,P3,Pz,P4,T6,O1,O2,X1,X2,X3;
    channelImpedance()
    {
        Fp1.second = "Fp1";
        Fp2.second = "Fp2";
        F7.second = "F7";
        F3.second = "F3";
        Fz.second = "Fz";
        F4.second = "F4";
        F8.second = "F8";
        A1.second = "A1";
        T3.second = "T3";
        C3.second = "C3";
        Cz.second = "Cz";
        C4.second = "C4";
        T4.second = "T4";
        A2.second = "A2";
        T5.second = "T5";
        P3.second = "P3";
        Pz.second = "Pz";
        P4.second = "P4";
        T6.second = "T6";
        O1.second = "O1";
        O2.second = "O2";
        X1.second = "X1";
        X2.second = "X2";
        X3.second = "X3";
        Fp1.first=Fp2.first=F7.first=F3.first=Fz.first=F4.first=F8.first=\
        A1.first=T3.first=C3.first=Cz.first=C4.first=T4.first=A2.first=T5.first=\
        P3.first=Pz.first=P4.first=T6.first=O1.first=O2.first=X1.first=X2.first=X3.first = 0;
    }
};

#endif // DATAFORM_H
