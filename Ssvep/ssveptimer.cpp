#include "Ssvep/ssveptimer.h"

using namespace std;
using namespace ehdu;

SsvepTimer::SsvepTimer(const chrono::nanoseconds &cycle){
    timer = new QElapsedTimer;
    threshold = cycle.count();
    flashFlag = true;
}

void SsvepTimer::run(){
//    setPriority(TimeCriticalPriority);
    timer->start();
    while(flashFlag){
        if(timer->nsecsElapsed() >= threshold){
            timer->restart();
            emit timeout();
        }
    }
}
