#include "XPlay.h"
#include <QtWidgets/QApplication>

#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "swscale.lib")

extern "C"
{
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

#define NEW_API 1

static double r2d(AVRational r)
{
	return r.num == 0 || r.den == 0 ?  0 : (double)r.num /(double) r.den;
}

int main(int argc, char *argv[])
{
	av_register_all();

	//char *path = "2504.mkv";
	char *path = "swxfdemo.mp4";

	AVFormatContext *ic = NULL;

	int re = avformat_open_input(&ic, path, NULL, NULL);

	if (re != 0) {
		char buf[1024] = {0};
		av_strerror(re, buf, sizeof(buf));
		printf("%s open failed: %s \n", path, buf);
		getchar();
		return -1;
	}

	int totalSec = ic->duration / AV_TIME_BASE;
	printf("file total sec: %d-%d.\n", totalSec / 60, totalSec % 60);

	int videoStream = 0;
	AVCodecContext   *videoCtx = NULL;

	for (int i = 0; i < ic->nb_streams; i++)
	{
		AVCodecContext *enc = ic->streams[i]->codec;
		
		if (enc->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			videoStream = i;

			AVCodec *codec = avcodec_find_decoder(enc->codec_id);

			if (!codec) 
			{
				printf("video code not find. \n");
				return -1;
			}

			int err = avcodec_open2(enc, codec, NULL);
			if (err != 0)
			{
				char buf[1024] = {0};
				av_strerror(err, buf, sizeof(buf));
				printf(buf);
				return -2;
			}

			videoCtx = enc;
			printf("open decoder success. \n");
		}
	}

	AVFrame *yuv = av_frame_alloc();
	SwsContext *cCtx = NULL;
	int outwidth = 640;
	int outheight = 480;

	char *rgb = new char[outwidth * outheight * 4];

	for (;;) {
		AVPacket pkt;
		re = av_read_frame(ic, &pkt);
		if (re != 0) break;
		int pts = pkt.pts * r2d(ic->streams[pkt.stream_index]->time_base) * 1000;

#ifdef NEW_API
		int re = avcodec_send_packet(videoCtx, &pkt);
		if (re != 0)
		{
			av_packet_unref(&pkt);
			continue;
		}

		re = avcodec_receive_frame(videoCtx, yuv);
		if (re != 0)
		{
			av_packet_unref(&pkt);
			continue;
		}
		printf("[D]  decoder success. \n");
#else
		int got_picture = 0;
		int re = avcodec_decode_video2(videoCtx, yuv, &got_picture, &pkt);
		if (got_picture)
		{
			printf("size: [%d] \n", re);
		}
#endif

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
			break;
		}

		uint8_t *data[AV_NUM_DATA_POINTERS] = {0};
		data[0] = (uint8_t *)rgb;

		int linesize[AV_NUM_DATA_POINTERS] = { 0 };
		linesize[0] = outwidth * 4;

		int h = sws_scale(cCtx, yuv->data, yuv->linesize, 0, videoCtx->height,
			 data,
			 linesize);

		if (h > 0) {
			printf("(%d) .\n", h);
		}
		//printf("pts = %d.\n", pts);
		av_packet_unref(&pkt);
	}
	

	if (cCtx)
	{
		sws_freeContext(cCtx);
		cCtx = NULL;
	}

	if (rgb) {
		delete rgb;
	}

	avformat_close_input(&ic);


	QApplication a(argc, argv);
	XPlay w;
	w.show();
	return a.exec();
}
