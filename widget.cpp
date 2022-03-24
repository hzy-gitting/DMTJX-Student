#include "widget.h"
#include "ui_widget.h"
#include<QUdpSocket>
#include<QTcpSocket>
#include"networkcommunicationsystem.h"
#include<QHostInfo>
#include<QFile>
#include <QPainter>
#include<QTime>
#include"stumessageform.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    //探测端口
    usdet=new QUdpSocket();
    connect(usdet,&QUdpSocket::readyRead,this,&Widget::usdetreadyread);
    usdet->bind(QHostAddress::LocalHost,8900);



    us=new QUdpSocket();
    connect(us,&QUdpSocket::readyRead,this,&Widget::readyread);
    us->bind(QHostAddress::LocalHost,8890);

    tserv=new QTcpServer();
    tserv->connect(tserv,&QTcpServer::newConnection,this,&Widget::newconn);
    tserv->listen(QHostAddress::Any,8887);
    ui->fileTranProgress->reset();
    ui->fileTranProgress->setFormat("%v / %mbytes (%p%)");
    ui->fileTranProgress->setFormat("%v / %mbytes (%p%)");

    //远程教学控制协议（RTCP）
    rtcp = new QTcpServer();
    rtcp->listen(QHostAddress::Any,8888);
    connect(rtcp,&QTcpServer::newConnection,this,&Widget::tcconn);

    isReceiving = false;
    fileSize = 0;
    bytesReceived=0;
}

Widget::~Widget()
{
    delete ui;
}

//探测端口收到数据
void Widget::usdetreadyread(){
    int len = 50000;
    char *data = new char[len];
    qDebug()<<"usdet redr";qDebug()<<usdet->bytesAvailable();
    int n=0;
    n = usdet->bytesAvailable();
    if(n){
        if(usdet->readDatagram(data,n)){
            data[n]='\0';
            qDebug()<<data;
            if(strcmp(data,"detect") == 0){
                //回应探测
                this->detectReply();
            }
        }
    }
    delete[] data;
}


//教师机连接上来
void Widget::tcconn(){
    qDebug()<<"teacher conn";
    if(rtcp->hasPendingConnections()){
        QTcpSocket *s = rtcp->nextPendingConnection();
        connect(s,&QTcpSocket::readyRead,this,&Widget::rtcpReadyRead);
    }
}

void Widget::rtcpReadyRead(){
    QTcpSocket *s = (QTcpSocket*)sender();
    QByteArray ba = s->readAll();
    if(ba == "shutdown"){
        if(!MySystemShutDown()){
            qDebug()<<"shutdown failed";
        }
    }
}

void Widget::newconn(){
    qDebug()<<"new conn";
    if(tserv->hasPendingConnections()){
        QTcpSocket *s = tserv->nextPendingConnection();
        connect(s,&QTcpSocket::readyRead,this,&Widget::tcpReadyread);
    }
}
void Widget::readyread(){
    /*qDebug()<<"ready";
    int b = us->bytesAvailable();
    if(b <=0){
        return;
    }
    char *buf=new char[b+1];
    quint16 port;
    QHostAddress addr;
    us->readDatagram(buf,b,&addr,&port);
    buf[b]='\0';
    QString msg = QString::fromLocal8Bit(buf,b);
    qDebug()<<QString::fromLocal8Bit(buf,b)<<" ip:"<<addr<<" port="<<port;
    QTextEdit* edit = ui->textEdit;
    QString s = edit->toPlainText();
    ui->textEdit->setText(s+msg);*/

    qDebug()<<"sr";
    uint b = us->bytesAvailable();
    if(b <=0){
        return;
    }
    char *buf=new char[b+1];
    us->readDatagram(buf,b);    //0ms
    screen.loadFromData((uchar*)buf,b,"jpg");//解压缩 耗时50ms 瓶颈1
    qDebug()<<QTime::currentTime();
    update();
    qDebug()<<QTime::currentTime();
    qDebug()<<"er";
}

void Widget::paintEvent(QPaintEvent *event){//1-2ms 渲染

    QPainter painter(this);
    painter.drawLine(0,0,100,100);
    screen.setDevicePixelRatio(1.25);
    painter.drawPixmap(0,0,screen);
    qDebug()<<screen.width();

}
//tcp读取
void Widget::tcpReadyread(){
    qDebug()<<"tcp serv read";
    QTcpSocket *s = (QTcpSocket*)sender();
    qDebug()<<"buf size=" << s->readBufferSize();
    //注意，这里有问题。不一定能一次性全部读取完数据。应该先提前知道文件总大小，分次接收保存。
    //
    QDataStream in(s);
    in.setVersion(QDataStream::Qt_5_1);
    if(!isReceiving){
        in.startTransaction();
        in >> fileSize;
        if(!in.commitTransaction()){
            return;
        }
        qDebug()<<"开始接收文件，文件大小="<<fileSize;
        isReceiving = true;
        bytesReceived=0;
        ui->fileTranProgress->reset();
        ui->fileTranProgress->setRange(0,fileSize);
    }
    QFile f("E:/test/a.txt");
    if(bytesReceived == 0){
        if(!f.open(QIODevice::WriteOnly)){
            qDebug() << "打开文件失败";
        }
    }else{
        if(!f.open(QIODevice::Append)){
            qDebug()<<"打开文件失败";
        }
    }
    QByteArray msg = s->readAll();
    int bytesWritten = 0;
    int bytesLeft=msg.size();
    char const *data = msg.constData();
    while(bytesLeft > 0){
        bytesWritten = f.write(data,bytesLeft);
        if(-1 == bytesWritten){
            qDebug()<<"写入失败";
        }
        bytesReceived+=bytesWritten;
        qDebug()<<"接收数据"<<bytesWritten<<" bytes."<<" 当前进度："<<bytesReceived <<"/"<<fileSize;
        ui->fileTranProgress->setValue(bytesReceived);
        bytesLeft -= bytesWritten;
        data += bytesWritten;
    }

    f.close();
    if(bytesReceived==fileSize){
        isReceiving=false;
        qDebug()<<"接收完成！";
    }
}


void Widget::on_bindButton_clicked()
{
    us->bind(QHostAddress(ui->ipEdit->text()),ui->portEdit->text().toInt());
}

#include <windows.h>
#pragma comment(lib,"user32.lib")
bool Widget::MySystemShutDown()
{
    HANDLE hToken;
    TOKEN_PRIVILEGES tkp;

    //获取进程标志
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
         return false;

    //获取关机特权的LUID
    LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME,    &tkp.Privileges[0].Luid);
    tkp.PrivilegeCount = 1;
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    //获取这个进程的关机特权
    AdjustTokenPrivileges(hToken, false, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
    if (GetLastError() != ERROR_SUCCESS) return false;

    // 强制关闭计算机
    if ( !ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCE, 0))
          return false;
    return true;
}

//回应教师端发送的探测包
bool Widget::detectReply(){
    qDebug()<<"asd";
    return true;
}
