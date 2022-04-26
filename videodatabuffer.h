#ifndef VIDEODATABUFFER_H
#define VIDEODATABUFFER_H

//视频数据缓冲区，可读可写
//****注意，这个缓冲区不是线程安全的****
//从udp套接字读取的视频数据将被暂存到内存中，以供后续解码使用
class VideoDataBuffer
{
private:
    int buf_size;       //当前缓冲区总容量
    int length;     //当前缓冲区有效数据长度（字节）

    char *buf;  //缓冲区起始地址
    char *read_ptr;  //当前读取到的位置指针
    char *write_ptr;  //当前写入到的位置指针

    int read_pos;    //当前读取到的字节位置
    int write_pos;  //当前写入到的字节位置

    static const int INIT_BUF_SIZE = 1024 * 1024 * 4;   //初始缓冲区大小（常数）
    static const int BUF_SIZE_INC = 1024 * 1024 * 4;   //缓冲区大小增长量（常数）

public:
    VideoDataBuffer();
    ~VideoDataBuffer();

    int read(char *buf,int buf_size);   //读取缓冲区数据
    int write(char *data,int len);  //写入数据到缓冲区
    int read_seek(int offset, int whence);     //改变当前读取指针位置
    int getBufSize();
    int getLength();
    int getReadPos();
};

#endif // VIDEODATABUFFER_H
