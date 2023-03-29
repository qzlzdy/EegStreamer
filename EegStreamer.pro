QT += core gui serialport charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++20 strict_c++

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    brainwave.cpp \
    eegstreamer.cpp \
    ssvep.cpp \
    ssveptimer.cpp

HEADERS += \
    brainwave.h \
    eegstreamer.h \
    ssvep.h \
    ssveptimer.h

FORMS += \
    eegstreamer.ui \
    ssvep.ui

INCLUDEPATH += \
    $$PWD/ThirdParty/brainflow/inc

LIBS += \
    -L$$PWD/ThirdParty/brainflow/lib/ \
    -lBrainflow \
    -lDataHandler \
    -lBoardController

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
