#include "networkdatahandler.h"

NetworkDataHandler::NetworkDataHandler(QObject *parent) : QObject(parent)
{

}

bool NetworkDataHandler::process(QByteArray data)
{
    emit processBegin();

    bool b = _process(data);

    emit processOver();
    return b;
}

bool NetworkDataHandler::_process(QByteArray data)
{
    //默认处理，什么也不做，只返回true
    return true;
}
