#-------------------------------------------------
#
# Project created by QtCreator 2018-03-18T06:22:36
#
#-------------------------------------------------

QT       += core gui serialport multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = DSI_CSP_CCA
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
#DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += \
    dialog.h \
    deprecated/qmediaplaylist.h \
    deprecated/qmediaplaylist_p.h \
    deprecated/qplaylistfileparser_p.h \
    deprecated/playercontrols.h \
    DSIAPI/DSI.h \
    DSIAPI/dsi_main.h \
    DSIAPI/dsi_debug.h \
    CHART/qcustomplot.h \
    CHART/signalchart.h \
    DSIAPI/dataform.h \
    CHART/impedancechart.h \
    FFTW/fftw3.h \
    FFTW/motorimagery.h \
    OFFLINE/saveeeg.h \
    USART/rehabilitativeusart.h \
    PARADIGM/paradigm.h \
    DSIAPI/state_command.h \
    CCA/me_CCA.h \
    CCA/me_CCA_types.h \
    CCA/rt_nonfinite.h \
    CCA/rtGetInf.h \
    CCA/rtGetNaN.h \
    CCA/rtwtypes.h \
    KCCA/kcca.h \
    FFTW/fftw3.h \
    FFTW/motorimagery.h \
    CSP/csp_rec_ovo.h \
    CSP/csp_train_ovo.h \
    DSIAPI/alorithmswitch.h \
    DSIAPI/csp_train.h

SOURCES += \
    main.cpp \
    dialog.cpp \
    deprecated/qmediaplaylist.cpp \
    deprecated/qplaylistfileparser.cpp \
    deprecated/playercontrols.cpp \
    DSIAPI/DSI_API_Loader.c \
    DSIAPI/dsi_main.cpp \
    CHART/qcustomplot.cpp \
    CHART/signalchart.cpp \
    CHART/impedancechart.cpp \
    FFTW/motorimagery.cpp \
    OFFLINE/saveeeg.cpp \
    USART/rehabilitativeusart.cpp \
    PARADIGM/paradigm.cpp \
    DSIAPI/state_command.cpp \
    CCA/me_CCA.cpp \
    CCA/rt_nonfinite.cpp \
    CCA/rtGetInf.cpp \
    CCA/rtGetNaN.cpp \
    KCCA/kcca.cpp \
    CSP/csp_rec_ovo.cpp \
    CSP/csp_train_ovo.cpp \
    DSIAPI/alorithmswitch.cpp \
    DSIAPI/csp_train.cpp

FORMS += \
        dialog.ui \
    CHART/signalchart.ui \
    CHART/impedancechart.ui \
    OFFLINE/saveeeg.ui \
    PARADIGM/paradigm.ui

LIBS += "$$PWD/LIB/libfftw3-3.dll"
#LIBS += libfftw3-3.dll


# 使用armadillo
INCLUDEPATH+= $$PWD/3rdLIB/armadillo-9.900.2/include

# 使用自带的blas和lapack
#LIBS += -L$$PWD/3rdLIB/lib_win64 \
#        -lblas_win64_MT \
#        -llapack_win64_MT

#使用openblas，和上述二选一
INCLUDEPATH+= $$PWD/3rdLIB/OpenBLAS-0.3.10-x64\include
LIBS += -L$$PWD/3rdLIB/OpenBLAS-0.3.10-x64/lib \
        -llibopenblas
LIBS += $$PWD/3rdLIB/OpenBLAS-0.3.10-x64/lib/libopenblas.dll.a
LIBS += $$PWD/3rdLIB/OpenBLAS-0.3.10-x64/bin/mingw64_dll/libgfortran-3.dll


QMAKE_CXXFLAGS += -DDSI_PLATFORM=-Windows-x86_64
QMAKE_CFLAGS += -DDSI_PLATFORM=-Windows-x86_64

RESOURCES += \
    pic.qrc

