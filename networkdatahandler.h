#ifndef NETWORKDATAHANDLER_H
#define NETWORKDATAHANDLER_H

#include <QObject>

class NetworkDataHandler : public QObject
{
    Q_OBJECT
public:
    explicit NetworkDataHandler(QObject *parent = nullptr);

    bool process(QByteArray data);  //处理数据体的方法，调用_process前后发出信号

    virtual bool _process(QByteArray data);  //真正处理数据体的方法


signals:
    void processBegin();    //正式处理前信号
    void processOver();     //处理完毕后信号
};

#endif // NETWORKDATAHANDLER_H
