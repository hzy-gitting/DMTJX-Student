#include "stumsgwindow.h"
#include "ui_stumsgwindow.h"
#include"widget.h"
#include<QScrollBar>
StuMsgWindow::StuMsgWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::StuMsgWindow)
{
    ui->setupUi(this);

}

StuMsgWindow::~StuMsgWindow()
{
    delete ui;
}

void StuMsgWindow::msgAdd(QString msg)
{
    NetMessage nm(msg,QDateTime::currentDateTime());
    netMsgLst.push_back(nm);
    QString oldMsg = ui->msgRecvEdit->toPlainText();
    QString newMsg = "老师" + nm.getSenderAddr().toString() + "  "+
            nm.getDateTime().toString("yyyy-MM-dd HH:mm:ss")+"\n"+
            "    " +nm.getContent() + "\n";
    ui->msgRecvEdit->setText(oldMsg+newMsg);

    ui->msgRecvEdit->verticalScrollBar()->setValue(ui->msgRecvEdit->verticalScrollBar()->maximum());
    return ;
}
