QT += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

CONFIG += c++20

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += \
    eegstreamer.h \
    Chart/impedancechart.h \
    Chart/qcustomplot.h \
    Chart/signalchart.h \
    OpenBCI/Cyton.h \
    OpenBCI/Dataform.h \
    Ssvep/ssvep.h

SOURCES += \
    main.cpp \
    eegstreamer.cpp \
    Chart/impedancechart.cpp \
    Chart/qcustomplot.cpp \
    Chart/signalchart.cpp \
    OpenBCI/Cyton.cpp \
    Ssvep/ssvep.cpp

FORMS += \
    eegstreamer.ui \
    Chart/impedancechart.ui \
    Chart/signalchart.ui \
    Ssvep/ssvep.ui

INCLUDEPATH += \
    $$PWD/ThirdParty/brainflow/inc

LIBS += \
    $$PWD/ThirdParty/brainflow/lib/libBrainflow.a \
    $$PWD/ThirdParty/brainflow/lib/libMLModule.so \
    $$PWD/ThirdParty/brainflow/lib/libDataHandler.so \
    $$PWD/ThirdParty/brainflow/lib/libBoardController.so
