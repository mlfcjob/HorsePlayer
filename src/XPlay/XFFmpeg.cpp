#include "XFFmpeg.h"

#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "swscale.lib")
#pragma comment(lib, "swresample.lib")


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
		else if (enc->codec_type == AVMEDIA_TYPE_AUDIO) {
			audioStream = i;

			AVCodec *codec = avcodec_find_decoder(enc->codec_id);

			int err;
			if (err = avcodec_open2(enc, codec, NULL) < 0) {
				mutex.unlock();
				av_strerror(err, errorbuf, sizeof(errorbuf));
				printf("open decode error: %s. \n", errorbuf);
				return false;
			}

			this->sampleRate = enc->sample_rate;
			this->channel = enc->channels;

			switch (enc->sample_fmt) {
			case AV_SAMPLE_FMT_U8:          ///< unsigned 8 bits
			case AV_SAMPLE_FMT_U8P:         ///< unsigned 8 bits, planar
				break;
			case AV_SAMPLE_FMT_FLTP:        ///< float, planar
			case AV_SAMPLE_FMT_DBLP:        ///< double, planar
			case AV_SAMPLE_FMT_FLT:         ///< float
			case AV_SAMPLE_FMT_DBL:         ///< double
				break;
			case AV_SAMPLE_FMT_S64:         ///< signed 64 bits
			case AV_SAMPLE_FMT_S64P:        ///< signed 64 bits, planar
				break;
			case AV_SAMPLE_FMT_S32:         ///< signed 32 bits
			case AV_SAMPLE_FMT_S32P:        ///< signed 32 bits, planar
				this->sampleSize = 32;
				break;
			case AV_SAMPLE_FMT_S16P:        ///< signed 16 bits, planar
			case AV_SAMPLE_FMT_S16:
				this->sampleSize = 16;
				break;
			default:
				break;
			}

			printf("audio smaple_rate: %d, smapel_size: %d, channel: %d. \n", this->sampleRate, this->sampleSize, this->channel);
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

	if (aCtx) {
		swr_free(&aCtx);
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


/////////////////////////////
///// 获取包的pts
///// @para AVPacket   一帧数据
///// return int   pts
int XFFmpeg::GetPts(const AVPacket *pkt)
{
	mutex.lock();
	if (!ic) {
		mutex.unlock();
		return -1;
	}

	int pts = (pkt->pts * r2d(ic->streams[pkt->stream_index]->time_base)) * 1000;

	mutex.unlock();
	return pts;

}

///////////////////////////////////
/////  解码一帧
/////  @para AVPacket 一帧数据
/////  @return  int  返回解码的pts
int XFFmpeg::Decode(const AVPacket *pkt)
{
	mutex.lock();
	if (!ic) {
		mutex.unlock();
		return NULL;
	}

	if (yuv == NULL) {
		yuv = av_frame_alloc();
	}

	if (pcm == NULL)
	{
		pcm = av_frame_alloc();
	}

	AVFrame *frame = yuv;
	if (pkt->stream_index == audioStream){
		frame = pcm;
	}

	int re = avcodec_send_packet(ic->streams[pkt->stream_index]->codec, pkt);
	if (re != 0) {
		mutex.unlock();
		av_strerror(re, errorbuf, sizeof(errorbuf));
		printf("send_packet_error: %s. \n", errorbuf);
		return NULL;
	}

	re = avcodec_receive_frame(ic->streams[pkt->stream_index]->codec, frame);
	if (re != 0) {
		mutex.unlock();
		av_strerror(re, errorbuf, sizeof(errorbuf));
		printf("receive_frame_error: %s.\n", errorbuf);
		return NULL;
	}

	mutex.unlock();

	int p = frame->pts * r2d(ic->streams[pkt->stream_index]->time_base) * 1000;
	if (pkt->stream_index == audioStream) {
		this->pts = p;
	}

	return p;
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
		printf("scale failed (%d) .\n", h);
		mutex.unlock();
		return false;
	}

	mutex.unlock();
	return true;
}

////////////////////////////////////////
////// 转换为PCM
////// @out   char*    转换后数据存储指向地址
////// @return  int  转换后的大小
int  XFFmpeg::ToPCM(char *out)
{
	mutex.lock();

	if (!ic || !pcm || !out){
		mutex.unlock();
		return 0;
	}

	AVCodecContext *ctx = ic->streams[audioStream]->codec;
	if (aCtx == NULL) {
		aCtx = swr_alloc();
		swr_alloc_set_opts(aCtx, ctx->channel_layout, AV_SAMPLE_FMT_S16, 
			               ctx->sample_rate, ctx->channels, 
			               ctx->sample_fmt, ctx->sample_rate,
			               0, NULL);

		swr_init(aCtx);
	}

	uint8_t *data[1];
	data[0] = (uint8_t *)out;
	int len = swr_convert(aCtx, data, 10000, (const uint8_t**)pcm->data, pcm->nb_samples);

	if (len <= 0){
		mutex.unlock();
		return 0;
	}

	int outsize = av_samples_get_buffer_size(NULL, ctx->channels, pcm->nb_samples, AV_SAMPLE_FMT_S16, 0);

	mutex.unlock();

	return outsize;
}


////////////////////////////////////////
////// 拖动视频到指定位置
////// @pos   float     视频播放的百分比  0.0 - 1.0
////// 
////// @return  bool  视频跳转是否成功
bool  XFFmpeg::Seek(float  pos)
{
	mutex.lock();
	if (!ic) {
		mutex.unlock();
		return false;
	}

	int64_t stamp = 0;
	stamp = pos * ic->streams[videoStream]->duration;

	int re = av_seek_frame(ic, videoStream, stamp, AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_FRAME);

	avcodec_flush_buffers(ic->streams[videoStream]->codec);

	pts = stamp * r2d(ic->streams[videoStream]->time_base) * 1000;

	mutex.unlock();

	if (re >= 0) {
		return true;
	} else {
		return false;
	}
}



std::string XFFmpeg::GetError()   //使用string为了线程安全
{
	mutex.lock();
	std::string re = this->errorbuf;
	mutex.unlock();
	return re;
}