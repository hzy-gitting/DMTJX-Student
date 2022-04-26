#ifndef STUMSGWINDOW_H
#define STUMSGWINDOW_H

#include <QMainWindow>
#include"netmessage.h"
namespace Ui {
class StuMsgWindow;
}

class StuMsgWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit StuMsgWindow(QWidget *parent = nullptr);
    ~StuMsgWindow();

private:
    Ui::StuMsgWindow *ui;

    //消息列表
    QList<NetMessage> netMsgLst;

    //槽：界面上添加新消息
private slots:
    void msgAdd(QString msg);
};

#endif // STUMSGWINDOW_H
