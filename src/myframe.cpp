#include "myframe.h"
#include "ui_myframe.h"

myFrame::myFrame(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::myFrame)
{
    ui->setupUi(this);
}

myFrame::~myFrame()
{
    delete ui;
}

void myFrame::setVersionLabel(QString text)
{
    ui->versionLabel->setText(text);
}

void myFrame::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    QRectF r1(0.0, 0.0, 220.0, 650.0);
    painter.drawImage(r1,QImage(":/icons/icons/logo.png"));

    QFrame::paintEvent(e);
}
