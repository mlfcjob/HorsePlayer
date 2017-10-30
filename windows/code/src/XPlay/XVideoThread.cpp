#include "XVideoThread.h"
#include "XFFmpeg.h"
#include <iostream>

static bool isexit = false;

XVideoThread::XVideoThread()
{
}


XVideoThread::~XVideoThread()
{
	isexit = true;
}


void XVideoThread::run()
{
	while (!isexit) {
		// ÔÝÍ£×´Ì¬
		if (!XFFmpeg::get()->isPlay) {
			msleep(10);
			continue;
		}

		AVPacket pkt = XFFmpeg::get()->Read();
		if (pkt.size <= 0) {
			msleep(10);
			continue;
		}

		//ÒôÆµºÍ×ÖÄ»ÔÝ²»´¦Àí
		if (pkt.stream_index != XFFmpeg::get()->videoStream) {
			av_packet_unref(&pkt);
			continue;
		}

		XFFmpeg::get()->Decode(&pkt);
		av_packet_unref(&pkt);

		if (XFFmpeg::get()->fps > 0) {
			msleep(1000 / XFFmpeg::get()->fps);
		}
	}
}