#ifndef NETWORKCOMMUNICATIONSYSTEM_H
#define NETWORKCOMMUNICATIONSYSTEM_H

#include <QObject>
#include<QUdpSocket>
#include<QTcpSocket>

//应该抽象接口出来，针对接口编程，不要针对具体类
class NetworkCommunicationSystem : public QObject
{
    Q_OBJECT
public:
    explicit NetworkCommunicationSystem(QObject *parent = nullptr);

    //不要完美主义，别想太多，先写！！！

    //非阻塞
    bool tcpSendData(char *data,int size,QHostAddress addr,int port);

    //阻塞的
    bool tcpSendData_block(char *data,int size,QHostAddress addr,int port,int msec = 3000);

    //非阻塞
    bool udpSendData(char *data,int size,QHostAddress addr,int port);

private:
    bool isConnectedTo(QHostAddress addr,int port);
    bool connectTo(QHostAddress addr,int port);
    QTcpSocket* getTcpSocket(QHostAddress addr,int port);
private:
    QUdpSocket *udpSocket;
    QTcpSocket *tcpSocket;
    QMap<QHostAddress,QTcpSocket*> tcpSocketMap;

signals:
    void dataReceived(char *data,int length,QHostAddress addr,int port);
    void error();

};

#endif // NETWORKCOMMUNICATIONSYSTEM_H
