#include "VideoWidget.h"
#include <QPainter>
#include "XFFmpeg.h"

VideoWidget::VideoWidget(QWidget *p) : QOpenGLWidget(p)
{
	XFFmpeg::get()->Open("2504.mkv");
	startTimer(20);
}


VideoWidget::~VideoWidget()
{
}

//���ڵ����»���
void VideoWidget::paintEvent(QPaintEvent *e)
{
	static QImage  *image = NULL;

	if (image == NULL) {
		uchar *buf = new uchar[width() * height() * 4];
		image = new QImage(buf, width(), height(), QImage::Format_ARGB32);
	}

	AVPacket pkt = XFFmpeg::get()->Read();
	if (pkt.stream_index != XFFmpeg::get()->videoStream)
	{
		av_packet_unref(&pkt);
		return;
	}

	if (pkt.size == 0) return;
	AVFrame *yuv =  XFFmpeg::get()->Decode(&pkt);
	av_packet_unref(&pkt);

	if (yuv == NULL)  return;

	XFFmpeg::get()->ToRGB(yuv, (char*)image->bits(), width(), height());

	QPainter  painter;
	painter.begin(this);
	painter.drawImage(QPoint(0, 0), *image);
	painter.end();
}
//��ʱ������ˢ��
void VideoWidget::timerEvent(QTimerEvent *e)
{
	this->update();
}
