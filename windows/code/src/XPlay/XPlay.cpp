#include "XPlay.h"
#include <QFileDialog>
#include <QMessageBox>
#include "XFFmpeg.h"

XPlay::XPlay(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}

void XPlay::open()
{
	QString name = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("选择视频文件"));

	if (name.isEmpty()) {
		return;
	}

	this->setWindowTitle(name);

	if (!XFFmpeg::get()->Open(name.toLocal8Bit()))
	{
		QMessageBox::information(this, "err", "file open failed");
		return;
	}
}

XPlay::~XPlay()
{
}

