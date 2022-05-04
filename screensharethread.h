#ifndef SCREENSHARETHREAD_H
#define SCREENSHARETHREAD_H

#include <QObject>
#include<QDebug>
#include"videodatabuffer.h"
#include<QUdpSocket>


//学生端接收视频流，启动解码器解码为帧，让SDL来渲染呈现视频
class ScreenShareThread : public QObject
{
    Q_OBJECT
public:
    explicit ScreenShareThread(QObject *parent = nullptr);
private:
    QUdpSocket *usVideo;


    VideoDataBuffer vdb;    //视频数据缓冲区


public slots:
    void start();   //线程工作入口
signals:

private slots:
    void usVideoreadyread();
};

#endif // SCREENSHARETHREAD_H
