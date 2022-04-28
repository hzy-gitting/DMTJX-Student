#include "commitfilewindow.h"
#include "ui_commitfilewindow.h"
#include<QFileDialog>
#include"rtcp.h"
#include<QMessageBox>
CommitFileWindow::CommitFileWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CommitFileWindow)
{
    ui->setupUi(this);

    //文件发送列表 设置表头
    ui->fileTable->setColumnCount(2);
    ui->fileTable->setHorizontalHeaderItem(0,new QTableWidgetItem("文件名"));
    ui->fileTable->setHorizontalHeaderItem(1,new QTableWidgetItem("大小"));
}

CommitFileWindow::~CommitFileWindow()
{
    delete ui;
}

//选择文件
void CommitFileWindow::on_chooseFileBtn_clicked()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(this,"选择文件");
    if(fileNames.isEmpty()){
        return;
    }
    for(int i=0;i<fileNames.size();i++){
        QString fileName = fileNames.at(i);
        QFile f(fileName);
        int rowIndex = ui->fileTable->rowCount();
        ui->fileTable->setRowCount(rowIndex+1);//添加一行
        ui->fileTable->setItem(rowIndex,0,new QTableWidgetItem(fileName));  //文件名
        ui->fileTable->setItem(rowIndex,1,new QTableWidgetItem(QString::number(f.size(),10)));//大小
        fileSizeList.append(f.size());
    }
    fileList += fileNames;
}

//提交作业（发送文件）
void CommitFileWindow::on_commitBtn_clicked()
{
    //没有选择文件，直接返回
    if(fileList.isEmpty()){
        return;
    }
    //先禁用发送按钮防止重复发送
    ui->commitBtn->setDisabled(true);

    StudentNS::RTCP *rtcp = StudentNS::RTCP::getInstance();

    if(!rtcp->sendFileRcvCommand(fileList,fileSizeList)){
        qDebug()<<"sendFileRcvCommand faIl";
        return;
    }
    //连接成功后，发送内容
    QFile f;
    QByteArray fdata;   //文件数据
    int blockSize = 1024;   //每次读取的文件块大小
    char *data = new char[blockSize];
    for(int i=0;i<fileList.size();i++){
        f.setFileName(fileList.at(i));
        if(!f.open(QIODevice::ReadOnly)){
            qDebug()<<"fopen fail";
            delete[]data;
            return;
        }
        while(f.bytesAvailable()){
            qint64 n = f.read(data,blockSize);
            if(-1 == n){
                qDebug()<<"read file fail";
            }
            qDebug()<<"read file n="<<n;
            fdata.setRawData(data,n);
            if(!rtcp->sendFileData(fdata)){
                qDebug()<<"sendFileData faIl";
                return;
            }
            QGuiApplication::processEvents();
        }
        f.close();

    }
    delete[]data;
    qDebug()<<"作业提交完毕！";
    ui->commitBtn->setEnabled(true);    //重新启用按钮
    QMessageBox::information(NULL,"提示","作业提交成功");
}

