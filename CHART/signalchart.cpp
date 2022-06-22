#include "signalchart.h"
#include "ui_signalchart.h"

#define XAXIS_STEP 0.2 //设置X步长
#define GRAPH_POINT_NUM 250//总共显示的的点数

using namespace ehdu;

SignalChart::SignalChart(QWidget *parent, QList<QString> paintSignal):
QWidget(parent), paintSignals(paintSignal), ui(new Ui::signalChart){
    ui->setupUi(this);
    xAxisNum = 0;
}

SignalChart::~SignalChart(){
    delete ui;
}

//曲线初始化
void SignalChart::chartInit(){
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
    for(int i = 0; i < paintSignals.size(); ++i){
        QString temp = paintSignals.at(i);
        QPen qPen1;

        if(temp == "F3"){
            F3 = ui->chartWidget->addGraph();
            qPen1.setColor(QColor(0,255,255));
            F3->setPen(qPen1);
            F3->setName("F3");
        }
        else if(temp == "F4"){
            F4 = ui->chartWidget->addGraph();
            qPen1.setColor(QColor(255,255,0));
            F4->setPen(qPen1);
            F4->setName("F4");
        }
        else if(temp == "C3"){
            C3 = ui->chartWidget->addGraph();
            qPen1.setColor(QColor(0,128,255));
            C3->setPen(qPen1);
            C3->setName("C3");
        }
        else if(temp == "Cz"){
            Cz = ui->chartWidget->addGraph();
            qPen1.setColor(QColor(255,0,128));
            Cz->setPen(qPen1);
            Cz->setName("Cz");
        }
        else if(temp == "C4"){
            C4 = ui->chartWidget->addGraph();
            qPen1.setColor(QColor(128,0,255));
            C4->setPen(qPen1);
            C4->setName("C4");
        }
        else if(temp == "P3"){
            P3 = ui->chartWidget->addGraph();
            qPen1.setColor(QColor(64,128,0));
            P3->setPen(qPen1);
            P3->setName("P3");
        }
        else if(temp == "Pz"){
            Pz = ui->chartWidget->addGraph();
            qPen1.setColor(QColor(0,128,64));
            Pz->setPen(qPen1);
            Pz->setName("Pz");
        }
        else if(temp == "P4"){
            P4 = ui->chartWidget->addGraph();
            qPen1.setColor(QColor(128,0,64));
            P4->setPen(qPen1);
            P4->setName("P4");
        }
    }

    //调整轴使得图中曲线所有可见
    ui->chartWidget->rescaleAxes(true);
    //刷新,重新绘制界面
    ui->chartWidget->replot();
}

//数据动态更新槽
void SignalChart::chartAddData(ChannelSignal data){
    static int ind = 0;
    //增加X
    xAxisNum += XAXIS_STEP;
    //数据满时，清除数据
    if(xAxisNum / XAXIS_STEP > GRAPH_POINT_NUM){
        for(int i = 0; i < paintSignals.size(); ++i){
            QString temp = paintSignals.at(i);
            if(temp == "F3"){
                F3->clearData();
            }
            else if(temp == "F4"){
                F4->clearData();
            }
            else if(temp == "C3"){
                C3->clearData();
            }
            else if(temp == "Cz"){
                Cz->clearData();
            }
            else if(temp == "C4"){
                C4->clearData();
            }
            else if(temp == "P3"){
                P3->clearData();
            }
            else if(temp == "Pz"){
                Pz->clearData();
            }
            else if(temp == "P4"){
                P4->clearData();
            }
        }
        xAxisNum = 0;
    }
    //添加数据
    for(int i = 0; i < paintSignals.size(); ++i){
        QString temp = paintSignals.at(i);
        if(temp == "F3"){
            F3->addData(xAxisNum, data.F3.first);
        }
        else if(temp == "F4"){
            F4->addData(xAxisNum, data.F4.first);
        }
        else if(temp == "C3"){
            C3->addData(xAxisNum, data.C3.first);
        }
        else if(temp == "Cz"){
            Cz->addData(xAxisNum, data.Cz.first);
        }
        else if(temp == "C4"){
            C4->addData(xAxisNum, data.C4.first);
        }
        else if(temp == "P3"){
            P3->addData(xAxisNum, data.P3.first);
        }
        else if(temp == "Pz"){
            Pz->addData(xAxisNum, data.Pz.first);
        }
        else if(temp == "P4"){
            P4->addData(xAxisNum, data.P4.first);
        }
    }
    if(ind > 2){
        //调整轴使得图中曲线所有可见
        ui->chartWidget->rescaleAxes(true);
        //刷新,重新绘制界面
        ui->chartWidget->replot();
        ind = 0;
    }
    else{
        ++ind;
    }
}

