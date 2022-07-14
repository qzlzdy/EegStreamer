#include "saveeeg.h"
#include "ui_saveeeg.h"
#include "data_filter.h"

using namespace ehdu;

saveEEG::saveEEG(QWidget *parent, QList<QString> saveSignal, QString reference):
QWidget(parent), ui(new Ui::saveEEG),
signalTemp(saveSignal),saveReference(reference){
    ui->setupUi(this);
    count = 0;
    QObject::connect(ui->startRecordButton, &QPushButton::clicked,
                     this, &saveEEG::startRecordFile);
    QObject::connect(ui->stopRecordButton, &QPushButton::clicked,
                     this, &saveEEG::stopRecordFile);
}

saveEEG::~saveEEG(){
    delete ui;
}

//记录槽函数
void saveEEG::recordEEGslots(const BrainFlowArray<double, 2> &Data){
    //时间
    ++count;

    //数据
    DataFilter::write_file(Data, filename, "a");

    if(count >= 9000){
        stopRecordFile();
        QMessageBox::about(parentWidget(), "Cyton",
            QString("Finish %1!").arg(ui->comboBox->currentText()));
    }
}

//开始记录
void saveEEG::startRecordFile(){
    //输入是否为空
    if(ui->lineEdit->text().isEmpty()){
        QMessageBox::about(parentWidget(), "Cyton", "LineEdit enter null!");
        return;
    }
    //判断文件是否存在
    QString csvFilePath = QString("%1/train/%2_%3.csv")
        .arg(QCoreApplication::applicationDirPath(), ui->lineEdit->text(),
             ui->comboBox->currentText());
    if(QFile::exists(csvFilePath)){
        QMessageBox::about(parentWidget(), "Cyton", "File already exits!");
        return;
    }
    //创建文件并打开
    filename = csvFilePath.toStdString();

    //告诉线程可以开始记录了
    count = 0;
    emit isTimeToRecord(true);
    ui->startRecordButton->setEnabled(false);
    ui->stopRecordButton->setEnabled(true);
}

//停止记录
void saveEEG::stopRecordFile(){
    //告诉线程停止记录
    emit isTimeToRecord(false);
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
