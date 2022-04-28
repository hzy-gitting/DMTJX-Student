#include "widget.h"

#include <QApplication>
#include"rtcp.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    setbuf(stdout, NULL);

    StudentNS::RTCP::start();

    Widget w;
    w.show();
    return a.exec();
}