//单机事件,拖动
void SignalChart::mousePress(){
    //如果选择了一个轴，只允许拖动那个方向的轴
    //如果没有选择轴，则可以拖动两个方向
    if(ui->chartWidget->xAxis->selectedParts().testFlag(QCPAxis::spAxis)){
        ui->chartWidget->axisRect()->setRangeDrag(ui->chartWidget->xAxis->orientation());
    }
    else if(ui->chartWidget->yAxis->selectedParts().testFlag(QCPAxis::spAxis)){
        ui->chartWidget->axisRect()->setRangeDrag(ui->chartWidget->yAxis->orientation());
    }
    else{
        ui->chartWidget->axisRect()->setRangeDrag(Qt::Horizontal|Qt::Vertical);
    }
}

//滑轮事件,缩放
void SignalChart::mouseWheel(){
    //如果选择了一个轴，只允许该轴的方向被缩放
    //如果没有选择轴，两个方向都可能被放大
    if(ui->chartWidget->xAxis->selectedParts().testFlag(QCPAxis::spAxis)){
        ui->chartWidget->axisRect()->setRangeZoom(ui->chartWidget->xAxis->orientation());
    }
    else if(ui->chartWidget->yAxis->selectedParts().testFlag(QCPAxis::spAxis)){
        ui->chartWidget->axisRect()->setRangeZoom(ui->chartWidget->yAxis->orientation());
    }
    else{
        ui->chartWidget->axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);
    }
}

//选择改变
void SignalChart::selectionChanged(){
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
    if(ui->chartWidget->xAxis->selectedParts().testFlag(QCPAxis::spAxis)
    || ui->chartWidget->xAxis->selectedParts().testFlag(QCPAxis::spTickLabels)
    || ui->chartWidget->xAxis2->selectedParts().testFlag(QCPAxis::spAxis)
    || ui->chartWidget->xAxis2->selectedParts().testFlag(QCPAxis::spTickLabels)){
        ui->chartWidget->xAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
        ui->chartWidget->xAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    }
    // make left and right axes be selected synchronously, and handle axis and tick labels as one selectable object:
    if(ui->chartWidget->yAxis->selectedParts().testFlag(QCPAxis::spAxis)
    || ui->chartWidget->yAxis->selectedParts().testFlag(QCPAxis::spTickLabels)
    || ui->chartWidget->yAxis2->selectedParts().testFlag(QCPAxis::spAxis)
    || ui->chartWidget->yAxis2->selectedParts().testFlag(QCPAxis::spTickLabels)){
        ui->chartWidget->yAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
        ui->chartWidget->yAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    }
    // synchronize selection of graphs with selection of corresponding legend items:
    for(int i = 0; i < ui->chartWidget->graphCount(); ++i){
        QCPGraph *graph = ui->chartWidget->graph(i);
        QCPPlottableLegendItem *item = ui->chartWidget->legend->itemWithPlottable(graph);
        if(item->selected() || graph->selected()){
            item->setSelected(true);
            graph->setSelected(true);
        }
    }
}
