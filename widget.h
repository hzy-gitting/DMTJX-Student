#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include<QUdpSocket>
#include<QTcpServer>
#include"networkcommunicationsystem.h"
#include"stumsgwindow.h"
#include<QFile>
#include<QBuffer>
#include"videodatabuffer.h"
#include"filereceiver.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    bool MySystemShutDown();
    bool detectReply();
public slots:
    void readyread();
    void newconn();
    void tcpReadyread();
private slots:
    void on_bindButton_clicked();

    void tcconn();
    void rtcpReadyRead();
    void usdetreadyread();
    void usVideoreadyread();
    void rtcpRDRD();
    void fileDataRDRD();
    void rtcpError(QAbstractSocket::SocketError socketError);
private:
    Ui::Widget *ui;
    QUdpSocket *us,*usdet,*usVideo;
    QTcpServer *tserv;
    QTcpSocket *rtcpSkt;


    bool isReceiving;
    qint64 fileSize;
    qint64 bytesReceived;
    QPixmap screen;

    QTcpServer *rtcp;   //教学控制协议

    FileReceiver *frcver;   //文件接收模块

    QHostAddress teacherAddr;//教师端ip
    int teacherPort;   //教师端端口
    QFile f;
    void paintEvent(QPaintEvent *event) override;
    StuMsgWindow *smw;

    VideoDataBuffer vdb;    //视频数据缓冲区


    //信号：接收到新消息
signals:
    void msgReceive(QString msg);

};
#endif // WIDGET_H
