#ifndef SSVEP_SSVEPTIMER_H
#define SSVEP_SSVEPTIMER_H

#include <chrono>
#include <QElapsedTimer>
#include <QThread>

namespace ehdu{

class SsvepTimer: public QThread{
    Q_OBJECT
public:
    SsvepTimer(const std::chrono::nanoseconds &cycle);
    bool flashFlag;
signals:
    void timeout();
protected:
    void run() override;
private:
    QElapsedTimer *timer;
    qint64 threshold;
};

};

#endif // SSVEP_SSVEPTIMER_H
