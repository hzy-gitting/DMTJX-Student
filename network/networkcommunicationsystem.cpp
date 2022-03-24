#include "networkcommunicationsystem.h"

NetworkCommunicationSystem::NetworkCommunicationSystem(QObject *parent) : QObject(parent)
{
    udpSocket = new QUdpSocket();
}
//发送tcp数据，返回成功布尔值
bool NetworkCommunicationSystem::tcpSendData(char *data, int size, QHostAddress addr, int port)
{
    //先看是否与该主机建立了tcp连接，如果是，则直接发送数据，否则先连接再发送
    QTcpSocket *s = nullptr;
    int lenWritten = 0;
    if(!isConnectedTo(addr,port)){
        if(!connectTo(addr,port)){
           return false;
        }
    }
    s = getTcpSocket(addr,port);
    while(size > 0){
        lenWritten = s->write(data,size);
        if(lenWritten == -1){
            qDebug()<<s->error();
            emit error();
            return false;
        }
        size -= lenWritten;
        data += lenWritten;
    }
    return true;
}

bool NetworkCommunicationSystem::tcpSendData_block(char *data, int size, QHostAddress addr, int port, int msec)
{
    return false;
}

bool NetworkCommunicationSystem::udpSendData(char *data, int size, QHostAddress addr, int port)
{
    udpSocket->writeDatagram(data,size,addr,port);
}

bool NetworkCommunicationSystem::isConnectedTo(QHostAddress addr, int port)
{
    return tcpSocketMap.contains(addr);
}
//连接
bool NetworkCommunicationSystem::connectTo(QHostAddress addr, int port)
{
    QTcpSocket *s = new QTcpSocket();
    s->connectToHost(addr,port);
    if(s->waitForConnected(5000)){
        connect(s,&QTcpSocket::readyRead,this,&NetworkCommunicationSystem::tcpReadyRead);
        tcpSocketMap.value(addr,s);
        return true;
    }else{
        delete s;
        qDebug()<<s->error();
        emit error();
        return false;
    }
}

QTcpSocket *NetworkCommunicationSystem::getTcpSocket(QHostAddress addr, int port)
{
    return tcpSocketMap.value(addr);
}

//接收到tcp数据发射dataReceived信号
void NetworkCommunicationSystem::tcpReadyRead(){
    QTcpSocket *s = (QTcpSocket*)sender();
    QByteArray data = s->readAll();
    int port = s->peerPort();
    QHostAddress addr=s->peerAddress();
    emit dataReceived(data,addr,port);
}
