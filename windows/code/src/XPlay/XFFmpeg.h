#pragma once

extern "C"
{
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}

#include <string>
#include <QMutex>

class XFFmpeg
{
public:
	static XFFmpeg *get() {
		static XFFmpeg ff;
		return &ff;
	}

	/////////////////////////////////////////////
	/////  ����Ƶ�ļ�������ϴ��Ѿ��򿪣��ȹر�
	///// @para  path  ��Ƶ�ļ�·��
	///// @return  int  return total ms
	int Open(const char *path);

	/////////////////////////////////////////////
	/////  �ر�
	///// 
	///// @return  bool   �Ƿ�رճɹ�
	bool Close();

	/////////////////////////////////////////////
	///// ��ȡһ֡
	///// @para  NULL
	///// @return  AVPacket һ֡�Ľṹ ����ֵ��Ҫ�û�����
	AVPacket Read();

	///////////////////////////////////
	/////  ����һ֡
    /////  @para AVPacket һ֡����
	/////  @return  int  ���ؽ����pts
	int Decode(const AVPacket *pkt);

	////////////////////////////////////////
	////// ת��ΪRGB
	////// @out   char*    ת�������ݴ洢ָ���ַ
	////// @outwidth  int   ת�����width
	////// @outheight  int  ת�����height
	////// @return  bool    �����Ƿ�ת���ɹ�
	bool ToRGB(char *out, int outwidth, int outheight);

	////////////////////////////////////////
	////// ת��ΪPCM
	////// @out   char*    ת�������ݴ洢ָ���ַ
	////// @return  int  ת����Ĵ�С
	int  ToPCM(char *out);

	////////////////////////////////////////
	////// �϶���Ƶ��ָ��λ��
	////// @pos   float     ��Ƶ���ŵİٷֱ�  0.0 - 1.0
	////// 
	////// @return  bool  ��Ƶ��ת�Ƿ�ɹ�
	bool Seek(float  pos);

	/////////////////////////////////////////////
	///// ���ش�����Ϣ
	///// @para  NULL 
	///// @return  string  ���ش�����Ϣ
	std::string GetError();   //ʹ��stringΪ���̰߳�ȫ

	virtual ~XFFmpeg();
	int  totalMs = 0;
	int  videoStream = 0;
	int  audioStream = 1;
	int  fps = 0;
	int  pts = 0;
	bool isPlay = false;
	int  sampleRate = 48000;
	int  sampleSize = 16;
	int  channel    = 2;
protected:
	char errorbuf[1024];
	AVFormatContext *ic = NULL;
	AVFrame *yuv = NULL;
	AVFrame *pcm = NULL;
	SwsContext  *cCtx = NULL;
	SwrContext  *aCtx = NULL;
	QMutex mutex;
	XFFmpeg();
};


