#include "screensharethread.h"
#include"SDLMaster.h"
#include"ffmpegDecoder.h"
#include"ffmpeg.h"
#include"videodatabuffer.h"
#include<QMessageBox>
#include<QUdpSocket>
#include<QThread>

ScreenShareThread::ScreenShareThread(QObject *parent) : QObject(parent)
{

}

void ScreenShareThread::start()
{

    qDebug()<<"ScreenShareThread start at"<<QThread::currentThread();
    Sleep(2000);
    //视频端口，接收教师屏幕流

    usVideo=new QUdpSocket();
    connect(usVideo,&QUdpSocket::readyRead,this,&ScreenShareThread::usVideoreadyread);
    if(!usVideo->bind(QHostAddress::AnyIPv4,8901)){
        qDebug()<<"视频端口监听失败，请尝试更换端口号";
        return;
    }
    SOCKET sk = usVideo->socketDescriptor();
    //扩大套接字接收缓冲区
    int ns = 8000000;
    int len = sizeof(int);
    if(setsockopt(sk,SOL_SOCKET,SO_RCVBUF,(char*)&ns,len)){
        qDebug()<<"setsockopt err";
    }

    ffmpeg_init();

    AVFrame *frame;
    x264Decoder *dec = new x264Decoder(&vdb);
    qDebug()<<"wefwe";
    SDLMaster::init(dec->getWidth(), dec->getHeight());

    for (int i = 0;!QThread::currentThread()->isInterruptionRequested() ; i++)
    {
        if(dec->getVideoFrame(&frame)){
            SDLMaster::updateScreen(frame);
        }else{
            qDebug()<<"getVideoFrame false";
        }
    }

    qDebug()<<"ScreenShareThread ret"<<QThread::currentThread();
}


//视频端口接收到数据
//*****************问题：缓冲区溢出，部分数据被丢弃
void ScreenShareThread::usVideoreadyread(){
    int n=0;
    n = usVideo->pendingDatagramSize();
    qDebug()<<"pendingDatagramSize="<<n;
    char *data = new char[n];

    QHostAddress addr;
    quint16 port;
    if(n){
        if(-1 != usVideo->readDatagram(data,n,&addr,&port)){
            int wn = vdb.write(data,n);

            if(wn == -1){
                qDebug()<<"wdb write err";
            }
            if(wn != n){
                qDebug()<<"wn != n";
            }

        }
    }
    delete[] data;
}
