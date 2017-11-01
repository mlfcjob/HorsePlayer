#include "XPlay.h"
#include <QFileDialog>
#include <QMessageBox>
#include "XFFmpeg.h"

static bool isPressSlider = false;
static bool isPlay = true;

#define PAUSE "QPushButton {border-image: url(:/XPlay/Resources/Pause_Normal.ico);}"
#define PLAY "QPushButton {border-image: url(:/XPlay/Resources/Play_Normal.ico);}"

XPlay::XPlay(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	startTimer(40);
}



void  XPlay::play()
{
	isPlay = !isPlay;
	XFFmpeg::get()->isPlay = isPlay;

	if (isPlay) {
		// 播放时将按钮设置为暂停图标
		ui.playButton->setStyleSheet(PAUSE);
	}
	else {
		// 暂停时将按钮设为播放图标
		ui.playButton->setStyleSheet(PLAY);
	}
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

    isPlay = false;
	play();
}

XPlay::~XPlay()
{
}


void XPlay::resizeEvent(QResizeEvent *e)
{
	ui.openGLWidget->resize(size());

	ui.playButton->move(this->width() / 2 + 50, this->height() - 80);
	ui.openButton->move(this->width() / 2 - 50, this->height() - 80);
	ui.playSlider->move(50, this->height() - 120);
	ui.playSlider->resize(this->width() - 25, ui.playSlider->height());
	ui.playtime->move(5, ui.playButton->y());
	ui.sp->move(ui.playtime->x() + ui.playtime->width() + 5, ui.playtime->y());
	ui.totaltime->move(120, ui.playButton->y());
}


void XPlay::timerEvent(QTimerEvent  *e)
{
	int min = (XFFmpeg::get()->pts / 1000) / 60;
	int sec = (XFFmpeg::get()->pts / 1000) % 60;

	char buf[1024] = {0};
	sprintf(buf, "%03d:%02d", min, sec);
	ui.playtime->setText(buf);

	if (XFFmpeg::get()->totalMs > 0) {
		float rate = (float)XFFmpeg::get()->pts / (float)XFFmpeg::get()->totalMs;

		if (!isPressSlider) {
			ui.playSlider->setValue(rate * 1000);
		}
	}
}

void XPlay::sliderPress()
{
	isPressSlider = true;
}
void XPlay::sliderRelease()
{
	isPressSlider = false;
	float pos = 0;
	pos = (float)ui.playSlider->value() / (float)(ui.playSlider->maximum() + 1);

	XFFmpeg::get()->Seek(pos);
}
