//BY hzy
#include "ffmpeg.h"
#include "ffmpegDecoder.h"
#define LOGE printf
#define LOGD LOGE
#include "SDLMaster.h"

#include"videodatabuffer.h"
#include<QBuffer>
#include<QDebug>
#include<QTime>
#include<QMessageBox>
#include<QCoreApplication>
//自定义读取钩子函数
int my_read_packet(void* opaque, uint8_t* buf, int buf_size) {
    VideoDataBuffer *vdb = (VideoDataBuffer*)opaque;
    int bytesLeft = vdb->getLength() - vdb->getReadPos();
    while(bytesLeft <= 0){
        //Sleep(100);
        //qDebug()<<"sleep... len="<<vdb->getLength()<<"rp="<<vdb->getReadPos();
        QCoreApplication::processEvents();
        bytesLeft = vdb->getLength() - vdb->getReadPos();
    }
    qDebug()<<"length="<<vdb->getLength()<<" readpos="<<vdb->getReadPos()<<" buf_size="<<buf_size;
    size_t n = min(bytesLeft, buf_size);
    if (n <= 0) {
        QMessageBox::information(nullptr,"info","eof");
        return AVERROR_EOF;
    }
    qint64 rn = vdb->read((char*)buf,n);
    if(rn == -1){
        qDebug()<<"vdb read err";
    }
    if(rn != n){
        qDebug()<<"vdb read !=!=!=";
    }

    return rn;
}
int64_t my_seek(void* opaque, int64_t offset, int whence) {
    VideoDataBuffer *vdb = (VideoDataBuffer*)opaque;
    int64_t ret = -1;

     printf("whence=%d , offset=%lld , file_size=%ld\n", whence, offset, vdb->getLength());
    switch (whence)
    {
    case AVSEEK_SIZE:
        ret = vdb->getLength();
        break;
    case SEEK_SET:
        QMessageBox::information(NULL,"sad","sek set");
        if(!vdb->read_seek(offset,whence)){
            printf("vdb seek fail");
            return -1;
        }
        ret = (int64_t)vdb->getBufSize();
        break;
    }
    return ret;

}

