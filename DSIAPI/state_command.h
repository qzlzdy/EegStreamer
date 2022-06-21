#ifndef STATE_COMMAND_H
#define STATE_COMMAND_H

#include <DSIAPI/dataform.h>
#include <USART/rehabilitativeusart.h>
#include <PARADIGM/paradigm.h>
#include <dialog.h>

class Dialog;

class State_Command
{
public:
    State_Command();
    virtual State_Command* Operation(const EEG_STATE& data, rehabilitativeUsart* reh, Paradigm* para, Dialog *da) = 0;
    virtual ~State_Command();
    bool nowState;
};

class Stand_Mode: public State_Command{
public:
    Stand_Mode();
    virtual State_Command* Operation(const EEG_STATE& data, rehabilitativeUsart* reh, Paradigm* para, Dialog *da) override;
    virtual ~Stand_Mode() override;
};

class One_Mode: public State_Command{
public:
    One_Mode();
    virtual State_Command* Operation(const EEG_STATE& data, rehabilitativeUsart* reh, Paradigm* para, Dialog *da) override;
    virtual ~One_Mode() override;
};

class Two_Mode: public State_Command{
public:
    Two_Mode();
    virtual State_Command* Operation(const EEG_STATE& data, rehabilitativeUsart* reh, Paradigm* para, Dialog *da) override;
    virtual ~Two_Mode() override;
};

class Down_Mode: public State_Command{
public:
    Down_Mode();
    virtual State_Command* Operation(const EEG_STATE& data, rehabilitativeUsart* reh, Paradigm* para, Dialog *da) override;
    virtual ~Down_Mode() override;
};

class Up_Mode: public State_Command{
public:
    Up_Mode();
    virtual State_Command* Operation(const EEG_STATE& data, rehabilitativeUsart* reh, Paradigm* para, Dialog *da) override;
    virtual ~Up_Mode() override;
};

class Walk_Mode: public State_Command{
public:
    Walk_Mode();
    virtual State_Command* Operation(const EEG_STATE& data, rehabilitativeUsart* reh, Paradigm* para, Dialog *da) override;
    virtual ~Walk_Mode() override;
};

class Sit_Mode: public State_Command{
public:
    Sit_Mode();
    virtual State_Command* Operation(const EEG_STATE& data, rehabilitativeUsart* reh, Paradigm* para, Dialog *da) override;
    virtual ~Sit_Mode() override;
};

class MI_Mode: public State_Command{
public:
    MI_Mode();
    virtual State_Command* Operation(const EEG_STATE& data, rehabilitativeUsart* reh, Paradigm* para, Dialog *da) override;
    virtual ~MI_Mode() override;
};

#endif // STATE_COMMAND_H
