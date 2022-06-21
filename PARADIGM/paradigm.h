#ifndef PARADIGM_H
#define PARADIGM_H

#include <QWidget>
#include <windows.h>
#include <QProcess>

#define BLACK_C true
#define WHITE_C false

namespace Ui {
class Paradigm;
}

class Paradigm : public QWidget
{
    Q_OBJECT

public:
    explicit Paradigm(QWidget *parent = 0);
    ~Paradigm();

    void StandMode();
    void ModeOne();
    void ModeTwo();
    void DownMode();
    void UpMode();
    void WalkMode();
    void SitMode();
    void MI_Mode();

    void init();

private:
    Ui::Paradigm *ui;
    QProcess *process1;
    QProcess *process2;

signals:
    void closeSignal();
};

#endif // PARADIGM_H
