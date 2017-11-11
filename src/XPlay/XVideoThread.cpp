#include "XVideoThread.h"
#include "XFFmpeg.h"
#include <iostream>
#include "XAudioPlay.h"

#include <list>

using namespace std;
static bool isexit = false;

static list<AVPacket> videos;
static int apts = -1;


XVideoThread::XVideoThread()
{
}


XVideoThread::~XVideoThread()
{
	isexit = true;
	wait();
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

		while (videos.size() > 0)
		{
			AVPacket pack = videos.front();
			int pts = XFFmpeg::get()->GetPts(&pack);

			if (pts > apts) {
				break;
			}
			XFFmpeg::get()->Decode(&pack);

			av_packet_unref(&pack);
			videos.pop_front();
		}
		// 音频缓冲区剩余空间大小
		int free = XAudioPlay::Get()->GetFree();
        float audioSize = (XAudioPlay::Get()->channel * XAudioPlay::Get()->sampleRate * XAudioPlay::Get()->sampleSize) / (float)XFFmpeg::get()->fps / 8.0;
        if (free < audioSize) { //实际应该是 一帧的大小
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
			apts = XFFmpeg::get()->Decode(&pkt);	
			av_packet_unref(&pkt);

			int len = XFFmpeg::get()->ToPCM(out);
			XAudioPlay::Get()->Write(out, len);
		
			continue;
		}

		if (pkt.stream_index != XFFmpeg::get()->videoStream) {
			av_packet_unref(&pkt);
			continue;
		}

		//XFFmpeg::get()->Decode(&pkt);
		//av_packet_unref(&pkt);
		videos.push_back(pkt);

	/*	if (XFFmpeg::get()->fps > 0) {
			msleep(1000 / XFFmpeg::get()->fps);
		}*/
	}
}
