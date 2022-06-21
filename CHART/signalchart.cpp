#include "signalchart.h"
#include "ui_signalchart.h"

#define XAXIS_STEP 0.2 //设置X步长
#define GRAPH_POINT_NUM 250//总共显示的的点数

signalChart::signalChart(QWidget *parent, QList<QString> paintSignal) :
    QWidget(parent),
    ui(new Ui::signalChart),
    paintSignals(paintSignal)
{
    ui->setupUi(this);
    xAxis_num = 0;
}

signalChart::~signalChart()
{
    delete ui;
}

//曲线初始化
void signalChart::chartInit()
{
    //连接鼠标点击信号和槽
    //connect(ui->chartWidget, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePress()));
    //连接鼠标滚轮信号和槽
    //connect(ui->chartWidget, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(mouseWheel()));
    //连接曲线选择信号和槽
    //connect(ui->chartWidget, SIGNAL(selectionChangedByUser()), this, SLOT(selectionChanged()));
    //设置交互方式
    //ui->chartWidget->setInteractions(QCP::iRangeDrag|QCP::iRangeZoom|QCP::iSelectAxes|QCP::iSelectLegend|QCP::iSelectPlottables);

    //设置背景
    QBrush qBrush(0xffffff);//qBrush(0xf0f0f0);//qBrush(0xd6d3ce);
    ui->chartWidget->setBackground(qBrush);

    //设置标题
    ui->chartWidget->legend->setVisible(true);
    ui->chartWidget->legend->setBrush(qBrush);
    ui->chartWidget->legend->setIconSize(15,1);

    //设置字体
    QFont qfont;
    qfont.setPointSize(6);
    ui->chartWidget->legend->setFont(qfont);

    //设置坐标轴
    ui->chartWidget->xAxis->setTicks(false);
    ui->chartWidget->yAxis->setTicks(true);
    ui->chartWidget->yAxis->setAutoTicks(true);
    ui->chartWidget->yAxis->setAutoTickStep(true);
    ui->chartWidget->yAxis->setAutoSubTicks(true);
    //ui->chartWidget->yAxis->setRange(-20000.0,20000.0);

    //生成曲线加入widght中
    //曲线设置名称以及颜色
    for(int i = 0;i<paintSignals.size();i++)
    {
        QString temp = paintSignals.at(i);
        QPen qPen1;

        if(temp == "Fp1"){
            graph_Fp1 = ui->chartWidget->addGraph();
            qPen1.setColor(QColor(255,0,0));
            graph_Fp1->setPen(qPen1);
            graph_Fp1->setName("Fp1");
        }else if(temp == "Fp2"){
            graph_Fp2 = ui->chartWidget->addGraph();
            qPen1.setColor(QColor(0,255,0));
            graph_Fp2->setPen(qPen1);
            graph_Fp2->setName("Fp2");
        }else if(temp == "F7"){
            graph_F7 = ui->chartWidget->addGraph();
            qPen1.setColor(QColor(0,0,255));
            graph_F7->setPen(qPen1);
            graph_F7->setName("F7");
        }else if(temp == "F3"){
            graph_F3 = ui->chartWidget->addGraph();
            qPen1.setColor(QColor(0,255,255));
            graph_F3->setPen(qPen1);
            graph_F3->setName("F3");
        }else if(temp == "Fz"){
            graph_Fz = ui->chartWidget->addGraph();
            qPen1.setColor(QColor(255,0,255));
            graph_Fz->setPen(qPen1);
            graph_Fz->setName("Fz");
        }else if(temp == "F4"){
            graph_F4 = ui->chartWidget->addGraph();
            qPen1.setColor(QColor(255,255,0));
            graph_F4->setPen(qPen1);
            graph_F4->setName("F4");
        }else if(temp == "F8"){
            graph_F8 = ui->chartWidget->addGraph();
            qPen1.setColor(QColor(255,128,0));
            graph_F8->setPen(qPen1);
            graph_F8->setName("F8");
        }else if(temp == "A1"){
            graph_A1 = ui->chartWidget->addGraph();
            qPen1.setColor(QColor(128,255,0));
            graph_A1->setPen(qPen1);
            graph_A1->setName("A1");
        }else if(temp == "T3"){
            graph_T3 = ui->chartWidget->addGraph();
            qPen1.setColor(QColor(0,255,128));
            graph_T3->setPen(qPen1);
            graph_T3->setName("T3");
        }else if(temp == "C3"){
            graph_C3 = ui->chartWidget->addGraph();
            qPen1.setColor(QColor(0,128,255));
            graph_C3->setPen(qPen1);
            graph_C3->setName("C3");
        }else if(temp == "Cz"){
            graph_Cz = ui->chartWidget->addGraph();
            qPen1.setColor(QColor(255,0,128));
            graph_Cz->setPen(qPen1);
            graph_Cz->setName("Cz");
        }else if(temp == "C4"){
            graph_C4 = ui->chartWidget->addGraph();
            qPen1.setColor(QColor(128,0,255));
            graph_C4->setPen(qPen1);
            graph_C4->setName("C4");
        }else if(temp == "T4"){
            graph_T4 = ui->chartWidget->addGraph();
            qPen1.setColor(QColor(0,64,64));
            graph_T4->setPen(qPen1);
            graph_T4->setName("T4");
        }else if(temp == "A2"){
            graph_A2 = ui->chartWidget->addGraph();
            qPen1.setColor(QColor(64,0,64));
            graph_A2->setPen(qPen1);
            graph_A2->setName("A2");
        }else if(temp == "T5"){
            graph_T5 = ui->chartWidget->addGraph();
            qPen1.setColor(QColor(64,64,0));
            graph_T5->setPen(qPen1);
            graph_T5->setName("T5");
        }else if(temp == "P3"){
            graph_P3 = ui->chartWidget->addGraph();
            qPen1.setColor(QColor(64,128,0));
            graph_P3->setPen(qPen1);
            graph_P3->setName("P3");
        }else if(temp == "Pz"){
            graph_Pz = ui->chartWidget->addGraph();
            qPen1.setColor(QColor(0,128,64));
            graph_Pz->setPen(qPen1);
            graph_Pz->setName("Pz");
        }else if(temp == "P4"){
            graph_P4 = ui->chartWidget->addGraph();
            qPen1.setColor(QColor(128,0,64));
            graph_P4->setPen(qPen1);
            graph_P4->setName("P4");
        }else if(temp == "T6"){
            graph_T6 = ui->chartWidget->addGraph();
            qPen1.setColor(QColor(0,255,64));
            graph_T6->setPen(qPen1);
            graph_T6->setName("T6");
        }else if(temp == "O1"){
            graph_O1 = ui->chartWidget->addGraph();
            qPen1.setColor(QColor(128,0,128));
            graph_O1->setPen(qPen1);
            graph_O1->setName("O1");
        }else if(temp == "O2"){
            graph_O2 = ui->chartWidget->addGraph();
            qPen1.setColor(QColor(0,128,128));
            graph_O2->setPen(qPen1);
            graph_O2->setName("O2");
        }else if(temp == "X1"){
            graph_X1 = ui->chartWidget->addGraph();
            qPen1.setColor(QColor(255,0,64));
            graph_X1->setPen(qPen1);
            graph_X1->setName("X1");
        }else if(temp == "X2"){
            graph_X2 = ui->chartWidget->addGraph();
            qPen1.setColor(QColor(64,128,0));
            graph_X2->setPen(qPen1);
            graph_X2->setName("X2");
        }else if(temp == "X3"){
            graph_X3 = ui->chartWidget->addGraph();
            qPen1.setColor(QColor(64,128,64));
            graph_X3->setPen(qPen1);
            graph_X3->setName("X3");
        }
    }

    //调整轴使得图中曲线所有可见
    ui->chartWidget->rescaleAxes(true);
    //刷新,重新绘制界面
    ui->chartWidget->replot();

}