//
// 打开视频数据缓冲区,解析码流信息,并且创建和打开对应的解码器
//
int32_t x264Decoder::Open(VideoDataBuffer *vdb)
{
    AVCodec* pVidDecoder = nullptr;
    AVCodec* pAudDecoder = nullptr;
    int       res = 0;

    char eStr[100];

    AVIOContext* avIOContext = nullptr;

    size_t avIOCtxBufSize = 4096;
    //分配avioCtx缓冲区
    char* avIOCtxBuf = (char*)av_malloc(avIOCtxBufSize);
    if (!avIOCtxBuf) {
        printf("av_malloc err");
        exit(0);
    }
    //分配一个avIOContext
    avIOContext = avio_alloc_context((unsigned char *)avIOCtxBuf,
        avIOCtxBufSize, 0, vdb, my_read_packet, NULL, my_seek);
    if (!avIOContext) {
        printf("avio_alloc_context err");
        exit(0);
    }
    //分配FormatCtx
    m_pAvFormatCtx = avformat_alloc_context();

    //将avIOContext挂载到m_pAvFormatCtx的pb字段
    m_pAvFormatCtx->pb = avIOContext;
    //打开流
    res = avformat_open_input(&m_pAvFormatCtx, nullptr, nullptr, nullptr);
    if (res < 0) {
        printf("avformat_open_input err");
        av_make_error_string(eStr, 100, res);
                printf("%s", eStr);
        exit(0);
    }
    QMessageBox::information(NULL,"asd","open input ret");
    //res = avformat_open_input(&m_pAvFormatCtx, pszFilePath, nullptr, nullptr);
    if (m_pAvFormatCtx == nullptr)
    {
        LOGE("<Open> [ERROR] fail avformat_open_input(), res=%d\n", res);
        av_make_error_string(eStr, 100, res);
        printf("%s", eStr);
        return res;
    }
    // 查找所有媒体流信息
    res = avformat_find_stream_info(m_pAvFormatCtx, nullptr);
    if (res == AVERROR_EOF)
    {
        LOGD("<Open> reached to file end\n");
        Close();
        return -1;
    }

    // 遍历所有的媒体流信息
    for (unsigned int i = 0; i < m_pAvFormatCtx->nb_streams; i++)
    {
        AVStream* pAvStream = m_pAvFormatCtx->streams[i];
        if (pAvStream->codec->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            if ((pAvStream->codec->width <= 0) || (pAvStream->codec->height <= 0))
            {
                LOGE("<Open> [ERROR] invalid resolution, streamIndex=%d\n", i);
                continue;
            }
            width = pAvStream->codec->width;
            height = pAvStream->codec->height;
            m_pVidDecodeCtx = pAvStream->codec;
            pVidDecoder = avcodec_find_decoder(pAvStream->codec->codec_id);  // 找到视频解码器

            if (pVidDecoder == nullptr)
            {
                LOGE("<Open> [ERROR] can not find video codec\n");
                continue;
            }

            m_nVidStreamIndex = (uint32_t)i;

        }
    }
    if (pVidDecoder == nullptr && pAudDecoder == nullptr)
    {
        LOGE("<Open> [ERROR] can not find video or audio stream\n");
        Close();
        return -1;
    }
    // seek到第0ms开始读取
    //res = avformat_seek_file(m_pAvFormatCtx, 0, INT_MIN, 0, INT_MAX, 0);
    /*res = av_seek_frame(m_pAvFormatCtx,m_nVidStreamIndex,0,0);
    if(res < 0){
        av_make_error_string(eStr, 100, res);
        printf("%s", eStr);
        QMessageBox::information(NULL,"aavf seek err",eStr);
    }*/
    QMessageBox::information(NULL,"asd","avf seek");
    // 创建视频解码器并且打开
    if (pVidDecoder != nullptr)
    {
        /*m_pVidDecodeCtx = avcodec_alloc_context3(pVidDecoder);
        if (m_pVidDecodeCtx == nullptr)
        {
            LOGE("<Open> [ERROR] fail to video avcodec_alloc_context3()\n");
            Close();
            return -1;
        }*/
        res = avcodec_open2(m_pVidDecodeCtx, pVidDecoder, nullptr);
        //m_pVidDecodeCtx->pix_fmt = AV_PIX_FMT_YUV420P;
        //m_pVidDecodeCtx->bit_rate = 4 * 1024 * 1024;
        if (res != 0)
        {
            LOGE("<Open> [ERROR] fail to video avcodec_open2(), res=%d\n", res);
            av_make_error_string(eStr, 100, res);
            printf("%s", eStr);
            Close();
            return -1;
        }
    }


    return 0;
}


