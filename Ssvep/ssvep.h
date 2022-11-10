#ifndef SSVEP_SSVEP_H
#define SSVEP_SSVEP_H

#include <chrono>
#include <QGridLayout>
#include <QWidget>
#include "Ssvep/ssveptimer.h"

namespace Ui { class Ssvep; }

namespace ehdu{

class Ssvep: public QWidget{
    Q_OBJECT
public:
    explicit Ssvep(QWidget *parent = nullptr);
    ~Ssvep();
public slots:
    void startSsvep(const std::chrono::nanoseconds &cycle);
    void stopSsvep();
private:
    void ssvepOn();
    void ssvepOff();
    Ui::Ssvep *ui;
    SsvepTimer *timer;
    bool flipFlag;

    QWidget *center;
    QWidget *east;
    QWidget *south;
    QWidget *west;
    QWidget *north;
};

}

#endif // SSVEP_SSVEP_H
