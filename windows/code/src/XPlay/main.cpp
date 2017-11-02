#include "XPlay.h"
#include <QtWidgets/QApplication>
#include <QAudioOutput>

#include "XFFmpeg.h"

#define NEW_API 1


int main(int argc, char *argv[])
{
	QAudioOutput *out;
	QAudioFormat fmt;

	fmt.setSampleRate(48000);
	fmt.setSampleSize(16);
	fmt.setChannelCount(2);
	fmt.setCodec("audio/pcm");
	fmt.setByteOrder(QAudioFormat::LittleEndian);
	fmt.setSampleType(QAudioFormat::UnSignedInt);

	out = new QAudioOutput(fmt);
	QIODevice  *ad = out->start();

	//ad->write();
	//ad->read();
	//ad->reset();

	QApplication a(argc, argv);
	XPlay w;
	w.show();
	return a.exec();
}
