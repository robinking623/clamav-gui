#ifndef MYFRAME_H
#define MYFRAME_H

#include <QFrame>
#include <QPainter>
#include <QRectF>

namespace Ui {
class myFrame;
}

class myFrame : public QFrame
{
    Q_OBJECT

public:
    explicit myFrame(QWidget *parent = nullptr);
    ~myFrame();
    void setVersionLabel(QString text);

private:
    Ui::myFrame *ui;
    void paintEvent(QPaintEvent *e);
};

#endif // MYFRAME_H
