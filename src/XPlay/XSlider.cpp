#include "XSlider.h"
#include <QMouseEvent>


XSlider::XSlider(QWidget *p): QSlider(p)
{

}


XSlider::~XSlider()
{
}


void XSlider::mousePressEvent(QMouseEvent  *e)
{
	int value = ((float)e->pos().x() / (float)this->width()) * this->maximum();
	this->setValue(value);
	QSlider::mousePressEvent(e);
}