#include "widget.h"
#include<QObject>
#include <QApplication>
#include"rtcp.h"
#include"networkmessagelist.h"
#include<QMessageBox>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    setbuf(stdout, NULL);

    StudentNS::RTCP::start();
    StudentNS::RTCP*rtcp = StudentNS::RTCP::getInstance();
    NetworkMessageList *nm = NetworkMessageList::getInstance();
    QObject::connect(rtcp,&StudentNS::RTCP::sigNewMessage,
            nm,&NetworkMessageList::slotNewMessage);

    Widget w;
    w.show();
    a.exec();

    qDebug()<<"学生端消息循环退出";
    QMessageBox::information(NULL,"提示","学生端消息循环退出");
    return 0;
}
