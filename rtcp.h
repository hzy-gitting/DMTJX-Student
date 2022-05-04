#ifndef RTCP_H
#define RTCP_H

#include <QObject>
#include<QTcpSocket>
#include<QHostAddress>
#include"filereceiver.h"

namespace StudentNS {


class RTCP : public QObject
{
    Q_OBJECT

private:
    QTcpSocket *rtcpSocket;     //学生端与教师端关联的套接字，用于发送RTCP协议数据包

    static RTCP *pInst;


    FileReceiver *frcver;
private:
    RTCP();
    void init();

public:
    enum RTCP_STATUS{
        Unconnected,
        Connected,
        Disconnected,
        Error,
    }status;

    static RTCP * getInstance();    //获取单例

    bool sendFileRcvCommand(QList<QString> fileList,QList<qint64> fileSizeList);

    bool sendFileData(QByteArray data);
    bool sendMessage(QByteArray data);

    static void start();

    QHostAddress getTeacherIpAddr();

    quint16 getTeacherPort();
    QHostAddress getLocalIpAddr();

    quint16 getLocalPort();

    RTCP_STATUS getStatus();

    QString getErrorString();

    bool connectToTeacher(QHostAddress teacherAddr,quint16 teacherPort);



signals:
    void statusChanged();   //rtcp状态改变信号
    void sigNewMessage(const QByteArray&msgContent,const QHostAddress &senderAddr);   //rtcp新消息信号
    void startScreenShare();
    void stopScreenShare();
private slots:
    void slotConnected();
    void rtcpRDRD();
    void rtcpError(QAbstractSocket::SocketError socketError);
};
}
#endif // RTCP_H
