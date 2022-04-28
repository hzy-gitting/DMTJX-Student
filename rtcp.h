#ifndef RTCP_H
#define RTCP_H

#include <QObject>

class RTCP : public QObject
{
    Q_OBJECT
public:
    explicit RTCP(QObject *parent = nullptr);

signals:

};

#endif // RTCP_H
