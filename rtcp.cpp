#include "rtcp.h"
#include"networkmessagelist.h"
#include<QMessageBox>

StudentNS::RTCP * StudentNS::RTCP::pInst = nullptr;

StudentNS::RTCP::RTCP()
{
    init();
}

void StudentNS::RTCP::init()
{
    status = Unconnected;
    rtcpSocket = new QTcpSocket;
    connect(rtcpSocket,&QTcpSocket::connected,this,&StudentNS::RTCP::slotConnected);
    connect(rtcpSocket,&QTcpSocket::readyRead,this,&StudentNS::RTCP::rtcpRDRD);
    //connect(rtcpSocket,&QTcpSocket::errorOccurred,this,&StudentNS::RTCP::rtcpError);
}

StudentNS::RTCP *StudentNS::RTCP::getInstance()
{
    if(pInst == nullptr){
        pInst = new RTCP;
    }
    return pInst;
}

bool StudentNS::RTCP::sendFileRcvCommand(QList<QString> fileList, QList<qint64> fileSizeList)
{
    QByteArray data;
    QDataStream dataOut(&data,QIODevice::ReadWrite);
    QDataStream out(rtcpSocket);
    out.setVersion(QDataStream::Qt_5_1);
    out << QString("fileRcv");//命令字段
    dataOut << fileList;  //文件名列表
    dataOut<< fileSizeList;   //文件大小列表

    out << data;
    return true;
}

bool StudentNS::RTCP::sendFileData( QByteArray data)
{
    QDataStream out(rtcpSocket);
    out.setVersion(QDataStream::Qt_5_1);

    out << QString("fileData");//命令字段
    out << data;
    out.device()->waitForBytesWritten(3000);

    return true;
}

void StudentNS::RTCP::start()
{
    if(pInst == nullptr){
        pInst = new RTCP;
    }
}

QHostAddress StudentNS::RTCP::getTeacherIpAddr()
{
    return rtcpSocket->peerAddress();   //返回套接字对端ip地址，即为教师ip地址
}

quint16 StudentNS::RTCP::getTeacherPort()
{
    return rtcpSocket->peerPort();
}

StudentNS::RTCP::RTCP_STATUS StudentNS::RTCP::getStatus()
{
    return status;
}

bool StudentNS::RTCP::connectToTeacher(QHostAddress teacherAddr,quint16 teacherPort)
{
    rtcpSocket->connectToHost(teacherAddr,teacherPort);
    return true;
}
void StudentNS::RTCP::slotConnected(){
    qDebug()<<"成功连接到教师端RTCP ip="<<getTeacherIpAddr();
    status = Connected;
}
bool MySystemShutDown();

//将数据包打包，放到待处理队列中
//由各个消息handler进行具体的处理
void StudentNS::RTCP::rtcpRDRD(){
    QDataStream in(rtcpSocket);
    in.setVersion(QDataStream::Qt_5_1);
    QString cmd;
    QByteArray payload;
    in.startTransaction();
    in >>cmd;   //读取命令字段
    in >> payload;  //读取数据体（负载）
    qDebug()<<"cmd="<<cmd;
    if(!in.commitTransaction()){
        qDebug()<<"commitTransaction fail";
        return;
    }
    if(cmd == "fileRcv"){//准备接收文件
        qsizetype fileNum;  //文件个数
        QDataStream inData(payload);
        inData >> fileNum;
        qDebug()<<"准备接收文件 数量="<<fileNum;
        QString fileName;
        qint64 fileSize;
        QList<QString> fileList;
        QList<qint64> fileSizeList;
        for(int i=0;i<fileNum;i++){
            inData >> fileName;
            inData >> fileSize;
            fileList.append(fileName);
            fileSizeList.append(fileSize);
            qDebug()<<"文件名："<<fileName<<" 大小="<<fileSize;
        }
        frcver = new FileReceiver(fileList,fileSizeList);
    }
    else if(cmd == "fileData"){//文件数据
        qDebug()<<"收到文件数据";
        frcver->writeFile(payload);
    }
    else if(cmd == "msg"){
        qDebug()<<"收到网络消息";
        //将消息信号传递给消息模块来处理
        emit sigNewMessage(payload,rtcpSocket->peerAddress());
    }
    else if(cmd == "shutdown"){
        if(!MySystemShutDown()){
            QMessageBox::information(NULL,"提示","关机失败");
        }
    }
    if(rtcpSocket->bytesAvailable()){
        rtcpRDRD();
    }
}


#include <windows.h>
#pragma comment(lib,"user32.lib")
bool MySystemShutDown()
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

