#include "clamav_ctrl.h"
#include "ui_clamav_ctrl.h"
#include "qscreen.h"

clamav_ctrl::clamav_ctrl(QWidget *parent) : QWidget(parent), ui(new Ui::clamav_ctrl)
{
    ui->setupUi(this);
    initWidget();
}

void clamav_ctrl::initWidget(){
QScreen *m_screen = QGuiApplication::primaryScreen();
QRect  screenGeometry = m_screen->geometry();
    int m_height = screenGeometry.height();
    int m_width = screenGeometry.width();

    this->setStyleSheet("background:transparent;");
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setWindowFlags(Qt::FramelessWindowHint|Qt::Tool|Qt::WindowStaysOnTopHint);
    this->move(m_width-this->width()-10,m_height-this->height()-10);
    this->setAcceptDrops(true);
}

clamav_ctrl::~clamav_ctrl()
{
    delete ui;
}

void clamav_ctrl::dragEnterEvent(QDragEnterEvent * event){
    event->acceptProposedAction();
}

void clamav_ctrl::dropEvent(QDropEvent * event){
QStringList m_list;
QList <QUrl> m_tempList;
QString m_tempString;

    if (event && event->mimeData()){
        const QMimeData * mimeData;
        mimeData = event->mimeData();
        if (mimeData->hasUrls()){
            m_tempList = mimeData->urls();
            for (int i = 0; i < m_tempList.count(); i++){
                m_tempString = m_tempList.at(i).toString();
                if (m_tempString.indexOf("file://") == 0){
                    m_list << m_tempString.mid(7);
                }
            }
            if (m_list.count() > 0) emit scanRequest(m_list);
        }
    }
}
