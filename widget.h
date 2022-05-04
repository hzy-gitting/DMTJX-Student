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
#include"NetworkMessageList.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    bool detectReply();

private slots:
    void usdetreadyread();

    void on_commitFileBtn_clicked();

    void slotNewMessage();
private:
    Ui::Widget *ui;

    QUdpSocket *usdet,*usVideo;

    NetworkMessageList *nm;

    QHostAddress teacherAddr;//教师端ip
    int teacherPort;   //教师端端口

    void paintEvent(QPaintEvent *event) override;
    StuMsgWindow *smw;

    VideoDataBuffer vdb;    //视频数据缓冲区

    // QWidget interface
protected:
    void closeEvent(QCloseEvent *event) override;
};
#endif // WIDGET_H
