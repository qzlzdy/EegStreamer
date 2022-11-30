QT += core gui serialport charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += printsupport

CONFIG += c++20

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    cyton.cpp \
    eegstreamer.cpp \
    ssvep.cpp \
    ssveptimer.cpp

HEADERS += \
    cyton.h \
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
