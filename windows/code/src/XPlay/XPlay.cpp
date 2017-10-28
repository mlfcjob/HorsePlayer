#include "XPlay.h"
#include <QFileDialog>
#include <QMessageBox>
#include "XFFmpeg.h"

XPlay::XPlay(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	startTimer(40);
}

void XPlay::open()
{
	QString name = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("选择视频文件"));

	if (name.isEmpty()) {
		return;
	}

	this->setWindowTitle(name);

	int totalMs = XFFmpeg::get()->Open(name.toLocal8Bit());
	if (totalMs <= 0)
	{
		QMessageBox::information(this, "err", "file open failed");
		return;
	}

	char buf[1024] = {0};
	int min = (totalMs / 1000) / 60;
	int sec = (totalMs / 1000) % 60;
	sprintf(buf, "%03d:%02d", min, sec);
	ui.totaltime->setText(buf); 
}

XPlay::~XPlay()
{
}

void XPlay::timerEvent(QTimerEvent  *e)
{
	int min = (XFFmpeg::get()->pts / 1000) / 60;
	int sec = (XFFmpeg::get()->pts / 1000) % 60;

	char buf[1024] = {0};
	sprintf(buf, "%03d:%02d", min, sec);
	ui.playtime->setText(buf);
}