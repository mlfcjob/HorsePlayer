#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_firstQt.h"

class firstQt : public QMainWindow
{
	Q_OBJECT

public:
	firstQt(QWidget *parent = Q_NULLPTR);

private:
	Ui::firstQtClass ui;
};
