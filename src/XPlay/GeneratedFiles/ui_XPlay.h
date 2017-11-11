/********************************************************************************
** Form generated from reading UI file 'XPlay.ui'
**
** Created by: Qt User Interface Compiler version 5.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_XPLAY_H
#define UI_XPLAY_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>
#include "videowidget.h"
#include "xslider.h"

QT_BEGIN_NAMESPACE

class Ui_XPlayClass
{
public:
    VideoWidget *openGLWidget;
    QPushButton *openButton;
    QPushButton *playButton;
    QLabel *totaltime;
    QLabel *playtime;
    QLabel *sp;
    XSlider *playSlider;

    void setupUi(QWidget *XPlayClass)
    {
        if (XPlayClass->objectName().isEmpty())
            XPlayClass->setObjectName(QStringLiteral("XPlayClass"));
        XPlayClass->resize(800, 600);
        QIcon icon;
        icon.addFile(QStringLiteral(":/XPlay/Resources/player.ico"), QSize(), QIcon::Normal, QIcon::Off);
        XPlayClass->setWindowIcon(icon);
        openGLWidget = new VideoWidget(XPlayClass);
        openGLWidget->setObjectName(QStringLiteral("openGLWidget"));
        openGLWidget->setGeometry(QRect(0, 0, 800, 600));
        openButton = new QPushButton(XPlayClass);
        openButton->setObjectName(QStringLiteral("openButton"));
        openButton->setGeometry(QRect(360, 550, 61, 31));
        openButton->setStyleSheet(QLatin1String("QPushButton:!hover {border-image: url(:/XPlay/Resources/open_normal.ico);}\n"
"QPushButton:hover {border-image: url(:/XPlay/Resources/open_hot.ico);}"));
        playButton = new QPushButton(XPlayClass);
        playButton->setObjectName(QStringLiteral("playButton"));
        playButton->setGeometry(QRect(440, 550, 41, 31));
        playButton->setStyleSheet(QLatin1String("QPushButton::!hover {border-image: url(:/XPlay/Resources/Play_Disabled.ico);}\n"
"QPushButton::hover {border-image: url(:/XPlay/Resources/Play_Normal.ico);}"));
        totaltime = new QLabel(XPlayClass);
        totaltime->setObjectName(QStringLiteral("totaltime"));
        totaltime->setGeometry(QRect(280, 560, 61, 21));
        totaltime->setStyleSheet(QLatin1String("background-color: rgb(255, 255, 255);\n"
"font: 63 10pt \"Adobe Caslon Pro\";"));
        playtime = new QLabel(XPlayClass);
        playtime->setObjectName(QStringLiteral("playtime"));
        playtime->setGeometry(QRect(190, 560, 61, 21));
        playtime->setStyleSheet(QLatin1String("background-color: rgb(255, 255, 255);\n"
"font: 63 10pt \"Adobe Caslon Pro\";"));
        sp = new QLabel(XPlayClass);
        sp->setObjectName(QStringLiteral("sp"));
        sp->setGeometry(QRect(260, 560, 16, 21));
        sp->setStyleSheet(QLatin1String("background-color: rgb(255, 255, 255);\n"
"font: 63 10pt \"Adobe Caslon Pro\";"));
        playSlider = new XSlider(XPlayClass);
        playSlider->setObjectName(QStringLiteral("playSlider"));
        playSlider->setGeometry(QRect(20, 520, 761, 19));
        playSlider->setMaximum(999);
        playSlider->setPageStep(100);
        playSlider->setValue(0);
        playSlider->setOrientation(Qt::Horizontal);

        retranslateUi(XPlayClass);
        QObject::connect(openButton, SIGNAL(clicked()), XPlayClass, SLOT(open()));
        QObject::connect(playSlider, SIGNAL(sliderPressed()), XPlayClass, SLOT(sliderPress()));
        QObject::connect(playSlider, SIGNAL(sliderReleased()), XPlayClass, SLOT(sliderRelease()));
        QObject::connect(playButton, SIGNAL(clicked()), XPlayClass, SLOT(play()));

        QMetaObject::connectSlotsByName(XPlayClass);
    } // setupUi

    void retranslateUi(QWidget *XPlayClass)
    {
        XPlayClass->setWindowTitle(QApplication::translate("XPlayClass", "XPlay", Q_NULLPTR));
        openButton->setText(QString());
        playButton->setText(QString());
        totaltime->setText(QApplication::translate("XPlayClass", "00:00:00", Q_NULLPTR));
        playtime->setText(QApplication::translate("XPlayClass", "00:00:00", Q_NULLPTR));
        sp->setText(QApplication::translate("XPlayClass", "/", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class XPlayClass: public Ui_XPlayClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_XPLAY_H
