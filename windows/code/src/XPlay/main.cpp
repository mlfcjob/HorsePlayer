#include "XPlay.h"
#include <QtWidgets/QApplication>

#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "avcodec.lib")
extern "C"
{
#include <libavformat/avformat.h>
}

static double r2d(AVRational r)
{
	return r.num == 0 || r.den == 0 ?  0 : (double)r.num /(double) r.den;
}

int main(int argc, char *argv[])
{
	av_register_all();

	char *path = "2504.mkv";

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

	for (;;) {
		AVPacket pkt;
		re = av_read_frame(ic, &pkt);
		if (re != 0) break;
		int pts = pkt.pts * r2d(ic->streams[pkt.stream_index]->time_base) * 1000;

		printf("pts = %d.\n", pts);
		av_packet_unref(&pkt);
	}
	

	avformat_close_input(&ic);


	QApplication a(argc, argv);
	XPlay w;
	w.show();
	return a.exec();
}
