#include "XAudioPlay.h"
#include <qaudiooutput.h>
#include <QMutex>

class CXAudioPlay : public XAudioPlay
{
public:
	QAudioOutput *output = NULL;
	QIODevice *io = NULL;
	QMutex mutex;

	void Stop(){
		mutex.lock();
		if (output) {
			output->stop();
			delete output;
			output = NULL;
			io = NULL;
		}

		mutex.unlock();
	}

	bool Start(){
		Stop();

		mutex.lock();

		QAudioFormat fmt;

		fmt.setSampleRate(sampleRate);
		fmt.setSampleSize(sampleSize);
		fmt.setChannelCount(channel);
		fmt.setCodec("audio/pcm");
		fmt.setByteOrder(QAudioFormat::LittleEndian);
		fmt.setSampleType(QAudioFormat::UnSignedInt);

		output = new QAudioOutput(fmt);
		io = output->start();

		mutex.unlock();
		return true;
	}

	void Play(bool isPlay) {
		mutex.lock();

		if (!output){
			mutex.unlock();
			return;
		}

		if (isPlay) {
			output->resume();   //»Ö¸´²¥·Å
		}
		else {
			output->suspend();   //ÔÝÍ£²¥·Å
		}

		mutex.unlock();
	}

	bool Write(const char *data, int datasize) {
		if (!data || datasize <= 0)
			return false;

		mutex.lock();
		if (io) {
			io->write(data, datasize);
		}

		mutex.unlock();

		return true;
	}

	int GetFree(){
		mutex.lock();
		if (!output) {
			mutex.unlock();
			return 0;
		}

		int free = output->bytesFree();
		mutex.unlock();
		return free;
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
