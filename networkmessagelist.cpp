#include "networkmessagelist.h"

NetworkMessageList::NetworkMessageList(QObject *parent) : QObject(parent)
{

}

void NetworkMessageList::insertMessage(const NetMessage &msg)
{
    msgList.append(msg);
    emit newMessage();  //触发新消息信号
}

void NetworkMessageList::clearMessage()
{
    msgList.clear();

    emit sigClearMessage(); //触发清空消息信号
}

QList<NetMessage> NetworkMessageList::getAllMessage()
{
    return msgList;
}
