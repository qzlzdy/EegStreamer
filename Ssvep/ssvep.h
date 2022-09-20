#ifndef SSVEP_SSVEP_H
#define SSVEP_SSVEP_H

#include <chrono>
#include <QGridLayout>
#include <QTimer>
#include <QWidget>

namespace Ui { class Ssvep; }

namespace ehdu{

class Ssvep : public QWidget{
    Q_OBJECT
public:
    explicit Ssvep(QWidget *parent = nullptr);
    ~Ssvep();
public slots:
    void startSsvep(std::chrono::milliseconds cycle);
    void stopSsvep();
private:
    void ssvepOn();
    void ssvepOff();
    Ui::Ssvep *ui;
    QTimer *timer;

    QWidget *center;
    QWidget *east;
    QWidget *south;
    QWidget *west;
    QWidget *north;

    bool timerFlag;
};

}

#endif // SSVEP_SSVEP_H
