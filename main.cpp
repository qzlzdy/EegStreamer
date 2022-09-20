#include <QApplication>
#include "eegstreamer.h"

int main(int argc, char *argv[]){
    QApplication a(argc, argv);
    ehdu::EegStreamer eegStreamer;
    eegStreamer.show();

    return a.exec();
}
