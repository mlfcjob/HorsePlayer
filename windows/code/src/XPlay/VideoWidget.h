#pragma once

#include <QtWidgets/qwidget.h>
#include <QOpenGLWidget>

class VideoWidget : public QOpenGLWidget
{
public:
	VideoWidget(QWidget  *p = NULL);
	//���ڵ����»���
	void paintEvent(QPaintEvent *e);
	//��ʱ������ˢ��
	void timerEvent(QTimerEvent *e);
	virtual ~VideoWidget();
};

