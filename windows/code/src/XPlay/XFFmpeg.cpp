#include "XFFmpeg.h"

#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "swscale.lib")

static double r2d(AVRational r)
{
	return r.num == 0 || r.den == 0 ? 0 : (double)r.num / (double)r.den;
}


XFFmpeg::XFFmpeg()
{
	errorbuf[0] = '\0';
	av_register_all();
}


XFFmpeg::~XFFmpeg()
{
}


int XFFmpeg::Open(const char *path)
{
	Close();

	mutex.lock();
	int re = avformat_open_input(&ic, path, NULL, NULL);
	if (re != 0) {
		mutex.unlock();
		av_strerror(re, errorbuf, sizeof(errorbuf));
		printf("%s open failed: %s \n", path, errorbuf);
		return 0;
	}

	for (int i = 0; i < ic->nb_streams; i++)
	{
		AVCodecContext *enc = ic->streams[i]->codec;
				
		if (enc->codec_type == AVMEDIA_TYPE_VIDEO){
			videoStream = i;

			fps = r2d(ic->streams[i]->avg_frame_rate);
			AVCodec *codec = avcodec_find_decoder(enc->codec_id);
			if (!codec) {
				printf("video code not find. \n");
				mutex.unlock();
				return 0;
			}

			int err = avcodec_open2(enc, codec, NULL);
			if (err != 0){
				mutex.unlock();
				av_strerror(err, errorbuf, sizeof(errorbuf));
				printf("open decode error: %s. \n", errorbuf);
				return 0;
			}
			printf("open decoder success. \n");
		}
	}

	totalMs = (ic->duration / AV_TIME_BASE) * 1000;  //ms


	mutex.unlock();

	return totalMs;
}

bool XFFmpeg::Close()
{
	mutex.lock();
	if (ic) {
		avformat_close_input(&ic); 
	}

	if (yuv) {
		av_frame_free(&yuv);
	}

	if (cCtx) {
		sws_freeContext(cCtx);
		cCtx = NULL;
	}
	mutex.unlock();

	return true;
}

/////////////////////////////////////////////
///// 读取一帧
///// @para  NULL
///// @return  AVPacket 一帧的结构
AVPacket XFFmpeg::Read()
{
	AVPacket pkt;
	memset(&pkt, 0, sizeof(AVPacket));
	mutex.lock();
	if (!ic) {
		mutex.unlock();
		return pkt;
	}

	int err = av_read_frame(ic, &pkt);
	if (err != 0) {
		av_strerror(err, errorbuf, sizeof(errorbuf));
		printf("read frame error: %s. \n", errorbuf);
	}
	mutex.unlock();

	return pkt;
}

///////////////////////////////////
/////  解码一帧
/////  @para AVPacket 一帧数据
/////  @return  AVFrame 解码后的一帧数据
AVFrame * XFFmpeg::Decode(const AVPacket *pkt)
{
	mutex.lock();
	if (!ic) {
		mutex.unlock();
		return NULL;
	}

	if (yuv == NULL) {
		yuv = av_frame_alloc();
	}

	int re = avcodec_send_packet(ic->streams[pkt->stream_index]->codec, pkt);
	if (re != 0) {
		mutex.unlock();
		av_strerror(re, errorbuf, sizeof(errorbuf));
		printf("send_packet_error: %s. \n", errorbuf);
		return NULL;
	}

	re = avcodec_receive_frame(ic->streams[pkt->stream_index]->codec, yuv);
	if (re != 0) {
		mutex.unlock();
		av_strerror(re, errorbuf, sizeof(errorbuf));
		printf("receive_frame_error: %s.\n", errorbuf);
		return NULL;
	}

	mutex.unlock();

	pts = yuv->pts * r2d(ic->streams[pkt->stream_index]->time_base) * 1000;
	return yuv;
}



////////////////////////////////////////
////// 转换为RGB
////// @out   char*    转换后数据存储指向地址
////// @outwidth  int   转换后的width
////// @outheight  int  转换后的height
bool XFFmpeg::ToRGB(char *out, int outwidth, int outheight)
{
	mutex.lock();

	if (!ic || !yuv) {
		mutex.unlock();
		return false;
	}

	AVCodecContext *videoCtx = ic->streams[this->videoStream]->codec;
	cCtx = sws_getCachedContext(cCtx,
				videoCtx->width,
				videoCtx->height,
				videoCtx->pix_fmt,
				outwidth,
				outheight,
				AV_PIX_FMT_BGRA,
				SWS_BICUBIC,
				NULL,
				NULL,
				NULL);
	
	if (!cCtx) {
		printf("sws_getCachedContext failed! \n");
		mutex.unlock();
		return false;
	}

	uint8_t *data[AV_NUM_DATA_POINTERS] = {0};
	data[0] = (uint8_t *)out;
	
	int linesize[AV_NUM_DATA_POINTERS] = { 0 };
	linesize[0] = outwidth * 4;
	
	int h = sws_scale(cCtx, yuv->data, yuv->linesize, 0, 
		              videoCtx->height,data, linesize);
	
	if (h <= 0) {
		printf("(%d) .\n", h);
		mutex.unlock();
		return false;
	}
	else {
		printf(" scale success: (%d). \n", h);
	}

	mutex.unlock();
	return true;
}


std::string XFFmpeg::GetError()   //使用string为了线程安全
{
	mutex.lock();
	std::string re = this->errorbuf;
	mutex.unlock();
	return re;
}