#ifndef PARADIGM_H
#define PARADIGM_H

#include <QWidget>
#include <QProcess>

#define BLACK_C true
#define WHITE_C false

QT_BEGIN_NAMESPACE
namespace Ui { class Paradigm; }
QT_END_NAMESPACE

class Paradigm: public QWidget{
    Q_OBJECT
public:
    explicit Paradigm(QWidget *parent = 0);
    ~Paradigm();
    void DownMode();
    void init();
    void MI_Mode();
    void ModeOne();
    void ModeTwo();
    void SitMode();
    void StandMode();
    void UpMode();
    void WalkMode();
signals:
    void closeSignal();
private:
    Ui::Paradigm *ui;
    QProcess *process1;
    QProcess *process2;
};

#endif // PARADIGM_H
