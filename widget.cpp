#include "widget.h"
#include "ui_widget.h"
#include<QUdpSocket>
#include<QTcpSocket>
#include"networkcommunicationsystem.h"
#include<QHostInfo>
#include<QFile>
#include <QPainter>
#include<QTime>
#include <QNetworkDatagram>
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
using namespace StudentNS;

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

    //探测端口
    usdet=new QUdpSocket();
    connect(usdet,&QUdpSocket::readyRead,this,&Widget::usdetreadyread);
    usdet->bind(QHostAddress::AnyIPv4,8900);

    rtcp = StudentNS::RTCP::getInstance();

    connect(rtcp,&RTCP::statusChanged,this,&Widget::slotRTCPStatusChanged);

    updateRTCPStatusText();

    //创建消息窗口
    smw=new StuMsgWindow(this);



}

Widget::~Widget()
{
    delete ui;
}
void Widget::slotNewMessage(){
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

}

void Widget::updateRTCPStatusText(){
    if(!rtcp){
        return;
    }
    QString statusText;
    if(rtcp->getStatus()==RTCP::Unconnected){
        statusText = "未连接";
    }else if(rtcp->getStatus()==RTCP::Connected){
        statusText = "已连接";
    }else if(rtcp->getStatus()==RTCP::Disconnected){
        statusText = "已断开";
    }else if(rtcp->getStatus()==RTCP::Error){
        statusText = "错误（"+rtcp->getErrorString()+")";
    }
    ui->RTCPStatus->setText(statusText);
}
//更新RTCP状态显示
void Widget::slotRTCPStatusChanged(){
    updateRTCPStatusText();
}

//探测端口收到数据
void Widget::usdetreadyread(){
    int n=0;
    n = usdet->bytesAvailable();
    qDebug()<<"bytesAvailable="<<n;
    quint16 port;
    QByteArray ba;
    QNetworkDatagram dg;
    dg = usdet->receiveDatagram();
    ba = dg.data();
    qDebug()<<"ba size="<<ba.size();
    QDataStream in(ba);
    in.setVersion(QDataStream::Qt_5_1);
    in.startTransaction();
    QString cmd;
    in>>cmd;
    in>>port;
    if(!in.commitTransaction()){
        qDebug()<<"usdetreadyread commitTransaction fail";
        return;
    }
    if(cmd == "detect"){
        qDebug()<<"已收到探测包";
        teacherAddr = dg.senderAddress();
        teacherPort = port;
        qDebug()<<"教师端ip:"<<teacherAddr<<"RTCP端口："<<teacherPort;
        ui->teacherIPAddr->setText(teacherAddr.toString());
        ui->teacherRTCPPort->setText(QString::number(teacherPort));
        if(!rtcp){
            qDebug()<<"rtcp 未实例化";
            return;
        }
        if(rtcp->getStatus() == RTCP::Unconnected||rtcp->getStatus() == RTCP::Disconnected){

            rtcp->connectToTeacher(teacherAddr,teacherPort);
            ui->localIPAddr->setText(rtcp->getLocalIpAddr().toString());
            ui->localRTCPPort->setText(QString::number(rtcp->getLocalPort()));
        }
        //回应探测
        this->detectReply();
    }
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


//回应教师端发送的探测包
bool Widget::detectReply(){
    //发送回应包，教师端就可以知道学生端的ip（封装在ip包头）
    /*if(-1 == usdet->writeDatagram("detectReply",11,teacherAddr,teacherPort)){
        qDebug()<<"发送回应失败："<<usdet->errorString();
        return false;
    }*/
    return true;
}

//学生提交作业给教师端（上传文件）
void Widget::on_commitFileBtn_clicked()
{
    CommitFileWindow *cfw = new CommitFileWindow;
    cfw->show();
}

//学生发送消息按钮
void Widget::on_sendMsgBtn_clicked()
{
    QString text = ui->msgSendEdit->toPlainText();
    StudentNS::RTCP *rtcp = StudentNS::RTCP::getInstance();
    rtcp->sendMessage(text.toUtf8());
}

