#include "firstQt.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	firstQt w;
	w.show();
	return a.exec();
}
