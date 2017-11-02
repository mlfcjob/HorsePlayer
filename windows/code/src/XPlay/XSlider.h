#pragma once

#include <QSlider>

class XSlider: public  QSlider
{
	Q_OBJECT

public:
	XSlider(QWidget *p = NULL);
	~XSlider();

	void mousePressEvent(QMouseEvent  *e);
};

