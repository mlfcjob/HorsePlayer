#pragma once

#include <QtWidgets/qwidget.h>
#include <QOpenGLWidget>

class VideoWidget : public QOpenGLWidget
{
public:
	VideoWidget(QWidget  *p = NULL);
	//窗口的重新绘制
	void paintEvent(QPaintEvent *e);
	//定时器界面刷新
	void timerEvent(QTimerEvent *e);
	virtual ~VideoWidget();
};

