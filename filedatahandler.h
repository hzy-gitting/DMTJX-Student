#ifndef FILEDATAHANDLER_H
#define FILEDATAHANDLER_H

#include "networkdatahandler.h"

class FileDataHandler : public NetworkDataHandler
{
public:
    explicit FileDataHandler(QObject *parent = nullptr);

    // NetworkDataHandler interface
public:
    bool _process(QByteArray data);
};

#endif // FILEDATAHANDLER_H
