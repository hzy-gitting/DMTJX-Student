#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include<QUdpSocket>
#include<QTcpServer>
#include"networkcommunicationsystem.h"

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
private:
    Ui::Widget *ui;
    QUdpSocket *us,*usdet;
    QTcpServer *tserv;
    bool isReceiving;
    qint64 fileSize;
    qint64 bytesReceived;
    QPixmap screen;

    QTcpServer *rtcp;   //教学控制协议

    void paintEvent(QPaintEvent *event) override;
};
#endif // WIDGET_H