int i = 0;

//数据动态更新槽
void signalChart::chartAddData(channelSignal data)
{
    //增加X
    xAxis_num += XAXIS_STEP;
    //数据满时，清除数据
    if(xAxis_num/XAXIS_STEP > GRAPH_POINT_NUM)
    {
        for(int i = 0;i<paintSignals.size();i++)
        {
            QString temp = paintSignals.at(i);
            if(temp == "Fp1"){
                graph_Fp1->clearData();
            }else if(temp == "Fp2"){
                graph_Fp2->clearData();
            }else if(temp == "F7"){
                graph_F7->clearData();
            }else if(temp == "F3"){
                graph_F3->clearData();
            }else if(temp == "Fz"){
                graph_Fz->clearData();
            }else if(temp == "F4"){
                graph_F4->clearData();
            }else if(temp == "F8"){
                graph_F8->clearData();
            }else if(temp == "A1"){
                graph_A1->clearData();
            }else if(temp == "T3"){
                graph_T3->clearData();
            }else if(temp == "C3"){
                graph_C3->clearData();
            }else if(temp == "Cz"){
                graph_Cz->clearData();
            }else if(temp == "C4"){
                graph_C4->clearData();
            }else if(temp == "T4"){
                graph_T4->clearData();
            }else if(temp == "A2"){
                graph_A2->clearData();
            }else if(temp == "T5"){
                graph_T5->clearData();
            }else if(temp == "P3"){
                graph_P3->clearData();
            }else if(temp == "Pz"){
                graph_Pz->clearData();
            }else if(temp == "P4"){
                graph_P4->clearData();
            }else if(temp == "T6"){
                graph_T6->clearData();
            }else if(temp == "O1"){
                graph_O1->clearData();
            }else if(temp == "O2"){
                graph_O2->clearData();
            }else if(temp == "X1"){
                graph_X1->clearData();
            }else if(temp == "X2"){
                graph_X2->clearData();
            }else if(temp == "X3"){
                graph_X3->clearData();
            }
        }
        xAxis_num = 0;
    }
    //添加数据
    for(int i = 0;i<paintSignals.size();i++)
    {
        QString temp = paintSignals.at(i);
        if(temp == "Fp1"){
            graph_Fp1->addData(xAxis_num,data.Fp1.first);
        }else if(temp == "Fp2"){
            graph_Fp2->addData(xAxis_num,data.Fp2.first);
        }else if(temp == "F7"){
            graph_F7->addData(xAxis_num,data.F7.first);
        }else if(temp == "F3"){
            graph_F3->addData(xAxis_num,data.F3.first);
        }else if(temp == "Fz"){
            graph_Fz->addData(xAxis_num,data.Fz.first);
        }else if(temp == "F4"){
            graph_F4->addData(xAxis_num,data.F4.first);
        }else if(temp == "F8"){
            graph_F8->addData(xAxis_num,data.F8.first);
        }else if(temp == "A1"){
            graph_A1->addData(xAxis_num,data.A1.first);
        }else if(temp == "T3"){
            graph_T3->addData(xAxis_num,data.T3.first);
        }else if(temp == "C3"){
            graph_C3->addData(xAxis_num,data.C3.first);
        }else if(temp == "Cz"){
            graph_Cz->addData(xAxis_num,data.Cz.first);
        }else if(temp == "C4"){
            graph_C4->addData(xAxis_num,data.C4.first);
        }else if(temp == "T4"){
            graph_T4->addData(xAxis_num,data.T4.first);
        }else if(temp == "A2"){
            graph_A2->addData(xAxis_num,data.A2.first);
        }else if(temp == "T5"){
            graph_T5->addData(xAxis_num,data.T5.first);
        }else if(temp == "P3"){
            graph_P3->addData(xAxis_num,data.P3.first);
        }else if(temp == "Pz"){
            graph_Pz->addData(xAxis_num,data.Pz.first);
        }else if(temp == "P4"){
            graph_P4->addData(xAxis_num,data.P4.first);
        }else if(temp == "T6"){
            graph_T6->addData(xAxis_num,data.T6.first);
        }else if(temp == "O1"){
            graph_O1->addData(xAxis_num,data.O1.first);
        }else if(temp == "O2"){
            graph_O2->addData(xAxis_num,data.O2.first);
        }else if(temp == "X1"){
            graph_X1->addData(xAxis_num,data.X1.first);
        }else if(temp == "X2"){
            graph_X2->addData(xAxis_num,data.X2.first);
        }else if(temp == "X3"){
            graph_X3->addData(xAxis_num,data.X3.first);
        }
    }
    if(i>2)
    {
        //调整轴使得图中曲线所有可见
        ui->chartWidget->rescaleAxes(true);
        //刷新,重新绘制界面
        ui->chartWidget->replot();
        i=0;
    }else{
        ++i;
    }
}

