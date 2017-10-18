#include "firstQt.h"
#include <QtWidgets/QApplication>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avcodec.lib")

int main(int argc, char *argv[])
{
	av_register_all();
	QApplication a(argc, argv);
	firstQt w;
	w.show();
	return a.exec();
}
