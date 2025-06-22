#include "dragablepushbutton.h"
#include "ui_dragablepushbutton.h"

dragablePushButton::dragablePushButton(QWidget *parent) : QPushButton(parent), ui(new Ui::dragablePushButton){
}

dragablePushButton::dragablePushButton(QIcon icon, QString text, QWidget *parent, QString dragText) : QPushButton(icon, text, parent){
    dragDropText = dragText;
}

dragablePushButton::~dragablePushButton(){
}

void dragablePushButton::mouseMoveEvent(QMouseEvent *event) {
QUrl m_url("file://" + dragDropText);
QList <QUrl> m_urls;
    event = event;

    m_urls << m_url;
    QDrag *m_drag = new QDrag(this);
    QMimeData *m_mimeData = new QMimeData;

    m_mimeData->setText("file://" + dragDropText);
    m_mimeData->setUrls(m_urls);

    m_drag->setMimeData(m_mimeData);
    m_drag->setPixmap(QPixmap(":/icons/icons/media.png"));

    emit dragStarted();

    m_drag->exec();
}
