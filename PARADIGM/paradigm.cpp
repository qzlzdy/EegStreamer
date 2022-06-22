#include "paradigm.h"
#include "ui_paradigm.h"

Paradigm::Paradigm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Paradigm)
{
    ui->setupUi(this);

    //init();
    connect(ui->closeButton, &QToolButton::clicked, [&](){ emit closeSignal(); });
}

Paradigm::~Paradigm()
{
    if(process1 != nullptr)
    {
        process1->close();
        delete process1;
//        process1 == nullptr;
    }

    if(process2 != nullptr)
    {
        process2->close();
        delete process2;
//        process2 == nullptr;
    }
    delete ui;
}

void Paradigm::StandMode()
{
    ui->closeButton->show();
    ui->leftButton->setText("Stairs Mode");
    ui->rightButton->setText("Walk/Sit Mode");
    ui->stateLabel->setText("Stand Mode");
}

void Paradigm::ModeOne()
{
    ui->closeButton->hide();
    ui->leftButton->setText("Down Mode");
    ui->rightButton->setText("Up Mode");
    ui->stateLabel->setText("Mode One");
}

void Paradigm::DownMode()
{
    ui->leftButton->setText("None");
    ui->rightButton->setText("Stand Mode");
    ui->stateLabel->setText("Down Mode");
}

void Paradigm::UpMode()
{
    ui->leftButton->setText("None");
    ui->rightButton->setText("Stand Mode");
    ui->stateLabel->setText("Up Mode");
}

void Paradigm::ModeTwo()
{
    ui->closeButton->hide();
    ui->leftButton->setText("Walk Mode");
    ui->rightButton->setText("Sit Mode");
    ui->stateLabel->setText("Mode Two");
}

void Paradigm::WalkMode()
{
    ui->leftButton->setText("MI Mode");
    ui->rightButton->setText("Stand Mode");
    ui->stateLabel->setText("Walk Mode");
}

void Paradigm::SitMode()
{
    ui->leftButton->setText("None");
    ui->rightButton->setText("Stand Mode");
    ui->stateLabel->setText("Sit Mode");
}

void Paradigm::MI_Mode()
{
    ui->leftButton->setText("L-Foot Move");
    ui->rightButton->setText("R-Foot Move");
    ui->stateLabel->setText("MI Mode");
    //ui->tipLabel->setText("Now Mode Is Manual Mode\nIdle To Back To Auto Walk Mode\nThree Second Change To MI Interface");
}

void Paradigm::init()
{
    process1 = new QProcess;
    process1->start("Stimulus-10hz.exe");

    process2 = new QProcess;
    process2->start("Stimulus-12hz.exe");

    Sleep(500);

    //获取窗口1
    HWND wid1 = FindWindow(L"Qt5QWindowIcon",L"10hz");
    //设置1父窗口
    HWND parentHwnd1 = (HWND)ui->left_widget->winId();
    SetParent(wid1, parentHwnd1);
    //设置窗口1位置
    SetWindowPos(wid1, HWND_TOP, -5, -5, 200, 200, SWP_FRAMECHANGED);

    //获取窗口2
    HWND wid2 = FindWindow(L"Qt5QWindowIcon",L"12hz");
    //设置2父窗口
    HWND parentHwnd2 = (HWND)ui->right_widget->winId();
    SetParent(wid2, parentHwnd2);
    //设置窗口2位置
    SetWindowPos(wid2, HWND_TOP, -5, -5, 200, 200, SWP_FRAMECHANGED);

//    WId wid1 = (WId)FindWindow(L"Qt5QWindowIcon",L"7hz");
//    QWindow *m_window1;
//    m_window1 = QWindow::fromWinId(wid1);
//    QWidget *m_widget;
//    m_widget = QWidget::createWindowContainer(m_window1,ui->left_widget);
//    m_widget->setMinimumSize(150,150);
//    m_widget->setMaximumSize(150,150);

}
