#pragma once
class XAudioPlay
{
public:
	static XAudioPlay *Get();
	virtual bool Start() = 0;
	virtual void Play(bool isPlay) = 0;
	virtual bool Write(const char *data, int datasize) = 0;
	virtual void Stop() = 0;
	virtual int  GetFree() = 0;   //��ȡʣ��ռ�
	int sampleRate = 48000;
	int sampleSize = 16;
	int channel = 2;

	virtual ~XAudioPlay();
protected:
	XAudioPlay();
};

