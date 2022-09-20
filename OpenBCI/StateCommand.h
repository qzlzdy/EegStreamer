#ifndef STATE_COMMAND_H
#define STATE_COMMAND_H

#include "EegStreamer.h"
#include "OpenBCI/Dataform.h"
#include "PARADIGM/paradigm.h"
#include "USART/rehabilitativeusart.h"

class Dialog;

class StateCommand{
public:
    StateCommand() = default;
    virtual ~StateCommand() = default;
    virtual StateCommand* Operation(const EEG_STATE& data, rehabilitativeUsart* reh, Paradigm* para, Dialog *da) = 0;
    bool nowState;
};

class StandMode: public StateCommand{
public:
    StandMode();
    virtual StateCommand* Operation(const EEG_STATE& data, rehabilitativeUsart*, Paradigm* para, Dialog *) override;
    virtual ~StandMode() override = default;
};

class OneMode: public StateCommand{
public:
    OneMode();
    virtual StateCommand* Operation(const EEG_STATE& data, rehabilitativeUsart* reh, Paradigm* para, Dialog *da) override;
    virtual ~OneMode() override = default;
};

class TwoMode: public StateCommand{
public:
    TwoMode();
    virtual StateCommand* Operation(const EEG_STATE& data, rehabilitativeUsart* reh, Paradigm* para, Dialog *da) override;
    virtual ~TwoMode() override = default;
};

class DownMode: public StateCommand{
public:
    DownMode();
    virtual StateCommand* Operation(const EEG_STATE& data, rehabilitativeUsart* reh, Paradigm* para, Dialog *da) override;
    virtual ~DownMode() override = default;
};

class UpMode: public StateCommand{
public:
    UpMode();
    virtual StateCommand* Operation(const EEG_STATE& data, rehabilitativeUsart* reh, Paradigm* para, Dialog *da) override;
    virtual ~UpMode() override = default;
};

class WalkMode: public StateCommand{
public:
    WalkMode();
    virtual StateCommand* Operation(const EEG_STATE& data, rehabilitativeUsart* reh, Paradigm* para, Dialog *da) override;
    virtual ~WalkMode() override = default;
};

class SitMode: public StateCommand{
public:
    SitMode();
    virtual StateCommand* Operation(const EEG_STATE& data, rehabilitativeUsart* reh, Paradigm* para, Dialog *da) override;
    virtual ~SitMode() override = default;
};

class MiMode: public StateCommand{
public:
    MiMode();
    virtual StateCommand* Operation(const EEG_STATE& data, rehabilitativeUsart* reh, Paradigm* para, Dialog *da) override;
    virtual ~MiMode() override = default;
};

#endif // STATE_COMMAND_H
