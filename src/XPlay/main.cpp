#include "XPlay.h"
#include <QtWidgets/QApplication>
#include <QAudioOutput>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	XPlay w;
	w.show();
	return a.exec();
}
