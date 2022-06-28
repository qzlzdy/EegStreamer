#include "saveeeg.h"
#include "ui_saveeeg.h"

using namespace ehdu;

saveEEG::saveEEG(QWidget *parent, QList<QString> saveSignal, QString reference):
QWidget(parent), ui(new Ui::saveEEG),
signalTemp(saveSignal),saveReference(reference){
    ui->setupUi(this);
    csvFile = new QFile();
    count = 0;
    in = nullptr;
    QObject::connect(ui->startRecordButton,&QPushButton::clicked,this,&saveEEG::startRecordFile);
    QObject::connect(ui->stopRecordButton,&QPushButton::clicked,this,&saveEEG::stopRecordFile);
}

saveEEG::~saveEEG(){
    if(csvFile->isOpen()){
        csvFile->close();
    }
    delete csvFile;
    delete in;
    delete ui;
}

//记录槽函数
void saveEEG::recordEEGslots(ChannelSignal Data){
    if(!csvFile->isOpen()){
        return;
    }

    //时间
    ++count;
    //QString recordTime = QString::number((double)(++count)/300,'f',4);
    //*in<<recordTime;

    //数据
    double saveData;
    saveData = Data.Fp1.first;
    *in << QString::number(saveData, 'f', 4);

    saveData = Data.Fp2.first;
    *in << QString::number(saveData, 'f', 4);

    saveData = Data.C3.first;
    *in << "," << QString::number(saveData,'f',4);

    saveData = Data.C4.first;
    *in << "," << QString::number(saveData,'f',4);

    saveData = Data.P7.first;
    *in << "," << QString::number(saveData,'f',4);

    saveData = Data.P8.first;
    *in << "," << QString::number(saveData,'f',4);

    saveData = Data.O1.first;
    *in << "," << QString::number(saveData,'f',4);

    saveData = Data.O2.first;
    *in << "," << QString::number(saveData,'f',4);

    *in << "\n";

    if(count >= 9000){
        stopRecordFile();
        QMessageBox::about(parentWidget(),"Cyton",QString("Finish %1!").arg(ui->comboBox->currentText()));
    }
}

//开始记录
void saveEEG::startRecordFile(){
    //输入是否为空
    if(ui->lineEdit->text().isEmpty()){
        QMessageBox::about(parentWidget(),"Cyton","LineEdit enter null!");
        return;
    }
    //判断文件是否存在
    QString csvFilePath = QString("%1/train/%2_%3.csv").arg(QCoreApplication::applicationDirPath()).arg(ui->lineEdit->text()).arg(ui->comboBox->currentText());
    if(QFile::exists(csvFilePath)){
        QMessageBox::about(parentWidget(),"Cyton","File already exits!");
        return;
    }
    //创建文件并打开
    csvFile->setFileName(csvFilePath);
    if(!csvFile->open(QIODevice::WriteOnly)){
        QMessageBox::about(parentWidget(),"Cyton","File create fail!");
        return;
    }
    //定义输出流
    in = new QTextStream(csvFile);
    //记录表头
    //recordHeadSlots();

    //告诉线程可以开始记录了
    count = 0;
    emit isTimeToRecord(true);
    ui->startRecordButton->setEnabled(false);
    ui->stopRecordButton->setEnabled(true);
}

//记录表头
void saveEEG::recordHeadSlots(){
    *in << "Reference" << "," << saveReference << "\n";
    *in << "Time";
    for(int i = 0; i < signalTemp.size() - 3; i++){
        QString temp = signalTemp.at(i);
        *in << "," << temp;
    }
    *in << "\n";
}

//停止记录
void saveEEG::stopRecordFile(){
    //告诉线程停止记录
    emit isTimeToRecord(false);
    //关闭文件
    csvFile->close();
    delete in;
    in = nullptr;
    ui->startRecordButton->setEnabled(true);
    ui->stopRecordButton->setEnabled(false);
}

//改变开始按键状态
void saveEEG::startRecordButton(bool flag){
    ui->startRecordButton->setEnabled(flag);
}

//改变停止按键状态
void saveEEG::stopRecordButton(bool flag){
    ui->stopRecordButton->setEnabled(flag);
}

QString saveEEG::lineeditdata(){
    return ui->lineEdit->text();
}
