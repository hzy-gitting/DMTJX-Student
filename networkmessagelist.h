#ifndef NETWORKMESSAGELIST_H
#define NETWORKMESSAGELIST_H

#include <QObject>
#include"netmessage.h"

//网络消息列表，用于存储接收到的网络消息。
//添加新消息时，发射相应信号。界面可以订阅此信号来更新界面
class NetworkMessageList : public QObject
{
    Q_OBJECT
public:
    explicit NetworkMessageList(QObject *parent = nullptr);

private:
    QList<NetMessage> msgList;

public:
    void insertMessage(const NetMessage &msg);
    void clearMessage();

    QList<NetMessage> getAllMessage();

    void save();

    void attach(QObject *ui);

signals:
    void newMessage();
    void sigClearMessage();
};

#endif // NETWORKMESSAGELIST_H
