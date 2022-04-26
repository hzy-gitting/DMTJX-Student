#include "videodatabuffer.h"
#define min(a,b) ((a) < (b) ? (a) : (b))
#include"stdlib.h"
#include"memory.h"
#include"stdio.h"
#include"ffmpeg.h"

VideoDataBuffer::VideoDataBuffer()
{
    buf = (char*)malloc(INIT_BUF_SIZE);
    if(!buf){
        printf("malloc fail");
        exit(-1);
    }
    read_ptr = buf;
    buf_size = INIT_BUF_SIZE;
    length = 0;
    read_pos = 0;
    write_pos=0;
}

VideoDataBuffer::~VideoDataBuffer()
{
    free(buf);
    buf = NULL;
    read_ptr = buf = nullptr;
    read_pos = buf_size = length = 0;
}

int VideoDataBuffer::read(char *buf, int buf_size)
{
    int len = min(buf_size,length - read_pos);
    memcpy(buf,this->buf + read_pos,len);
    read_pos += len;

    return len;
}

int VideoDataBuffer::write(char *data, int len)
{
    length += len;
    while(length > buf_size){
        buf = (char*)realloc(buf,buf_size + BUF_SIZE_INC);
        if(!buf){
            printf("realloc fail");
            exit(-1);
        }
        buf_size += BUF_SIZE_INC;
    }
    memcpy(buf + write_pos,data,len);
    write_pos += len;
    return len;
}

int VideoDataBuffer::read_seek(int offset, int whence)
{
    int ret = 0;
    switch (whence)
    {
    case AVSEEK_SIZE:
        ret = length;
        break;
    case SEEK_SET:
        read_pos = offset;
        read_ptr = buf+read_pos;
        ret = (int64_t)read_ptr;
        break;
    }
    return ret;
}

int VideoDataBuffer::getBufSize()
{
    return buf_size;
}

int VideoDataBuffer::getLength()
{
    return length;
}

int VideoDataBuffer::getReadPos()
{
    return read_pos;
}
