#include "widget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    setbuf(stdout, NULL);
    Widget w;
    w.show();
    return a.exec();
}
