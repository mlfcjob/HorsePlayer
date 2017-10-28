#include "XPlay.h"
#include <QtWidgets/QApplication>

#include "XFFmpeg.h"

#define NEW_API 1


int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	XPlay w;
	w.show();
	return a.exec();
}
