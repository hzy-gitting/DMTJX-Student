#include "filedatahandler.h"

FileDataHandler::FileDataHandler(QObject *parent) : NetworkDataHandler(parent)
{

}

bool FileDataHandler::_process(QByteArray data)
{
    return false;
}
