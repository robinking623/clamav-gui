#include "logviewobject.h"
#include "ui_logviewobject.h"

logViewObject::logViewObject(QWidget *parent, QString fileName) : QDialog(parent), ui(new Ui::logViewObject){
    setWindowFlags(((this->windowFlags() | Qt::CustomizeWindowHint) & ~Qt::WindowCloseButtonHint & ~Qt::WindowContextHelpButtonHint) );
    ui->setupUi(this);
    logFileName = fileName;
    loadLogFile(fileName);
    searchPosition = 0;
    contentChanged = false;
}

logViewObject::~logViewObject(){
    delete ui;
}

void logViewObject::closeEvent(QCloseEvent *event){
    this->accept();
    event->ignore();
}

void logViewObject::keyPressEvent(QKeyEvent * event){
    if (event->key() == Qt::Key_Escape) event->ignore(); else event->accept();
}

void logViewObject::loadLogFile(QString filename){
QFile m_file(filename);
QString m_buffer;
QStringList m_logs;
QString m_tabHeader;
setupFileHandler * m_sf = new setupFileHandler(QDir::homePath() + "/.clamav-gui/settings.ini",this);
bool m_css = m_sf->getSectionBoolValue("Setup","DisableLogHighlighter");

    while(ui->logTab->count() > 0){
        ui->logTab->removeTab(0);
    }

    if (m_file.open(QIODevice::ReadOnly)){
        QTextStream m_stream(&m_file);
        m_buffer = m_stream.readAll();
        m_logs = m_buffer.split("<Scanning startet>");

        for (int i = 1; i < m_logs.count(); i++){
            partialLogObject * log = new partialLogObject(this,m_logs[i],m_css);
            connect(this,SIGNAL(logHighlightingChanged(bool)),log,SLOT(slot_add_remove_highlighter(bool)));
            m_tabHeader = m_logs[i].mid(1,m_logs[i].indexOf("\n") - 1);
            ui->logTab->addTab(log,QIcon(":/icons/icons/information.png"),m_tabHeader);
        }

        m_file.close();
    }
}

void logViewObject::slot_closeButtonClicked(){
QString m_logText;
partialLogObject * m_log;

    if (contentChanged){
        if (QMessageBox::information(this,tr("INFO"),tr("Log-File was modified. Do you wanna save the changes?"),QMessageBox::Yes,QMessageBox::No) == QMessageBox::Yes){
            if (ui->logTab->count() > 0){
                for (int i = 0; i < ui->logTab->count(); i++){
                    m_log = (partialLogObject*)ui->logTab->widget(i);
                    m_logText = m_logText + m_log->getLogText();
                    QFile m_logFile(logFileName);
                    if (m_logFile.open(QIODevice::ReadWrite|QIODevice::Truncate|QIODevice::Text)){
                        QTextStream m_stream(&m_logFile);
                        m_stream << m_logText;
                        m_logFile.close();
                    }
                }
            } else {
                QFile m_logFile(logFileName);
                if (m_logFile.open(QIODevice::ReadWrite|QIODevice::Truncate|QIODevice::Text)){
                    QTextStream m_stream(&m_logFile);
                    m_stream << "";
                    m_logFile.close();
                }
            }
        }
    }
    this->accept();
}

void logViewObject::slot_clearLogButtonClicked(){
int m_currentTab= ui->logTab->currentIndex();

    if (m_currentTab > -1){
        if (QMessageBox::question(this,tr("Clear Log"),tr("Do you realy want to remove this partial log?"),QMessageBox::Yes,QMessageBox::No) == QMessageBox::Yes){
            ui->logTab->removeTab(m_currentTab);
            contentChanged = true;
            emit logChanged();
        }
    }
}

void logViewObject::slot_add_remove_highlighter(bool state)
{
    emit logHighlighterChanged(state);
}
