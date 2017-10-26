#include "XPlay.h"
#include <QtWidgets/QApplication>

#include "XFFmpeg.h"

#define NEW_API 1

static double r2d(AVRational r)
{
	return r.num == 0 || r.den == 0 ?  0 : (double)r.num /(double) r.den;
}

int main(int argc, char *argv[])
{

	/*if (XFFmpeg::get()->Open("2504.mkv"))
	{
		printf("Open sucess .\n");
	}
	else
	{
		printf("open failed: %s. \n", XFFmpeg::get()->GetError().c_str());
		getchar();
		return -1;
	}

	char *rgb = new char[ 800 * 600 * 4 ];
	for (;;) {
		AVPacket  pkt = XFFmpeg::get()->Read();
		if (pkt.size == 0)
			break;
		printf("pts=%lld \n", pkt.pts);
		if (pkt.stream_index != XFFmpeg::get()->videoStream) {
			av_packet_unref(&pkt);
			continue;
		}

		AVFrame *yuv = XFFmpeg::get()->Decode(&pkt);
		if (yuv) {
			printf("decode success. \n");
			XFFmpeg::get()->ToRGB(yuv, rgb, 800, 600);
		}

		av_packet_unref(&pkt);
	}*/


	QApplication a(argc, argv);
	XPlay w;
	w.show();
	return a.exec();
}
