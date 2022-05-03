#include "widget.h"
#include "ui_widget.h"
#include<QUdpSocket>
#include<QTcpSocket>
#include"networkcommunicationsystem.h"
#include<QHostInfo>
#include<QFile>
#include <QPainter>
#include<QTime>
#include"stumsgwindow.h"
#include"SDLMaster.h"
#include"ffmpegDecoder.h"
#include"ffmpeg.h"
#include"videodatabuffer.h"
#include<Windows.h>
#include<QMessageBox>
#include"filereceiver.h"
#include"commitfilewindow.h"
#include"rtcp.h"
#include"networkmessagelist.h"
#include<QCloseEvent>


#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define fn "E:/mn.mp4"
#define fn1 "E:/fj.mp4"
#define fn2 "C:\\Users\\hzy\\Desktop\\windows-screen-master\\屏幕分享例程\\save.h264"

void ffmpeg_init()
{
    av_register_all();
    avformat_network_init();
    avdevice_register_all();
}
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    nm = NetworkMessageList::getInstance();
    connect(nm,&NetworkMessageList::newMessage,this,&Widget::slotNewMessage);

    /*ffmpeg_init();
    x264Decoder dec(&vdb);
    dec.ReadFrame();
    SDL_Quit();*/

    //探测端口
    usdet=new QUdpSocket();
    connect(usdet,&QUdpSocket::readyRead,this,&Widget::usdetreadyread);
    usdet->bind(QHostAddress::AnyIPv4,8900);

    //视频端口，接收教师屏幕流

    usVideo=new QUdpSocket();
    connect(usVideo,&QUdpSocket::readyRead,this,&Widget::usVideoreadyread);
    usVideo->bind(QHostAddress::AnyIPv4,8901);
    SOCKET sk = usVideo->socketDescriptor();
    int rcvSize,ns = 8000000;
    int len = sizeof(int);
    if(setsockopt(sk,SOL_SOCKET,SO_RCVBUF,(char*)&ns,len)){
        qDebug()<<"setsockopt err";

    }
    if(getsockopt(sk,SOL_SOCKET,SO_RCVBUF,(char*)&rcvSize,&len)){
        qDebug()<<"getsockopt err";

    }
    qDebug()<<"rcvSize="<<rcvSize;

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

    //创建消息窗口
    smw=new StuMsgWindow(this);

    //打开视频数据缓冲区
    /*if(!vdb.open(QIODevice::ReadWrite)){
        qDebug()<<"vdb open fail";
    }*/

    f.setFileName("ad.h264");

    f.open(QIODevice::WriteOnly);

}

Widget::~Widget()
{
    delete ui;
}
void Widget::slotNewMessage(){
    qDebug()<<__FUNCTION__;
    QList< NetMessage> msgList =  nm->getAllMessage();
    qDebug()<<"getAllMessage";
    QString ss;
    NetMessage msg;
    for(int i=0;i<msgList.size();i++){
        const NetMessage &msg = msgList.at(i);
        ss+="老师(";
        ss+=msg.getSenderAddr().toString();
        ss+=") ";
        ss+=msg.getDateTime().toString("yyyy-MM-dd HH:mm:ss");
        ss+="\n    ";
        ss+=msg.getContent();
        ss+="\n";
    }
    ui->msgRcvEdit->setPlainText(ss);
    ui->msgRcvEdit->moveCursor(QTextCursor::End);
qDebug()<<__FUNCTION__ <<" end";

}
//视频端口接收到数据
//*****************问题：缓冲区溢出，部分数据被丢弃
void Widget::usVideoreadyread(){
    int n=0;
    n = usVideo->pendingDatagramSize();
    qDebug()<<"pendingDatagramSize="<<n;
    char *data = new char[n];

    QHostAddress addr;
    quint16 port;
    if(n){
        if(-1 != usVideo->readDatagram(data,n,&addr,&port)){
            int wn = vdb.write(data,n);

            if(wn == -1){
                qDebug()<<"wdb write err";
            }
            if(wn != n){
                qDebug()<<"wn != n";
            }
            /*vdb.seek(0);
            int rn = vdb.read(data,100);
            if(rn == -1){
                qDebug()<<"wdb read err";
            }
            if(rn != 100){
                qDebug()<<"rn = "<<rn;
            }*/
            if(-1 == f.write(data,n)){
                qDebug()<<"fw err";

            }
        }
    }
    delete[] data;
}