//
// 打开媒体文件,解析码流信息,并且创建和打开对应的解码器
//
int32_t x264Decoder::Open(const char* pszFilePath)
{
    AVCodec* pVidDecoder = nullptr;
    AVCodec* pAudDecoder = nullptr;
    int       res = 0;

    char eStr[100];

    AVIOContext* avIOContext = nullptr;
    char* buf = nullptr;
    size_t buf_size = 0;
    //映射文件到缓冲区buf
    if (0 > av_file_map(pszFilePath, (uint8_t**)&buf, &buf_size, 0, nullptr)) {
        printf("av_file_map err");
        exit(0);
    }

    size_t avIOCtxBufSize = 4096;
    //分配avioCtx缓冲区
    char* avIOCtxBuf = (char*)av_malloc(avIOCtxBufSize);
    if (!avIOCtxBuf) {
        printf("av_malloc err");
        exit(0);
    }
    //分配一个avIOContext
    avIOContext = avio_alloc_context((unsigned char *)avIOCtxBuf,
        avIOCtxBufSize, 0, buf, my_read_packet, NULL, my_seek);
    if (!avIOContext) {
        printf("avio_alloc_context err");
        exit(0);
    }
    //分配FormatCtx
    m_pAvFormatCtx = avformat_alloc_context();

    //将avIOContext挂载到m_pAvFormatCtx的pb字段
    m_pAvFormatCtx->pb = avIOContext;
    //打开流
    res = avformat_open_input(&m_pAvFormatCtx, nullptr, nullptr, nullptr);
    if (res < 0) {
        printf("avformat_open_input err");
        exit(0);
    }

    //res = avformat_open_input(&m_pAvFormatCtx, pszFilePath, nullptr, nullptr);
    if (m_pAvFormatCtx == nullptr)
    {
        LOGE("<Open> [ERROR] fail avformat_open_input(), res=%d\n", res);
        av_make_error_string(eStr, 100, res);
        printf("%s", eStr);
        return res;
    }
    // 查找所有媒体流信息
    res = avformat_find_stream_info(m_pAvFormatCtx, nullptr);
    if (res == AVERROR_EOF)
    {
        LOGD("<Open> reached to file end\n");
        Close();
        return -1;
    }

    // 遍历所有的媒体流信息
    for (unsigned int i = 0; i < m_pAvFormatCtx->nb_streams; i++)
    {
        AVStream* pAvStream = m_pAvFormatCtx->streams[i];
        if (pAvStream->codec->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            if ((pAvStream->codec->width <= 0) || (pAvStream->codec->height <= 0))
            {
                LOGE("<Open> [ERROR] invalid resolution, streamIndex=%d\n", i);
                continue;
            }
            width = pAvStream->codec->width;
            height = pAvStream->codec->height;
            m_pVidDecodeCtx = pAvStream->codec;
            pVidDecoder = avcodec_find_decoder(pAvStream->codec->codec_id);  // 找到视频解码器

            if (pVidDecoder == nullptr)
            {
                LOGE("<Open> [ERROR] can not find video codec\n");
                continue;
            }

            m_nVidStreamIndex = (uint32_t)i;

        }
    }
    if (pVidDecoder == nullptr && pAudDecoder == nullptr)
    {
        LOGE("<Open> [ERROR] can not find video or audio stream\n");
        Close();
        return -1;
    }
    // seek到第0ms开始读取
    //res = avformat_seek_file(m_pAvFormatCtx, -1, INT_MIN, 0, INT_MAX, 0);

    // 创建视频解码器并且打开
    if (pVidDecoder != nullptr)
    {
        /*m_pVidDecodeCtx = avcodec_alloc_context3(pVidDecoder);
        if (m_pVidDecodeCtx == nullptr)
        {
            LOGE("<Open> [ERROR] fail to video avcodec_alloc_context3()\n");
            Close();
            return -1;
        }*/
        res = avcodec_open2(m_pVidDecodeCtx, pVidDecoder, nullptr);
        //m_pVidDecodeCtx->pix_fmt = AV_PIX_FMT_YUV420P;
        //m_pVidDecodeCtx->bit_rate = 4 * 1024 * 1024;
        if (res != 0)
        {
            LOGE("<Open> [ERROR] fail to video avcodec_open2(), res=%d\n", res);
            av_make_error_string(eStr, 100, res);
            printf("%s", eStr);
            Close();
            return -1;
        }
    }


    return 0;
}



//
// 关闭媒体文件，关闭对应的解码器
//
void IDecoder::Close()
{
	// 关闭媒体文件解析
	if (m_pAvFormatCtx != nullptr)
	{
		avformat_close_input(&m_pAvFormatCtx);
		m_pAvFormatCtx = nullptr;
	}

	// 关闭视频解码器
	if (m_pVidDecodeCtx != nullptr)
	{
		avcodec_close(m_pVidDecodeCtx);
		m_pVidDecodeCtx = nullptr;
	}

}


//
// 循环不断的读取音视频数据包进行解码处理
//
int32_t IDecoder::ReadFrame()
{
	int res = 0;
	SDLMaster::init(m_pVidDecodeCtx->width, m_pVidDecodeCtx->height);
	for (int i = 0;;i++)
	{
        /*qDebug()<<"rsl "<<QTime::currentTime();
        Sleep(120);
        qDebug()<<"rsl end "<<QTime::currentTime();*/
		//AVPacket p;
		AVPacket* pPacket = (AVPacket * )av_malloc(sizeof(AVPacket));
		 //av_new_packet(pPacket, 0);//av_init_packet(pPacket);
        qDebug()<<"i = "<<i;
		// 依次读取数据包
		res = av_read_frame(m_pAvFormatCtx, pPacket);
		if (res == AVERROR_EOF)  // 正常读取到文件尾部退出
		{
			LOGE("<ReadFrame> reached media file end\n");
            continue;
		}
		else if (res < 0) // 其他小于0的返回值是数据包读取错误
		{
			LOGE("<ReadFrame> fail av_read_frame(), res=%d\n", res);
			break;
		}

		if (pPacket->stream_index == m_nVidStreamIndex)       // 读取到视频包
		{
			// 这里进行视频包解码操作,详细下一章节讲解
			AVFrame* pVideoFrame = nullptr;
			if (decode(pPacket, &pVideoFrame)) {
                qDebug()<<"updateScreen "<<QTime::currentTime();
				SDLMaster::updateScreen(pVideoFrame);
                qDebug()<<"updateScreen end"<<QTime::currentTime();
			}
		}
		av_free_packet(pPacket);  // 数据包用完了可以释放了 
	}
	fclose(fp1);
	return 0;
}

