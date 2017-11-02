#include "XAudioPlay.h"
#include <qaudiooutput.h>

class CXAudioPlay : public XAudioPlay
{
public:
	QAudioOutput *output = NULL;
	QIODevice *io = NULL;

	void Stop(){
		if (output) {
			output->stop();
			delete output;
			output = NULL;
			io = NULL;
		}
	}

	bool Start(){
		Stop();

		QAudioFormat fmt;

		fmt.setSampleRate(sampleReate);
		fmt.setSampleSize(sampleSize);
		fmt.setChannelCount(channel);
		fmt.setCodec("audio/pcm");
		fmt.setByteOrder(QAudioFormat::LittleEndian);
		fmt.setSampleType(QAudioFormat::UnSignedInt);

		output = new QAudioOutput(fmt);
		io = output->start();

		return true;
	}

	void Play(bool isPlay) {
		if (!output){
			return;
		}

		if (isPlay) {
			output->resume();   //»Ö¸´²¥·Å
		}
		else {
			output->suspend();   //ÔÝÍ£²¥·Å
		}
	}

	bool Write(const char *data, int datasize) {
		if (io) {
			io->write(data, datasize);
		}
	
		return true;
	}

	int GetFree(){
		if (!output) {
			return 0;
		}

		return output->bytesFree();
	}

};

XAudioPlay::XAudioPlay()
{
}

XAudioPlay *XAudioPlay::Get() {
	static CXAudioPlay ap;
	return &ap;
}


XAudioPlay::~XAudioPlay()
{
}