//探测端口收到数据
void Widget::usdetreadyread(){
    int len = 500000;
    char *data = new char[len];
    int n=0;
    n = usdet->bytesAvailable();
    qDebug()<<"bytesAvailable="<<n;
    QHostAddress addr;
    quint16 port;
    if(n){
        if(-1 != usdet->readDatagram(data,n,&addr,&port)){
            data[n]='\0';
            if(strcmp(data,"detect") == 0){
                //记录下教师端ip地址
                teacherAddr = addr;
                teacherPort = port;
                qDebug()<<"教师端ip:"<<teacherAddr<<"端口："<<teacherPort;
                StudentNS::RTCP *rtcp = StudentNS::RTCP::getInstance();
                rtcp->connectToTeacher(teacherAddr,8999);

                qDebug()<<"已收到探测包";

                //回应探测
                this->detectReply();
            }
        }
    }
    delete[] data;
}

void Widget::rtcpError(QAbstractSocket::SocketError socketError){
    qDebug()<<"rtcpError: "<<socketError;
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
    if(ba == "shutdown"){//远程关机
        if(!MySystemShutDown()){
            qDebug()<<"shutdown failed";
        }
    }else if(ba.startsWith("msg")){//接收到新消息
        smw->show();
        emit msgReceive(ba.sliced(3));//除掉msg前缀剩下消息体
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

}
//重写关闭事件处理
void Widget::closeEvent(QCloseEvent *event)
{
    QMessageBox::information(NULL,"提示","closeEvent");
    qDebug()<<"close event";
    QApplication::exit();

    QMessageBox::information(NULL,"提示","exit");
}
//tcp读取
void Widget::tcpReadyread(){
    qDebug()<<"tcp serv read";
    QTcpSocket *s = (QTcpSocket*)sender();
    qDebug()<<"buf size=" << s->readBufferSize();
    //注意，这里有问题。不一定能一次性全部读取完数据。应该先提前知道文件总大小，分次接收保存。
    //
    QDataStream in(s);
    QString fileName;
    in.setVersion(QDataStream::Qt_5_1);
    if(!isReceiving){
        in.startTransaction();
        in >> fileSize;
        in >> fileName;
        if(!in.commitTransaction()){
            return;
        }
        qDebug()<<"接受文件名："<<fileName;
        qDebug()<<"开始接收文件，文件大小="<<fileSize;
        QString compFileName = "E:/test/"+fileName;
        f.setFileName(compFileName);
        if(!f.open(QIODevice::WriteOnly)){
            qDebug() << "打开文件失败"<<f.errorString();
            exit(0);
        }
        isReceiving = true;
        bytesReceived=0;
        ui->fileTranProgress->reset();
        ui->fileTranProgress->setRange(0,fileSize);
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

    if(bytesReceived==fileSize){
        f.close();
        isReceiving=false;
        qDebug()<<"接收完成！";
    }
}


void Widget::on_bindButton_clicked()
{
    //us->bind(QHostAddress(ui->ipEdit->text()),ui->portEdit->text().toInt());
    //f.close();
    qDebug()<<"asd";
    ffmpeg_init();qDebug()<<"dasfasfca";
    x264Decoder *dec = new x264Decoder(&vdb);qDebug()<<"wefwe";
    QMessageBox::information(this,"sad","x264Decoder() ret");
    dec->ReadFrame();
    QMessageBox::information(this,"sad","bindclick ret");
    qDebug()<<"rdf rett";
    //SDL_Quit();

}

#include <QMessageBox>
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
    //发送回应包，教师端就可以知道学生端的ip（封装在ip包头）
    if(-1 == usdet->writeDatagram("detectReply",11,teacherAddr,teacherPort)){
        qDebug()<<"发送回应失败："<<usdet->errorString();
        return false;
    }
    return true;
}

//学生提交作业给教师端（上传文件）
void Widget::on_commitFileBtn_clicked()
{
    CommitFileWindow *cfw = new CommitFileWindow;
    cfw->show();
}
