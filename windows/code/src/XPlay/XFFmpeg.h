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
	/////  打开视频文件，如果上次已经打开，先关闭
	///// @para  path  视频文件路径
	///// @return  int  return total ms
	int Open(const char *path);

	/////////////////////////////////////////////
	/////  关闭
	///// 
	///// @return  bool   是否关闭成功
	bool Close();

	/////////////////////////////////////////////
	///// 读取一帧
	///// @para  NULL
	///// @return  AVPacket 一帧的结构 返回值需要用户清理
	AVPacket Read();

	///////////////////////////////////
	/////  解码一帧
    /////  @para AVPacket 一帧数据
	/////  @return  AVFrame 解码后的一帧数据
	AVFrame *Decode(const AVPacket *pkt);

	////////////////////////////////////////
	////// 转换为RGB
	////// @out   char*    转换后数据存储指向地址
	////// @outwidth  int   转换后的width
	////// @outheight  int  转换后的height
	bool ToRGB(char *out, int outwidth, int outheight);

	////////////////////////////////////////
	////// 拖动视频到指定位置
	////// @pos   float     视频播放的百分比  0.0 - 1.0
	////// 
	////// @return  bool  视频跳转是否成功
	bool Seek(float  pos);

	/////////////////////////////////////////////
	///// 返回错误信息
	///// @para  NULL 
	///// @return  string  返回错误信息
	std::string GetError();   //使用string为了线程安全

	virtual ~XFFmpeg();
	int totalMs = 0;
	int  videoStream = 0;
	int fps = 0;
	int pts = 0;
	bool isPlay = false;
protected:
	char errorbuf[1024];
	AVFormatContext *ic = NULL;
	AVFrame *yuv = NULL;
	SwsContext  *cCtx = NULL;
	QMutex mutex;
	XFFmpeg();
};


