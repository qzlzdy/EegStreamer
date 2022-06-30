QT       += core gui serialport multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

CONFIG += c++20

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += \
    dialog.h \
    deprecated/qmediaplaylist.h \
    deprecated/qmediaplaylist_p.h \
    deprecated/qplaylistfileparser_p.h \
    deprecated/playercontrols.h \
    CCA/me_CCA.h \
    CCA/me_CCA_types.h \
    CCA/rt_nonfinite.h \
    CCA/rtGetInf.h \
    CCA/rtGetNaN.h \
    CCA/rtwtypes.h \
    CSP/csp_rec_ovo.h \
    CSP/csp_train_ovo.h \
    CHART/impedancechart.h \
    CHART/qcustomplot.h \
    CHART/signalchart.h \
    FFTW/motorimagery.h \
    KCCA/kcca.h \
    OFFLINE/saveeeg.h \
    OpenBCI/AlgorithmSwitch.h \
    OpenBCI/CspTrain.h \
    OpenBCI/Cyton.h \
    OpenBCI/CytonFftw.h \
    OpenBCI/Dataform.h \
    OpenBCI/StateCommand.h \
    PARADIGM/paradigm.h \
    USART/rehabilitativeusart.h

SOURCES += \
    main.cpp \
    dialog.cpp \
    deprecated/qmediaplaylist.cpp \
    deprecated/qplaylistfileparser.cpp \
    deprecated/playercontrols.cpp \
    CCA/me_CCA.cpp \
    CCA/rt_nonfinite.cpp \
    CCA/rtGetInf.cpp \
    CCA/rtGetNaN.cpp \
    CSP/csp_rec_ovo.cpp \
    CSP/csp_train_ovo.cpp \
    CHART/impedancechart.cpp \
    CHART/qcustomplot.cpp \
    CHART/signalchart.cpp \
    FFTW/motorimagery.cpp \
    KCCA/kcca.cpp \
    OFFLINE/saveeeg.cpp \
    OpenBCI/AlgorithmSwitch.cpp \
    OpenBCI/CspTrain.cpp \
    OpenBCI/Cyton.cpp \
    OpenBCI/CytonFftw.cpp \
    OpenBCI/StateCommand.cpp \
    PARADIGM/paradigm.cpp \
    USART/rehabilitativeusart.cpp \

FORMS += \
    dialog.ui \
    CHART/signalchart.ui \
    CHART/impedancechart.ui \
    OFFLINE/saveeeg.ui \
    PARADIGM/paradigm.ui

INCLUDEPATH += \
        $$PWD/ThirdParty/fftw/include \
        $$PWD/ThirdParty/OpenBLAS/include \
        $$PWD/ThirdParty/armadillo/include \
        $$PWD/ThirdParty/brainflow/inc

LIBS += \
    $$PWD/ThirdParty/fftw/lib/libfftw3.a \
    $$PWD/ThirdParty/OpenBLAS/lib/libopenblas.so \
    $$PWD/ThirdParty/armadillo/lib/libarmadillo.so \
    $$PWD/ThirdParty/brainflow/lib/libBrainflow.a \
    $$PWD/ThirdParty/brainflow/lib/libMLModule.so \
    $$PWD/ThirdParty/brainflow/lib/libDataHandler.so \
    $$PWD/ThirdParty/brainflow/lib/libBoardController.so

RESOURCES += \
    pic.qrc
