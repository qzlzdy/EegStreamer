#include "OpenBCI/StateCommand.h"

static int lastState = 0;

StandMode::StandMode(){
    nowState = 0;
}

StateCommand* StandMode::Operation(const EEG_STATE& data,
    rehabilitativeUsart *, Paradigm *para, Dialog *){
    if(data == MI_LEFT){
        para->ModeOne();
        return new OneMode;
    }
    else if(data == MI_RIGHT){
        para->ModeTwo();
        return new TwoMode;
    }
    return nullptr;
}


OneMode::OneMode(){
    nowState = 0;
}

StateCommand* OneMode::Operation(const EEG_STATE& data,
    rehabilitativeUsart *reh, Paradigm *para, Dialog *){
    if(data == MI_LEFT){
        reh->sendToBend(0x1A);
        para->DownMode();
        return new DownMode;
    }
    else if(data == MI_RIGHT){
        reh->sendToBend(0x1B);
        para->UpMode();
        return new UpMode;
    }
    return nullptr;
}


DownMode::DownMode(){
    nowState = 0;
}

StateCommand* DownMode::Operation(const EEG_STATE& data,
    rehabilitativeUsart *reh, Paradigm *para, Dialog *){
    if(data == MI_LEFT){
        return nullptr;
    }
    else if(data == MI_RIGHT){
        reh->sendToBend(0xA1);
        para->StandMode();
        return new StandMode;
    }
    return nullptr;
}


UpMode::UpMode(){
    nowState = 0;
}

StateCommand* UpMode::Operation(const EEG_STATE& data,
    rehabilitativeUsart *reh, Paradigm *para, Dialog *){
    if(data == MI_LEFT){
        return nullptr;
    }
    else if(data == MI_RIGHT){
        reh->sendToBend(0xB1);
        para->StandMode();
        return new StandMode;
    }
    return nullptr;
}


TwoMode::TwoMode(){
    nowState = 0;
}

StateCommand* TwoMode::Operation(const EEG_STATE& data,
    rehabilitativeUsart *reh, Paradigm *para, Dialog *){
    if(data == MI_LEFT){
        reh->sendToBend(0x1D);
        para->WalkMode();
        return new WalkMode;
    }
    else if(data == MI_RIGHT){
        reh->sendToBend(0x1C);
        para->SitMode();
        return new SitMode;
    }
    return nullptr;
}


SitMode::SitMode(){
    nowState = 0;
}

StateCommand* SitMode::Operation(const EEG_STATE& data,
    rehabilitativeUsart *reh, Paradigm *para, Dialog *){
    if(data == MI_LEFT){
        return nullptr;
    }
    else if(data == MI_RIGHT){
        reh->sendToBend(0xC1);
        para->StandMode();
        return new StandMode;
    }
    return nullptr;
}


WalkMode::WalkMode(){
    nowState = 0;
}

StateCommand* WalkMode::Operation(const EEG_STATE& data,
    rehabilitativeUsart *reh, Paradigm *para, Dialog *da){
    if(data == MI_LEFT){
        lastState = STAND_MODE;
        da->threadC1();
        //切换到运动想象界面
        da->toMI();
        //播放左脚视频
        da->startVideoLeft();
        return new MiMode;
    }
    else if(data == MI_RIGHT){
        reh->sendToBend(0xD1);
        para->StandMode();
        return new StandMode;
    }
    return nullptr;
}

MiMode::MiMode(){
    nowState = 1;
}

StateCommand* MiMode::Operation(const EEG_STATE& data,
    rehabilitativeUsart *reh, Paradigm *, Dialog *da){
    if(data == MI_LEFT){
        if(lastState == STAND_MODE){
            reh->sendToBend(0x1E);
        }
        else{
            reh->sendToBend(0xFE);
        }
        lastState = MI_MODE_Left;
        //提示左想象 成功
        da->printToLable("Left");
        //切换到右-视频
        da->startVideoRight();
        return nullptr;
    }
    else if(data == MI_RIGHT){
        if(lastState == STAND_MODE){
            reh->sendToBend(0x1F);
        }
        else{
            reh->sendToBend(0xEF);
        }
        lastState = MI_MODE_Right;
        //提示右-想象 成功
        da->printToLable("Right");
        //切换到左-视频
        da->startVideoLeft();
        return nullptr;
    }
    else if(data == MI_IDLE){
        if(lastState == MI_MODE_Left){
            reh->sendToBend(0xE1);
        }
        else{
            reh->sendToBend(0xF1);
        }
        lastState = STAND_MODE;
        da->threadC2();
        //切换到SSVEP界面
        da->toSSVEP();
        return new WalkMode;
    }
    return nullptr;
}
