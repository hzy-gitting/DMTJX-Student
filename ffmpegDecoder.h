#pragma once
#include"ffmpeg.h"

#include<QBuffer>
#include"videodatabuffer.h"

class IDecoder
{
protected:
	AVCodec* pCodec;
	AVCodecContext* pCodecCtx;
	AVFormatContext* m_pAvFormatCtx = nullptr; // ���ļ�����������
	AVCodecContext* m_pVidDecodeCtx = nullptr; // ��Ƶ������������
	uint32_t         m_nVidStreamIndex = -1;      // ��Ƶ������ֵ

	FILE* fp_in;
	AVFrame *frame;
	AVFrame* frameYUV;
	SwsContext* sws_ctx;
	FILE* fp1;

	char filename[256];
	int got_output;
	int width, height;

	virtual void init() = 0;

public:
	void Close();
	int32_t ReadFrame();
	bool decode(AVPacket* pkt, AVFrame** pp_out_frame);
	bool flush(AVFrame* frame);
};
class x264Decoder : public IDecoder
{
protected:
	int32_t Open(const char* pszFilePath);
	void init();
	
	//void init_header();
public:
	x264Decoder(const char* filename2);
    ~x264Decoder();
    x264Decoder(VideoDataBuffer *vdb);
private:
    int32_t Open(VideoDataBuffer *vdb);
};

