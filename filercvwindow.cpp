#include "filercvwindow.h"
#include "ui_filercvwindow.h"
#include<QProgressBar>
#include"Windows.h"
FileRcvWindow::FileRcvWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FileRcvWindow)
{
    ui->setupUi(this);
}

FileRcvWindow::~FileRcvWindow()
{
    delete ui;
}

FileRcvWindow::FileRcvWindow(QList<QString> fileList, QList<qint64> fileSizeList,FileReceiver *fileReceiver):
    ui(new Ui::FileRcvWindow)
{
    ui->setupUi(this);
    connect(fileReceiver,&FileReceiver::fileRcvProgressUpdate,this,&FileRcvWindow::updateFileRcvProgress);
    QTableWidget *tab = ui->fileRcvTable;
    tab->setColumnCount(3);
    tab->setHorizontalHeaderItem(0,new QTableWidgetItem("文件名"));
    tab->setHorizontalHeaderItem(1,new QTableWidgetItem("大小"));
    tab->setHorizontalHeaderItem(2,new QTableWidgetItem("进度"));
    tab->setColumnWidth(2,200);
    tab->setRowCount(fileList.size());
    for(int i = 0;i< fileList.size();i++){
        QString fn = fileList.at(i);
        fn = fn.sliced(fn.lastIndexOf('/')+1);
        tab->setItem(i,0,new QTableWidgetItem(fn));
        tab->setItem(i,1,new QTableWidgetItem(QString::number(fileSizeList.at(i))));
        QProgressBar *pb = new QProgressBar;
        pb->setFixedHeight(20);
        pb->setMaximum(fileSizeList.at(i));     //设置进度条最大值为文件大小
        tab->setCellWidget(i,2,pb);
    }

}
void FileRcvWindow::updateFileRcvProgress(int n,qint64 sizeLeft){
    QTableWidget *tab = ui->fileRcvTable;
    QProgressBar *pb =(QProgressBar*) tab->cellWidget(n,2);
    pb->setValue(pb->maximum() - sizeLeft);     //设定当前进度值
}

void FileRcvWindow::on_openDirBtn_clicked()
{
    ShellExecuteA(NULL,"open",NULL,NULL,"E:/test",SW_SHOWNORMAL);
}


void FileRcvWindow::on_closeBtn_clicked()
{
    if(!close()){
        qDebug()<<"关闭窗口失败";
    }
}

