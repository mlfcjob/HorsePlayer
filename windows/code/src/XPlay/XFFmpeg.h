#pragma once

extern "C"
{
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
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
	///// @return  bool  �ɹ�ʧ�ܣ�ʧ�ܴ�����Ϣͨ��GetError��ȡ
	bool Open(const char *path);

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
	/////  @return  AVFrame ������һ֡����
	AVFrame *Decode(const AVPacket *pkt);

	////////////////////////////////////////
	////// ת��ΪRGB
	////// @yuv   AVFrame  ������һ֡����
	////// @out   char*    ת�������ݴ洢ָ���ַ
	////// @outwidth  int   ת�����width
	////// @outheight  int  ת�����height
	bool ToRGB(const AVFrame *yuv, char *out, int outwidth, int outheight);

	/////////////////////////////////////////////
	///// ���ش�����Ϣ
	///// @para  NULL 
	///// @return  string  ���ش�����Ϣ
	std::string GetError();   //ʹ��stringΪ���̰߳�ȫ

	virtual ~XFFmpeg();
	int totalMs = 0;
	int  videoStream = 0;
protected:
	char errorbuf[1024];
	AVFormatContext *ic = NULL;
	AVFrame *yuv = NULL;
	SwsContext  *cCtx = NULL;
	QMutex mutex;
	XFFmpeg();
};


