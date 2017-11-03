#include "XVideoThread.h"
#include "XFFmpeg.h"
#include <iostream>
#include "XAudioPlay.h"

static bool isexit = false;

XVideoThread::XVideoThread()
{
}


XVideoThread::~XVideoThread()
{
}


void XVideoThread::run()
{
	char out[10000] = { 0 };
	while (!isexit) {
		// 暂停状态
		if (!XFFmpeg::get()->isPlay) {
			msleep(10);
			continue;
		}

		// 音频缓冲区剩余空间大小
		int free = XAudioPlay::Get()->GetFree();
		if (free < 10000) { //实际应该是 一帧的大小
			msleep(1);
			continue;
		}

		AVPacket pkt = XFFmpeg::get()->Read();
		if (pkt.size <= 0) {
			msleep(10);
			continue;
		}

		//字幕暂不处理
		if (pkt.stream_index == XFFmpeg::get()->audioStream) {
			XFFmpeg::get()->Decode(&pkt);	
			av_packet_unref(&pkt);

			int len = XFFmpeg::get()->ToPCM(out);
			XAudioPlay::Get()->Write(out, len);
		
			continue;
		}

		if (pkt.stream_index != XFFmpeg::get()->videoStream) {
			av_packet_unref(&pkt);
			continue;
		}

		XFFmpeg::get()->Decode(&pkt);
		av_packet_unref(&pkt);

	/*	if (XFFmpeg::get()->fps > 0) {
			msleep(1000 / XFFmpeg::get()->fps);
		}*/
	}
}