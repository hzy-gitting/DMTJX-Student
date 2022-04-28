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
    int status;     //状态
    FileReceiver *frcver;
private:
    RTCP();
    void init();

public:

    static RTCP * getInstance();    //获取单例

    bool sendFileRcvCommand(QList<QString> fileList,QList<qint64> fileSizeList);

    bool sendFileData(QByteArray data);

    static void start();

    QHostAddress getTeacherIpAddr();

    bool connectToTeacher(QHostAddress teacherAddr,quint16 teacherPort);

    int getStatus();
signals:

private slots:
    void slotConnected();
    void rtcpRDRD();
};
}
#endif // RTCP_H