//单机事件,拖动
void signalChart::mousePress()
{
    //如果选择了一个轴，只允许拖动那个方向的轴
    //如果没有选择轴，则可以拖动两个方向
    if (ui->chartWidget->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
    {
        ui->chartWidget->axisRect()->setRangeDrag(ui->chartWidget->xAxis->orientation());
    }else if (ui->chartWidget->yAxis->selectedParts().testFlag(QCPAxis::spAxis)){
        ui->chartWidget->axisRect()->setRangeDrag(ui->chartWidget->yAxis->orientation());
    }else{
        ui->chartWidget->axisRect()->setRangeDrag(Qt::Horizontal|Qt::Vertical);
    }
}

//滑轮事件,缩放
void signalChart::mouseWheel()
{
    //如果选择了一个轴，只允许该轴的方向被缩放
    //如果没有选择轴，两个方向都可能被放大
    if (ui->chartWidget->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
    {
        ui->chartWidget->axisRect()->setRangeZoom(ui->chartWidget->xAxis->orientation());
    }else if (ui->chartWidget->yAxis->selectedParts().testFlag(QCPAxis::spAxis)){
        ui->chartWidget->axisRect()->setRangeZoom(ui->chartWidget->yAxis->orientation());
    }else{
        ui->chartWidget->axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);
    }
}

//选择改变
void signalChart::selectionChanged()
{
    /*
   normally, axis base line, axis tick labels and axis labels are selectable separately, but we want
   the user only to be able to select the axis as a whole, so we tie the selected states of the tick labels
   and the axis base line together. However, the axis label shall be selectable individually.

   The selection state of the left and right axes shall be synchronized as well as the state of the
   bottom and top axes.

   Further, we want to synchronize the selection of the graphs with the selection state of the respective
   legend item belonging to that graph. So the user can select a graph by either clicking on the graph itself
   or on its legend item.
  */
    // make top and bottom axes be selected synchronously, and handle axis and tick labels as one selectable object:
    if (ui->chartWidget->xAxis->selectedParts().testFlag(QCPAxis::spAxis) || ui->chartWidget->xAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
            ui->chartWidget->xAxis2->selectedParts().testFlag(QCPAxis::spAxis) || ui->chartWidget->xAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
    {
        ui->chartWidget->xAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
        ui->chartWidget->xAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    }
    // make left and right axes be selected synchronously, and handle axis and tick labels as one selectable object:
    if (ui->chartWidget->yAxis->selectedParts().testFlag(QCPAxis::spAxis) || ui->chartWidget->yAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
            ui->chartWidget->yAxis2->selectedParts().testFlag(QCPAxis::spAxis) || ui->chartWidget->yAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
    {
        ui->chartWidget->yAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
        ui->chartWidget->yAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    }
    // synchronize selection of graphs with selection of corresponding legend items:
    for (int i=0; i<ui->chartWidget->graphCount(); ++i)
    {
        QCPGraph *graph = ui->chartWidget->graph(i);
        QCPPlottableLegendItem *item = ui->chartWidget->legend->itemWithPlottable(graph);
        if (item->selected() || graph->selected())
        {
            item->setSelected(true);
            graph->setSelected(true);
        }
    }
}




