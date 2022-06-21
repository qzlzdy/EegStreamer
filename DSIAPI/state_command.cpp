#include "state_command.h"

static int lastState = 0;

State_Command::State_Command(){}
State_Command::~State_Command(){}


Stand_Mode::Stand_Mode(){ nowState = 0; }
Stand_Mode::~Stand_Mode(){}
State_Command* Stand_Mode::Operation(const EEG_STATE& data, rehabilitativeUsart *reh, Paradigm *para, Dialog *da)
{
    if(data == MI_LEFT){
        para->ModeOne();
        return new One_Mode;
    }else if(data == MI_RIGHT){
        para->ModeTwo();
        return new Two_Mode;
    }
    return nullptr;
}


One_Mode::One_Mode(){ nowState = 0; }
One_Mode::~One_Mode(){}
State_Command* One_Mode::Operation(const EEG_STATE& data, rehabilitativeUsart *reh, Paradigm *para, Dialog *da)
{
    if(data == MI_LEFT){
        reh->sendToBend(0x1A);
        para->DownMode();
        return new Down_Mode;
    }else if(data == MI_RIGHT){
        reh->sendToBend(0x1B);
        para->UpMode();
        return new Up_Mode;
    }
    return nullptr;
}


Down_Mode::Down_Mode(){ nowState = 0; }
Down_Mode::~Down_Mode(){}
State_Command* Down_Mode::Operation(const EEG_STATE& data, rehabilitativeUsart *reh, Paradigm *para, Dialog *da)
{
    if(data == MI_LEFT){
        return nullptr;
    }else if(data == MI_RIGHT){
        reh->sendToBend(0xA1);
        para->StandMode();
        return new Stand_Mode;
    }
    return nullptr;
}


Up_Mode::Up_Mode(){ nowState = 0; }
Up_Mode::~Up_Mode(){}
State_Command* Up_Mode::Operation(const EEG_STATE& data, rehabilitativeUsart *reh, Paradigm *para, Dialog *da)
{
    if(data == MI_LEFT){
        return nullptr;
    }else if(data == MI_RIGHT){
        reh->sendToBend(0xB1);
        para->StandMode();
        return new Stand_Mode;
    }
    return nullptr;
}


Two_Mode::Two_Mode(){ nowState = 0; }
Two_Mode::~Two_Mode(){}
State_Command* Two_Mode::Operation(const EEG_STATE& data, rehabilitativeUsart *reh, Paradigm *para, Dialog *da)
{
    if(data == MI_LEFT){
        reh->sendToBend(0x1D);
        para->WalkMode();
        return new Walk_Mode;
    }else if(data == MI_RIGHT){
        reh->sendToBend(0x1C);
        para->SitMode();
        return new Sit_Mode;
    }
    return nullptr;
}


Sit_Mode::Sit_Mode(){ nowState = 0; }
Sit_Mode::~Sit_Mode(){}
State_Command* Sit_Mode::Operation(const EEG_STATE& data, rehabilitativeUsart *reh, Paradigm *para, Dialog *da)
{
    if(data == MI_LEFT){
        return nullptr;
    }else if(data == MI_RIGHT){
        reh->sendToBend(0xC1);
        para->StandMode();
        return new Stand_Mode;
    }
    return nullptr;
}


Walk_Mode::Walk_Mode(){ nowState = 0; }
Walk_Mode::~Walk_Mode(){}
State_Command* Walk_Mode::Operation(const EEG_STATE& data, rehabilitativeUsart *reh, Paradigm *para, Dialog *da)
{
    if(data == MI_LEFT){
        lastState = STAND_MODE;
        //reh->sendToBend(0x36);
//        para->MI_Mode();
//        para->stop();
        da->threadC1();
        //切换到运动想象界面
        da->toMI();
        //播放左脚视频
        da->startVideoLeft();
        return new MI_Mode;
    }else if(data == MI_RIGHT){
        reh->sendToBend(0xD1);
        para->StandMode();
        return new Stand_Mode;
    }
    return nullptr;
}

MI_Mode::MI_Mode(){ nowState = 1; }
MI_Mode::~MI_Mode(){}
State_Command* MI_Mode::Operation(const EEG_STATE& data, rehabilitativeUsart *reh, Paradigm *para, Dialog *da)
{
    if(data == MI_LEFT){
        if(lastState == STAND_MODE)
            reh->sendToBend(0x1E);
        else
            reh->sendToBend(0xFE);
        lastState == MI_MODE_Left;
        //提示左想象 成功
        da->printToLable("Left");
        //切换到右-视频
        da->startVideoRight();
        return nullptr;
    }else if(data == MI_RIGHT){
        if(lastState == STAND_MODE)
            reh->sendToBend(0x1F);
        else
            reh->sendToBend(0xEF);
        lastState == MI_MODE_Right;
        //提示右-想象 成功
        da->printToLable("Right");
        //切换到左-视频
        da->startVideoLeft();
        return nullptr;
    }else if(data == MI_IDLE){
        if(lastState == MI_MODE_Left)
            reh->sendToBend(0xE1);
        else
            reh->sendToBend(0xF1);
        lastState == STAND_MODE;
//        para->WalkMode();
//        para->begin();
        da->threadC2();
        //切换到SSVEP界面
        da->toSSVEP();
        return new Walk_Mode;
    }
    return nullptr;
}