bool IDecoder::decode(AVPacket *pkt,AVFrame** pp_out_frame)
{
	
	int ret = avcodec_decode_video2(m_pVidDecodeCtx, frame, &got_output, pkt);
	if (ret < 0) {
		*pp_out_frame = nullptr;
		printf("Error encoding frame\n");
		return false;
	}
	if (got_output)
	{
		int h = sws_scale(sws_ctx, (const uint8_t**)frame->data, frame->linesize, 0, height, frameYUV->data, frameYUV->linesize);
		//fwrite(frameYUV->data[1], width * height/4, 1, fp1);
		*pp_out_frame = frameYUV;
		printf("decode a frame...h=%d.............\n",h);
	}
	else
	{
        *pp_out_frame = nullptr;
		return false;
	}
	return true;
}

bool IDecoder::flush(AVFrame* frame)
{
	return false;
}

void x264Decoder::init()
{
	if (filename != NULL)
		fp_in = fopen(filename, "rb");
	pCodec = avcodec_find_decoder(AV_CODEC_ID_H264);
	pCodecCtx = avcodec_alloc_context3(pCodec);
	pCodecCtx->bit_rate = 4 * 1024 * 1024;
	pCodecCtx->width = width;
	pCodecCtx->height = height;
	pCodecCtx->frame_number = 1;
	pCodecCtx->time_base.num = 1;
	pCodecCtx->time_base.den = 13; // fps
	pCodecCtx->gop_size = 10;
	pCodecCtx->max_b_frames = 0;
	pCodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;
	pCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
	pCodecCtx->slices = 1;
	AVDictionary* opts = NULL;
	av_opt_set(pCodecCtx->priv_data, "tune", "zerolatency", 0);
	av_opt_set(pCodecCtx->priv_data, "preset", "slow", 0);
	av_dict_set(&opts, "profile", "baseline", 0);
	if (avcodec_open2(pCodecCtx, pCodec, &opts) < 0) {
		printf("Could not open codec\n");
		return;
	}
}

x264Decoder::x264Decoder(const char* filename2)
{
	Open(filename2);

	frame = av_frame_alloc();
	frameYUV = av_frame_alloc();

	int nSize = avpicture_get_size(AV_PIX_FMT_YUV420P, width, height);
	uint8_t* buff = (uint8_t*)av_malloc(nSize);
	if (!buff){
		printf("av_malloc fail");
	}
	int r1 = avpicture_fill((AVPicture*)frameYUV, (uint8_t*)buff, AV_PIX_FMT_YUV420P, width, height);
	if (r1 < 0) {
		printf("avpicture_fill fail");
	}


	sws_ctx = sws_getContext(width, height, m_pVidDecodeCtx->pix_fmt, width, height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);

	if (!sws_ctx) {
		printf("sws_getContext fail");
	}

	fp1 = fopen("fj.y", "wb");
	if (!fp1) {
		printf("fopen fail");
	}
}
x264Decoder::x264Decoder(VideoDataBuffer *vdb)
{
    Open(vdb);

    frame = av_frame_alloc();
    frameYUV = av_frame_alloc();

    int nSize = avpicture_get_size(AV_PIX_FMT_YUV420P, width, height);
    uint8_t* buff = (uint8_t*)av_malloc(nSize);
    if (!buff){
        printf("av_malloc fail");
    }
    int r1 = avpicture_fill((AVPicture*)frameYUV, (uint8_t*)buff, AV_PIX_FMT_YUV420P, width, height);
    if (r1 < 0) {
        printf("avpicture_fill fail");
    }


    sws_ctx = sws_getContext(width, height, m_pVidDecodeCtx->pix_fmt, width, height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);

    if (!sws_ctx) {
        printf("sws_getContext fail");
    }

    fp1 = fopen("fj.y", "wb");
    if (!fp1) {
        printf("fopen fail");
    }
}

x264Decoder::~x264Decoder()
{
}
